//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/Protocol.h"

#include "comms_champion/property/message.h"

namespace comms_champion
{

Protocol::~Protocol() = default;

const QString& Protocol::name() const
{
    return nameImpl();
}

Protocol::MessagesList Protocol::read(
    const DataInfo& dataInfo,
    bool final)
{
    return readImpl(dataInfo, final);
}

Protocol::DataInfosList Protocol::write(const MessagesList& msgs)
{
    DataInfosList dataInfos;
    for (auto& msgPtr : msgs) {
        if (!msgPtr) {
            continue;
        }

        if (msgPtr->idAsString().isEmpty()) {

            auto rawDataMsg = property::message::RawDataMsg().getFrom(*msgPtr);
            if (!rawDataMsg) {
                continue;
            }

            auto dataInfoPtr = makeDataInfo();
            assert(dataInfoPtr);

            dataInfoPtr->m_timestamp = DataInfo::TimestampClock::now();
            dataInfoPtr->m_data = rawDataMsg->encodeData();

            if (!dataInfoPtr->m_data.empty()) {
                dataInfos.push_back(std::move(dataInfoPtr));
            }
            continue;
        }

        auto dataInfoPtr = writeImpl(*msgPtr);
        if (dataInfoPtr) {
            dataInfos.push_back(std::move(dataInfoPtr));
        }
    }

    return dataInfos;
}

Protocol::MessagesList Protocol::createAllMessages()
{
    return createAllMessagesImpl();
}

MessagePtr Protocol::createMessage(const QString& idAsString, unsigned idx)
{
    return createMessageImpl(idAsString, idx);
}

Protocol::UpdateStatus Protocol::updateMessage(Message& msg)
{
    return updateMessageImpl(msg);
}

MessagePtr Protocol::cloneMessage(const Message& msg)
{
    if (msg.idAsString().isEmpty()) {
        auto clonedMsg = createInvalidMessage();

        auto rawDataMsg = property::message::RawDataMsg().getFrom(msg);
        if (rawDataMsg) {
            auto clonedRawDataMsg = createRawDataMessage();
            if (!clonedRawDataMsg) {
                assert(!"Raw Data Message wasn't created properly");
                return clonedMsg;
            }

            auto data = rawDataMsg->encodeData();
            bool decodeResult = clonedRawDataMsg->decodeData(data);
            static_cast<void>(decodeResult);
            assert(decodeResult);
            setRawDataToMessageProperties(std::move(clonedRawDataMsg), *clonedMsg);
        }
        return clonedMsg;
    }

    auto clonedMsg = cloneMessageImpl(msg);
    if (clonedMsg) {
        updateMessage(*clonedMsg);
    }
    return clonedMsg;
}

MessagePtr Protocol::createInvalidMessage()
{
    return createInvalidMessageImpl();
}

MessagePtr Protocol::createRawDataMessage()
{
    return createRawDataMessageImpl();
}

void Protocol::setNameToMessageProperties(Message& msg)
{
    property::message::ProtocolName().setTo(name(), msg);
}

void Protocol::setTransportToMessageProperties(MessagePtr transportMsg, Message& msg)
{
    property::message::TransportMsg().setTo(std::move(transportMsg), msg);
}

void Protocol::setRawDataToMessageProperties(MessagePtr rawDataMsg, Message& msg)
{
    property::message::RawDataMsg().setTo(std::move(rawDataMsg), msg);
}


}  // namespace comms_champion


