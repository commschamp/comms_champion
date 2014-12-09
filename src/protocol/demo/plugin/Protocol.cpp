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

#include "message/CCHeartbeat.h"
#include "comms_champion/ErrorStatus.h"

#include "comms/util/ScopeGuard.h"

namespace demo
{

namespace plugin
{

namespace cc = comms_champion;

Protocol::~Protocol() = default;

cc::ErrorStatus Protocol::read(
        MsgPtr& msg,
        ReadIterType& iter,
        std::size_t size,
        std::size_t* missingSize)
{
    using ProtocolMsgPtr = ProtocolStack::MsgPtr;
    ProtocolMsgPtr msgPtr;
    auto es = m_protStack.read(msgPtr, iter, size, missingSize);
    if (es != comms::ErrorStatus::Success) {
        return comms_champion::transformErrorStatus(es);
    }

    assert(msgPtr);
    msg.reset(msgPtr.release());
    return cc::ErrorStatus::Success;
}

Protocol::MessagesList Protocol::readImpl(
    ReadIterType iter,
    std::size_t size,
    std::size_t* missingSize)
{
    MessagesList allInfos;
    m_data.reserve(m_data.size() + size);
    std::copy_n(iter, size, std::back_inserter(m_data));

    using ReadIterator = ProtocolStack::ReadIterator;
    ReadIterator readIter = &m_data[0];

    auto eraseGuard =
        comms::util::makeScopeGuard(
            [this, &readIter]()
            {
                auto dist = std::distance(ReadIterator(&m_data[0]), readIter);
                m_data.erase(m_data.begin(), m_data.begin() + dist);
            });

    while (true) {
        using AllFields = ProtocolStack::AllFields;
        using ProtocolMsgPtr = ProtocolStack::MsgPtr;

        AllFields fields;
        ProtocolMsgPtr msgPtr;

        auto readIterTmp = readIter;

        auto es = m_protStack.readFieldsCached<0>(
                    fields, msgPtr, readIterTmp, m_data.size(), missingSize);
        if (es == comms::ErrorStatus::NotEnoughData) {
            break;
        }

        using MessageInfoMsgPtr = comms_champion::MessageInfo::MessagePtr;
        if (es == comms::ErrorStatus::Success) {
            auto msgInfo = comms_champion::makeMessageInfo();
            msgInfo->setAppMessage(MessageInfoMsgPtr(std::move(msgPtr)));
            assert(msgInfo->getAppMessage());
            // TODO: setTransportMessage
            allInfos.push_back(std::move(msgInfo));
            readIter = readIterTmp;
            continue;
        }

        if (es == comms::ErrorStatus::InvalidMsgData) {
            readIter = readIterTmp;
            // TODO: setTransportMessage
            continue;
        }

        if (es == comms::ErrorStatus::MsgAllocFaulure) {
            assert(!"Mustn't happen");
            break;
        }

        // Protocol error:
        ++readIter;
        while (readIter != &m_data[m_data.size()]) {
            auto readIterTmp = readIter;
            auto diff = static_cast<std::size_t>(std::distance(ReadIterator(&m_data[0]), readIterTmp));
            es = m_protStack.read(msgPtr, readIterTmp, m_data.size() - diff);
            if ((es != comms::ErrorStatus::ProtocolError) &&
                (es != comms::ErrorStatus::InvalidMsgId)) {
                // TODO: setData
                readIter = readIterTmp;
                break;
            }
            ++readIter;
        }
    }

    return allInfos;
}

}  // namespace plugin

}  // namespace demo

