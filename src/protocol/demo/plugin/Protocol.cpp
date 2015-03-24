//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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


#include <cassert>
#include <algorithm>
#include <iterator>
#include "Protocol.h"

#include "comms_champion/ErrorStatus.h"
#include "comms/util/ScopeGuard.h"

#include "message/CCTransportMessage.h"
#include "message/CCRawDataMessage.h"

namespace demo
{

namespace plugin
{

namespace cc = comms_champion;

Protocol::~Protocol() = default;

const std::string& Protocol::nameImpl() const
{
    static std::string Name("Demo");
    return Name;
}

Protocol::MessagesList Protocol::readImpl(
    const cc::DataInfo& dataInfo)
{
    const std::uint8_t* iter = &dataInfo.m_data[0];
    auto size = dataInfo.m_data.size();

    MessagesList allInfos;
    m_data.reserve(m_data.size() + size);
    std::copy_n(iter, size, std::back_inserter(m_data));

    using ReadIterator = ProtocolStack::ReadIterator;
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
        using AllFields = ProtocolStack::AllFields;
        using ProtocolMsgPtr = ProtocolStack::MsgPtr;

        AllFields fields;
        ProtocolMsgPtr msgPtr;

        auto readIterCur = readIterBeg;
        auto es =
            m_protStack.readFieldsCached<0>(
                fields,
                msgPtr,
                readIterCur,
                remainingSizeCalc(readIterCur));

        if (es == comms::ErrorStatus::NotEnoughData) {
            break;
        }

        using MessageInfoMsgPtr = cc::MessageInfo::MessagePtr;
        auto msgInfo = cc::makeMessageInfo();
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
                std::unique_ptr<message::CCTransportMessage> transportMsgPtr(
                                    new message::CCTransportMessage());
                transportMsgPtr->setFields(fields);
                msgInfo->setTransportMessage(MessageInfoMsgPtr(transportMsgPtr.release()));
            };

        auto setRawDataMsgFunc =
            [readIterBeg, &readIterCur, &msgInfo]()
            {
                auto readIterBegTmp = readIterBeg;
                // readIterBeg is captured by value on purpose
                std::unique_ptr<message::CCRawDataMessage> rawDataMsgPtr(
                                    new message::CCRawDataMessage());
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
                    auto garbageMsgInfo = cc::makeMessageInfo();
                    garbageMsgInfo->setProtocolName(name());
                    std::unique_ptr<message::CCRawDataMessage> rawDataMsgPtr(
                                        new message::CCRawDataMessage());
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

        // TODO: check error

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

Protocol::DataInfosList Protocol::writeImpl(const MessagesList& msgs)
{
    DataInfosList dataList;
    for (auto& msgInfo : msgs) {
        auto msgPtr = msgInfo->getAppMessage();
        assert(msgPtr);

        cc::DataInfo::DataSeq data;
        auto writeIter = std::back_inserter(data);
        auto es =
            m_protStack.write(
                static_cast<const CCDemoMessage&>(*msgPtr),
                writeIter,
                data.max_size());
        if (es == comms::ErrorStatus::UpdateRequired) {
            auto updateIter = &data[0];
            es = m_protStack.update(updateIter, data.size());
        }

        assert(es == comms::ErrorStatus::Success);
        static_cast<void>(es);

        auto dataInfo = cc::makeDataInfo();
        assert(dataInfo);

        dataInfo->m_timestamp = cc::DataInfo::TimestampClock::now();
        dataInfo->m_data = std::move(data);

        dataList.push_back(std::move(dataInfo));
    }
    return dataList;
}

Protocol::MessagesList Protocol::createAllMessagesImpl()
{
    MessagesList allInfos;

    for (auto idx = 0; idx < demo::message::MsgId_NumOfMessages; ++idx) {
        auto id = static_cast<demo::message::MsgId>(idx);
        auto msgInfo = createMessage(id);
        allInfos.push_back(std::move(msgInfo));
    }
    return allInfos;
}

cc::MessageInfoPtr Protocol::createMessageImpl(const QString& idAsString)
{
    cc::MessageInfoPtr result;
    do {
        bool ok = false;
        int numId = idAsString.toInt(&ok, 10);
        if (!ok) {
            numId = idAsString.toInt(&ok, 16);
            if (!ok) {
                break;
            }
        }

        if ((numId < 0) ||
            (demo::message::MsgId_NumOfMessages <= numId)) {
            break;
        }

        result = createMessage(static_cast<demo::message::MsgId>(numId));
    } while (false);
    return result;
}

void Protocol::updateMessageInfoImpl(comms_champion::MessageInfo& msgInfo)
{
    auto msgPtr = msgInfo.getAppMessage();
    assert(msgPtr);

    using AllFields = ProtocolStack::AllFields;
    AllFields fields;
    std::vector<std::uint8_t> data;

    auto writeIter = std::back_inserter(data);
    auto es =
        m_protStack.writeFieldsCached<0>(
            fields,
            static_cast<const CCDemoMessage&>(*msgPtr),
            writeIter,
            data.max_size());
    if (es == comms::ErrorStatus::UpdateRequired) {
        auto updateIter = &data[0];
        es = m_protStack.updateFieldsCached<0>(fields, updateIter, data.size());
    }

    assert(es == comms::ErrorStatus::Success);
    static_cast<void>(es);

    std::unique_ptr<message::CCTransportMessage> transportMsgPtr(
                        new message::CCTransportMessage());
    transportMsgPtr->setFields(fields);

    std::unique_ptr<message::CCRawDataMessage> rawDataMsgPtr(
                        new message::CCRawDataMessage());
    message::CCRawDataMessage::ReadIterator rawDataReadIter = &data[0];
    es = rawDataMsgPtr->read(rawDataReadIter, data.size());
    static_cast<void>(es);
    assert(es == comms::ErrorStatus::Success);

    using MessageInfoMsgPtr = cc::MessageInfo::MessagePtr;
    msgInfo.setTransportMessage(MessageInfoMsgPtr(transportMsgPtr.release()));
    msgInfo.setRawDataMessage(MessageInfoMsgPtr(rawDataMsgPtr.release()));
}

cc::MessageInfoPtr Protocol::cloneMessageImpl(
        const cc::MessageInfo& msgInfo)
{
    auto appMsgPtr = msgInfo.getAppMessage();
    assert(appMsgPtr);
    auto* demoAppMsg = dynamic_cast<CCDemoMessage*>(appMsgPtr.get());
    if (demoAppMsg == nullptr) {
        assert(!"Invalid message provided for cloning");
        return cc::MessageInfoPtr();
    }

    auto msgId = demoAppMsg->getId();
    auto clonedAppMsg = m_protStack.createMsg(msgId);
    clonedAppMsg->assign(*demoAppMsg);
    assert(clonedAppMsg);

    auto clonedMsgInfo = cc::makeMessageInfo();
    clonedMsgInfo->setAppMessage(
        cc::MessageInfo::MessagePtr(std::move(clonedAppMsg)));

    updateMessageInfoImpl(*clonedMsgInfo);
    clonedMsgInfo->setProtocolName(name());
    assert(clonedMsgInfo->getTransportMessage());
    assert(clonedMsgInfo->getRawDataMessage());

    return clonedMsgInfo;
}

cc::MessageInfoPtr Protocol::createMessage(demo::message::MsgId id)
{
    assert(id < demo::message::MsgId_NumOfMessages);
    auto msgInfo = cc::makeMessageInfo();
    auto msgPtr = m_protStack.createMsg(id);
    assert(msgPtr);

    using MessageInfoMsgPtr = cc::MessageInfo::MessagePtr;
    msgInfo->setProtocolName(name());
    msgInfo->setAppMessage(MessageInfoMsgPtr(std::move(msgPtr)));
    updateMessageInfo(*msgInfo);
    return msgInfo;
}

}  // namespace plugin

}  // namespace demo

