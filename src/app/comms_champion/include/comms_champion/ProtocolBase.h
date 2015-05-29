//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <algorithm>
#include <iterator>

#include "comms_champion/ErrorStatus.h"
#include "comms/util/ScopeGuard.h"
#include "comms/util/Tuple.h"

#include "Protocol.h"
#include "MessageInfo.h"

namespace comms_champion
{

template <typename TProtStack, typename TTransportMsg, typename TRawDataMsg>
class ProtocolBase : public Protocol
{
protected:
    ProtocolBase() = default;

    typedef TProtStack ProtocolStack;
    typedef TTransportMsg TransportMsg;
    typedef TRawDataMsg RawDataMsg;
    typedef typename ProtocolStack::Message Message;
    typedef typename Message::MsgIdType MsgIdType;
    typedef typename Message::MsgIdParamType MsgIdParamType;
    typedef typename ProtocolStack::AllMessages AllMessages;

    static_assert(
        !std::is_void<AllMessages>::value,
        "AllMessages must be a normal type");

    static_assert(
        comms::util::IsTuple<AllMessages>::Value,
        "AllMessages is expected to be a tuple.");


    virtual MessagesList readImpl(const DataInfo& dataInfo)
    {
        const std::uint8_t* iter = &dataInfo.m_data[0];
        auto size = dataInfo.m_data.size();

        MessagesList allInfos;
        m_data.reserve(m_data.size() + size);
        std::copy_n(iter, size, std::back_inserter(m_data));

        using ReadIterator = typename ProtocolStack::ReadIterator;
        ReadIterator readIterBeg = &m_data[0];

        auto remainingSizeCalc =
            [this](ReadIterator iter) -> std::size_t
            {
                ReadIterator const dataBegin = &m_data[0];
                auto consumed =
                    static_cast<std::size_t>(
                        std::distance(dataBegin, iter));
                assert(consumed <= m_data.size());
                return m_data.size() - consumed;
            };

        auto eraseGuard =
            comms::util::makeScopeGuard(
                [this, &readIterBeg]()
                {
                    ReadIterator dataBegin = &m_data[0];
                    auto dist =
                        static_cast<std::size_t>(
                            std::distance(dataBegin, readIterBeg));
                    m_data.erase(m_data.begin(), m_data.begin() + dist);
                });

        while (true) {
            using AllFields = typename ProtocolStack::AllFields;
            using ProtocolMsgPtr = typename ProtocolStack::MsgPtr;

            AllFields fields;
            ProtocolMsgPtr msgPtr;

            auto readIterCur = readIterBeg;
            auto es =
                m_protStack.template readFieldsCached<0>(
                    fields,
                    msgPtr,
                    readIterCur,
                    remainingSizeCalc(readIterCur));

            if (es == comms::ErrorStatus::NotEnoughData) {
                break;
            }

            using MessageInfoMsgPtr = MessageInfo::MessagePtr;
            auto msgInfo = makeMessageInfo();
            msgInfo->setProtocolName(name());

            auto addMsgInfoGuard =
                comms::util::makeScopeGuard(
                    [&allInfos, &msgInfo]()
                    {
                        allInfos.push_back(std::move(msgInfo));
                    });

            auto setTransportMsgFunc =
                [&fields, &msgInfo]()
                {
                    std::unique_ptr<TransportMsg> transportMsgPtr(new TransportMsg());
                    transportMsgPtr->setFields(fields);
                    msgInfo->setTransportMessage(MessageInfoMsgPtr(transportMsgPtr.release()));
                };

            auto setRawDataMsgFunc =
                [readIterBeg, &readIterCur, &msgInfo]()
                {
                    auto readIterBegTmp = readIterBeg;
                    // readIterBeg is captured by value on purpose
                    std::unique_ptr<RawDataMsg> rawDataMsgPtr(new RawDataMsg());
                    auto dataSize = static_cast<std::size_t>(
                                    std::distance(readIterBegTmp, readIterCur));
                    auto es = rawDataMsgPtr->read(readIterBegTmp, dataSize);
                    static_cast<void>(es);
                    assert(es == comms::ErrorStatus::Success);
                    msgInfo->setRawDataMessage(MessageInfoMsgPtr(rawDataMsgPtr.release()));
                };

            auto checkGarbageFunc =
                [this, &allInfos]()
                {
                    if (!m_garbage.empty()) {
                        auto garbageMsgInfo = makeMessageInfo();
                        garbageMsgInfo->setProtocolName(name());
                        std::unique_ptr<RawDataMsg> rawDataMsgPtr(new RawDataMsg());
                        ReadIterator garbageReadIterator = &m_garbage[0];
                        auto es = rawDataMsgPtr->read(garbageReadIterator, m_garbage.size());
                        static_cast<void>(es);
                        assert(es == comms::ErrorStatus::Success);
                        garbageMsgInfo->setRawDataMessage(MessageInfoMsgPtr(rawDataMsgPtr.release()));
                        allInfos.push_back(std::move(garbageMsgInfo));
                        m_garbage.clear();
                    }
                };

            if (es == comms::ErrorStatus::Success) {
                checkGarbageFunc();
                assert(msgPtr);
                msgInfo->setAppMessage(MessageInfoMsgPtr(std::move(msgPtr)));
                assert(msgInfo->getAppMessage());

                setTransportMsgFunc();
                setRawDataMsgFunc();
                readIterBeg = readIterCur;
                continue;
            }

            if (es == comms::ErrorStatus::InvalidMsgData) {
                checkGarbageFunc();
                setTransportMsgFunc();
                setRawDataMsgFunc();
                readIterBeg = readIterCur;
                continue;
            }

            addMsgInfoGuard.release();

            if (es == comms::ErrorStatus::MsgAllocFaulure) {
                assert(!"Mustn't happen");
                break;
            }

            // Protocol error
            m_garbage.push_back(*readIterBeg);
            static const std::size_t GarbageLimit = 512;
            if (GarbageLimit <= m_garbage.size()) {
                checkGarbageFunc();
            }
            ++readIterBeg;
        }

        return allInfos;
    }

    virtual DataInfosList writeImpl(const MessagesList& msgs) override
    {
        DataInfosList dataList;
        for (auto& msgInfo : msgs) {
            auto msgPtr = msgInfo->getAppMessage();
            assert(msgPtr);

            DataInfo::DataSeq data;
            auto writeIter = std::back_inserter(data);
            auto es =
                m_protStack.write(
                    static_cast<const Message&>(*msgPtr),
                    writeIter,
                    data.max_size());
            if (es == comms::ErrorStatus::UpdateRequired) {
                auto updateIter = &data[0];
                es = m_protStack.update(updateIter, data.size());
            }

            if (es != comms::ErrorStatus::Success) {
                assert(!"Unexpected write/update failure");
                break;
            }

            auto dataInfo = makeDataInfo();
            assert(dataInfo);

            dataInfo->m_timestamp = DataInfo::TimestampClock::now();
            dataInfo->m_data = std::move(data);

            dataList.push_back(std::move(dataInfo));
        }
        return dataList;
    }

    virtual UpdateStatus updateMessageInfoImpl(MessageInfo& msgInfo) override
    {
        do {
            auto msgPtr = msgInfo.getAppMessage();
            assert(msgPtr);

            using AllFields = typename ProtocolStack::AllFields;
            AllFields fields;
            std::vector<std::uint8_t> data;

            auto writeIter = std::back_inserter(data);
            auto es =
                m_protStack.template writeFieldsCached<0>(
                    fields,
                    static_cast<const Message&>(*msgPtr),
                    writeIter,
                    data.max_size());
            if (es == comms::ErrorStatus::UpdateRequired) {
                auto updateIter = &data[0];
                es = m_protStack.template updateFieldsCached<0>(fields, updateIter, data.size());
            }

            if (es != comms::ErrorStatus::Success) {
                assert(!"Message write/update has failed unexpectedly");
                break;
            }

            std::unique_ptr<TransportMsg> transportMsgPtr(new TransportMsg());
            transportMsgPtr->setFields(fields);

            std::unique_ptr<RawDataMsg> rawDataMsgPtr(new RawDataMsg());

            typedef typename RawDataMsg::ReadIterator ReadIterator;
            ReadIterator rawDataReadIter = &data[0];
            es = rawDataMsgPtr->read(rawDataReadIter, data.size());
            if (es != comms::ErrorStatus::Success) {
                assert(!"Unexpected failure to read raw data of the message");
                break;
            }

            using MessageInfoMsgPtr = MessageInfo::MessagePtr;
            msgInfo.setTransportMessage(MessageInfoMsgPtr(transportMsgPtr.release()));
            msgInfo.setRawDataMessage(MessageInfoMsgPtr(rawDataMsgPtr.release()));
        } while (false);

        return UpdateStatus::NoChangeToAppMsg;
    }

    virtual MessageInfoPtr cloneMessageImpl(
            const MessageInfo& msgInfo) override
    {
        auto appMsgPtr = msgInfo.getAppMessage();
        assert(appMsgPtr);
        auto* actualAppMsg = dynamic_cast<Message*>(appMsgPtr.get());
        if (actualAppMsg == nullptr) {
            assert(!"Invalid message provided for cloning");
            return MessageInfoPtr();
        }

        auto msgId = actualAppMsg->getId();
        auto clonedAppMsg = m_protStack.createMsg(msgId);
        clonedAppMsg->assign(*actualAppMsg);
        assert(clonedAppMsg);

        auto clonedMsgInfo = makeMessageInfo();
        clonedMsgInfo->setAppMessage(
            MessageInfo::MessagePtr(std::move(clonedAppMsg)));

        updateMessageInfo(*clonedMsgInfo);
        clonedMsgInfo->setProtocolName(name());
        assert(clonedMsgInfo->getTransportMessage());
        assert(clonedMsgInfo->getRawDataMessage());

        return clonedMsgInfo;
    }

    ProtocolStack& protocolStack()
    {
        return m_protStack;
    }

    const ProtocolStack& protocolStack() const
    {
        return m_protStack;
    }

    MessageInfoPtr createMessage(MsgIdParamType id)
    {
        auto msgInfo = makeMessageInfo();
        auto msgPtr = m_protStack.createMsg(id);
        if (!msgPtr) {
            return MessageInfoPtr();
        }

        using MessageInfoMsgPtr = MessageInfo::MessagePtr;
        msgInfo->setProtocolName(name());
        msgInfo->setAppMessage(MessageInfoMsgPtr(std::move(msgPtr)));
        updateMessageInfo(*msgInfo);
        return msgInfo;
    }

    virtual MessagesList createAllMessagesImpl() override
    {
        MessagesList allMsgs;
        comms::util::tupleForEachType<AllMessages>(AllMsgsCreateHelper(name(), allMsgs));
        for (auto& msgInfoPtr : allMsgs) {
            updateMessageInfo(*msgInfoPtr);
        }
        return allMsgs;
    }

    virtual MessageInfoPtr createMessageImpl(const QString& idAsString) override
    {
        return createMessageInternal(idAsString, MsgIdTypeTag());
    }

private:
    struct NumericIdTag {};
    struct OtherIdTag {};

    typedef typename std::conditional<
        (std::is_enum<MsgIdType>::value || std::is_integral<MsgIdType>::value),
        NumericIdTag,
        OtherIdTag
    >::type MsgIdTypeTag;

    class AllMsgsCreateHelper
    {
    public:
        AllMsgsCreateHelper(const std::string& protName, MessagesList& allMsgs)
          : m_protName(protName),
            m_allMsgs(allMsgs)
        {
        }

        template <typename TMsg>
        void operator()()
        {
            MessageInfo::MessagePtr msgPtr(new TMsg());
            auto msgInfo = makeMessageInfo();
            assert(msgInfo);
            msgInfo->setProtocolName(m_protName);
            msgInfo->setAppMessage(std::move(msgPtr));
            m_allMsgs.push_back(std::move(msgInfo));
        }

    private:
        const std::string& m_protName;
        MessagesList& m_allMsgs;
    };

    class MsgCreateHelper
    {
    public:
        MsgCreateHelper(const std::string& protName, const QString& id, MessageInfoPtr& msgInfo)
          : m_protName(protName),
            m_id(id),
            m_msgInfo(msgInfo)
        {
        }

        template <typename TMsg>
        void operator()()
        {
            if (m_msgInfo) {
                return;
            }

            MessageInfo::MessagePtr msgPtr(new TMsg());
            if (m_id == msgPtr->idAsString()) {
                m_msgInfo = makeMessageInfo();
                m_msgInfo->setProtocolName(m_protName);
                m_msgInfo->setAppMessage(std::move(msgPtr));
            }
        }

    private:
        const std::string& m_protName;
        const QString& m_id;
        MessageInfoPtr& m_msgInfo;
    };

    MessageInfoPtr createMessageInternal(const QString& idAsString, NumericIdTag)
    {
        MessageInfoPtr result;
        do {
            bool ok = false;
            int numId = idAsString.toInt(&ok, 10);
            if (!ok) {
                numId = idAsString.toInt(&ok, 16);
                if (!ok) {
                    break;
                }
            }

            result = createMessage(static_cast<MsgIdType>(numId));
        } while (false);
        return result;
    }

    MessageInfoPtr createMessageInternal(const QString& idAsString, OtherIdTag)
    {
        MessageInfoPtr result;
        comms::util::tupleForEachType(MsgCreateHelper(name(), idAsString, result));
        if (result) {
            updateMessageInfo(*result);
        }
        return result;
    }

    ProtocolStack m_protStack;
    std::vector<std::uint8_t> m_data;
    std::vector<std::uint8_t> m_garbage;
};

}  // namespace comms_champion


