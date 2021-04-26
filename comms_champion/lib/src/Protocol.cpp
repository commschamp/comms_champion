//
// Copyright 2016 - 2021 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QByteArray>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"

namespace comms_champion
{

Protocol::~Protocol() noexcept = default;

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

DataInfoPtr Protocol::write(Message& msg)
{

    if (msg.idAsString().isEmpty()) {

        auto rawDataMsg = property::message::RawDataMsg().getFrom(msg);
        if (!rawDataMsg) {
            return DataInfoPtr();
        }

        auto dataInfoPtr = makeDataInfo();
        assert(dataInfoPtr);

        dataInfoPtr->m_timestamp = DataInfo::TimestampClock::now();
        dataInfoPtr->m_data = rawDataMsg->encodeData();

        return dataInfoPtr;
    }

    return writeImpl(msg);
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
    if (!msg.idAsString().isEmpty()) {
        return updateMessageImpl(msg);
    }

    auto extraInfo = getExtraInfoFromMessageProperties(msg);
    if (extraInfo.isEmpty()) {
        if (!property::message::ExtraInfoMsg().getFrom(msg)) {
            return UpdateStatus::NoChange;
        }

        setExtraInfoMsgToMessageProperties(MessagePtr(), msg);
        return UpdateStatus::NoChange;
    }

    auto infoMsg = createExtraInfoMessageImpl();
    if (!infoMsg) {
        static constexpr bool Info_must_be_created = false;
        static_cast<void>(Info_must_be_created);
        assert(Info_must_be_created);        
        return UpdateStatus::NoChange;
    }

    auto jsonObj = QJsonObject::fromVariantMap(extraInfo);
    QJsonDocument doc(jsonObj);
    auto jsonByteArray = doc.toJson();
    MsgDataSeq dataSeq;
    dataSeq.reserve(static_cast<std::size_t>(jsonByteArray.size()));
    std::copy_n(jsonByteArray.constData(), jsonByteArray.size(), std::back_inserter(dataSeq));
    if (!infoMsg->decodeData(dataSeq)) {
        setExtraInfoMsgToMessageProperties(MessagePtr(), msg);
        return UpdateStatus::NoChange;
    }

    setExtraInfoMsgToMessageProperties(std::move(infoMsg), msg);
    return UpdateStatus::NoChange;
}

MessagePtr Protocol::cloneMessage(const Message& msg)
{
    if (msg.idAsString().isEmpty()) {
        MessagePtr clonedMsg;

        auto rawDataMsg = property::message::RawDataMsg().getFrom(msg);
        if (rawDataMsg) {
            auto data = rawDataMsg->encodeData();
            clonedMsg = createInvalidMessage(data);
        }
        else {
            clonedMsg = createInvalidMessageImpl();
        }

        if (!clonedMsg) {
            return clonedMsg;
        }

        auto extraInfoMap = getExtraInfoFromMessageProperties(msg);
        if (!extraInfoMap.isEmpty()) {
            setExtraInfoToMessageProperties(extraInfoMap, *clonedMsg);
            updateMessage(*clonedMsg);
        }

        return clonedMsg;
    }

    auto clonedMsg = cloneMessageImpl(msg);
    if (clonedMsg) {
        setNameToMessageProperties(*clonedMsg);
        updateMessage(*clonedMsg);
        property::message::ExtraInfo().copyFromTo(msg, *clonedMsg);
    }
    return clonedMsg;
}

MessagePtr Protocol::createInvalidMessage(const MsgDataSeq& data)
{
    auto rawDataMsg = createRawDataMessageImpl();
    if (!rawDataMsg) {
        return MessagePtr();
    }

    if (!rawDataMsg->decodeData(data)) {
        return MessagePtr();
    }

    auto invalidMsg = createInvalidMessageImpl();
    if (!invalidMsg) {
        return invalidMsg;
    }

    setRawDataToMessageProperties(std::move(rawDataMsg), *invalidMsg);
    return invalidMsg;
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

void Protocol::setExtraInfoMsgToMessageProperties(MessagePtr extraInfoMsg, Message& msg)
{
    property::message::ExtraInfoMsg().setTo(std::move(extraInfoMsg), msg);
}

MessagePtr Protocol::getExtraInfoMsgToMessageProperties(const Message& msg)
{
    return property::message::ExtraInfoMsg().getFrom(msg);
}

QVariantMap Protocol::getExtraInfoFromMessageProperties(const Message& msg)
{
    return property::message::ExtraInfo().getFrom(msg);
}

void Protocol::setExtraInfoToMessageProperties(
    const QVariantMap& extraInfo,
    Message& msg)
{
    property::message::ExtraInfo().setTo(extraInfo, msg);
}

void Protocol::mergeExtraInfoToMessageProperties(
    const QVariantMap& extraInfo,
    Message& msg)
{
    auto map = getExtraInfoFromMessageProperties(msg);
    for (auto& key : extraInfo.keys()) {
        map.insert(key, extraInfo.value(key));
    }
    setExtraInfoToMessageProperties(map, msg);
}

void Protocol::setForceExtraInfoExistenceToMessageProperties(Message& msg)
{
    property::message::ForceExtraInfoExistence().setTo(true, msg);
}

bool Protocol::getForceExtraInfoExistenceFromMessageProperties(const Message& msg)
{
    return property::message::ForceExtraInfoExistence().getFrom(msg);
}


}  // namespace comms_champion


