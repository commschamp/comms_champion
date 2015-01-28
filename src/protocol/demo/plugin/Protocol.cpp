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

        auto advanceReadIterGuard
            = comms::util::makeScopeGuard(
                [&readIterBeg, &readIterCur]()
                {
                    readIterBeg = readIterCur;
                });

        auto addMsgInfoGuard =
            comms::util::makeScopeGuard(
                [&allInfos, &msgInfo]()
                {
                    allInfos.push_back(std::move(msgInfo));
                });

        auto setTransportMsgGuard =
            comms::util::makeScopeGuard(
                [&fields, &msgInfo]()
                {
                    std::unique_ptr<message::CCTransportMessage> transportMsgPtr(
                                        new message::CCTransportMessage());
                    transportMsgPtr->setFields(fields);
                    msgInfo->setTransportMessage(MessageInfoMsgPtr(transportMsgPtr.release()));
                });

        auto setRawDataMsgGuard =
            comms::util::makeScopeGuard(
                [readIterBeg, &readIterCur, msgInfo]()
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
                });

        if (es == comms::ErrorStatus::Success) {
            assert(msgPtr);
            msgInfo->setAppMessage(MessageInfoMsgPtr(std::move(msgPtr)));
            assert(msgInfo->getAppMessage());
            continue;
        }

        if (es == comms::ErrorStatus::InvalidMsgData) {
            continue;
        }

        if (es == comms::ErrorStatus::MsgAllocFaulure) {
            assert(!"Mustn't happen");
            setRawDataMsgGuard.release();
            setTransportMsgGuard.release();
            addMsgInfoGuard.release();
            break;
        }

        // Protocol error, no transport message,
        setTransportMsgGuard.release();

        while (true) {
            ++readIterBeg;
            if (&m_data[m_data.size()] <= readIterBeg) {
                break;
            }

            readIterCur = readIterBeg;
            es = m_protStack.read(msgPtr, readIterCur, remainingSizeCalc(readIterCur));
            if ((es != comms::ErrorStatus::ProtocolError) &&
                (es != comms::ErrorStatus::InvalidMsgId)) {
                break;
            }
        }
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

    using MessageInfoMsgPtr = cc::MessageInfo::MessagePtr;
    for (auto idx = 0; idx < demo::message::MsgId_NumOfMessages; ++idx) {
        auto id = static_cast<demo::message::MsgId>(idx);
        auto msgInfo = cc::makeMessageInfo();
        auto msgPtr = m_protStack.createMsg(id);
        assert(msgPtr);

        msgInfo->setProtocolName(name());
        msgInfo->setAppMessage(MessageInfoMsgPtr(std::move(msgPtr)));
        updateMessageInfo(*msgInfo);

        allInfos.push_back(std::move(msgInfo));
    }
    return allInfos;
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

}  // namespace plugin

}  // namespace demo

