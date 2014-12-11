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

#include "CCTransportMessage.h"

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
    ReadIterType iter,
    std::size_t size,
    std::size_t* missingSize)
{
    MessagesList allInfos;
    m_data.reserve(m_data.size() + size);
    std::copy_n(iter, size, std::back_inserter(m_data));

    using ReadIterator = ProtocolStack::ReadIterator;
    ReadIterator readIter = &m_data[0];

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
            [this, &readIter]()
            {
                ReadIterator const dataBegin = &m_data[0];
                auto dist = std::distance(dataBegin, readIter);
                assert(static_cast<std::size_t>(dist) <= m_data.size());
                m_data.erase(m_data.begin(), m_data.begin() + dist);
            });

    while (true) {
        using AllFields = ProtocolStack::AllFields;
        using ProtocolMsgPtr = ProtocolStack::MsgPtr;

        AllFields fields;
        ProtocolMsgPtr msgPtr;

        auto readIterTmp = readIter;
        auto dist = std::distance(ReadIterator(&m_data[0]), readIter);
        auto remainingSize = m_data.size() - static_cast<std::size_t>(dist);

        auto es =
            m_protStack.readFieldsCached<0>(
                fields,
                msgPtr,
                readIterTmp,
                remainingSizeCalc(readIterTmp),
                missingSize);

        if (es == comms::ErrorStatus::NotEnoughData) {
            break;
        }

        using MessageInfoMsgPtr = cc::MessageInfo::MessagePtr;
        auto msgInfo = cc::makeMessageInfo();
        auto setTransportMsgFunc =
            [&fields, &msgInfo]()
            {
                std::unique_ptr<CCTransportMessage> transportMsgPtr(new CCTransportMessage());
                transportMsgPtr->setFields(fields);
                msgInfo->setTransportMessage(MessageInfoMsgPtr(transportMsgPtr.release()));
            };

        if (es == comms::ErrorStatus::Success) {
            msgInfo->setAppMessage(MessageInfoMsgPtr(std::move(msgPtr)));
            assert(msgInfo->getAppMessage());
            setTransportMsgFunc();
            allInfos.push_back(std::move(msgInfo));
            readIter = readIterTmp;
            continue;
        }

        if (es == comms::ErrorStatus::InvalidMsgData) {
            readIter = readIterTmp;
            setTransportMsgFunc();
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
            es = m_protStack.read(msgPtr, readIterTmp, remainingSizeCalc(readIterTmp));
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

