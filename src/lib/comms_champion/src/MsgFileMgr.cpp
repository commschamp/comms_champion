//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "comms_champion/MsgFileMgr.h"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <iostream>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QVariantMap>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"

namespace comms_champion
{

namespace
{

class IdProp : public property::message::PropBase<QString>
{
    typedef property::message::PropBase<QString> Base;
public:
    IdProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString IdProp::Name("id");
const QByteArray IdProp::PropName = IdProp::Name.toUtf8();

class DataProp : public property::message::PropBase<QString>
{
    typedef property::message::PropBase<QString> Base;
public:
    DataProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString DataProp::Name("data");
const QByteArray DataProp::PropName = DataProp::Name.toUtf8();

class DelayProp : public property::message::PropBase<unsigned long long>
{
    typedef property::message::PropBase<unsigned long long> Base;
public:
    DelayProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString DelayProp::Name("delay");
const QByteArray DelayProp::PropName = DelayProp::Name.toUtf8();

class DelayUnitsProp : public property::message::PropBase<QString>
{
    typedef property::message::PropBase<QString> Base;
public:
    DelayUnitsProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString DelayUnitsProp::Name("delay_units");
const QByteArray DelayUnitsProp::PropName = DelayUnitsProp::Name.toUtf8();

class RepeatProp : public property::message::PropBase<unsigned long long>
{
    typedef property::message::PropBase<unsigned long long> Base;
public:
    RepeatProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString RepeatProp::Name("repeat");
const QByteArray RepeatProp::PropName = RepeatProp::Name.toUtf8();

class RepeatUnitsProp : public property::message::PropBase<QString>
{
    typedef property::message::PropBase<QString> Base;
public:
    RepeatUnitsProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString RepeatUnitsProp::Name("repeat_units");
const QByteArray RepeatUnitsProp::PropName = RepeatUnitsProp::Name.toUtf8();

class RepeatCountProp : public property::message::PropBase<unsigned>
{
    typedef property::message::PropBase<unsigned> Base;
public:
    RepeatCountProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString RepeatCountProp::Name("repeat_count");
const QByteArray RepeatCountProp::PropName = RepeatCountProp::Name.toUtf8();

class TimestampProp : public property::message::PropBase<unsigned long long>
{
    typedef property::message::PropBase<unsigned long long> Base;
public:
    TimestampProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString TimestampProp::Name("timestamp");
const QByteArray TimestampProp::PropName = TimestampProp::Name.toUtf8();

class TypeProp : public property::message::PropBase<unsigned>
{
    typedef property::message::PropBase<unsigned> Base;
public:
    TypeProp() : Base(Name, PropName) {}
private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString TypeProp::Name("type");
const QByteArray TypeProp::PropName = TypeProp::Name.toUtf8();


QString encodeMsgData(MsgFileMgr::MessagesList::const_reference msg)
{
    assert(msg);
    auto msgData = msg->encodeData();
    QString msgDataStr;
    for (auto dataByte : msgData) {
        if (!msgDataStr.isEmpty()) {
            msgDataStr.append(' ');
        }
        msgDataStr.append(
            QString("%1").arg(
                static_cast<unsigned>(dataByte), 2, 16, QChar('0')));
    }

    return msgDataStr;
}

MessagePtr createMsgObjectFrom(
    const QVariant& msgMapVar,
    Protocol& protocol)
{
    if ((!msgMapVar.isValid()) || (!msgMapVar.canConvert<QVariantMap>())) {
        return MessagePtr();
    }

    auto msgMap = msgMapVar.value<QVariantMap>();
    auto msgId = IdProp().getFrom(msgMap);
    if (msgId.isEmpty()) {
        return MessagePtr();
    }

    auto dataStr = DataProp().getFrom(msgMap);
    QString stripedDataStr;
    stripedDataStr.reserve(dataStr.size());
    std::copy_if(
        dataStr.begin(), dataStr.end(), std::back_inserter(stripedDataStr),
        [](QChar ch) -> bool
        {
            return
                ch.isDigit() ||
                ((QChar('A') <= ch) && (ch <= QChar('F'))) ||
                ((QChar('a') <= ch) && (ch <= QChar('f')));
        });

    if ((stripedDataStr.size() & 0x1) != 0) {
        stripedDataStr.prepend(QChar('0'));
    }

    Message::DataSeq data;
    QString num;
    for (auto ch : stripedDataStr) {
        num.append(ch);
        if (num.size() == 1) {
            continue;
        }

        assert(num.size() == 2);
        bool ok = false;
        auto byte = num.toInt(&ok, 16);
        static_cast<void>(ok);
        assert(ok);
        assert((0 <= byte) && (byte <= 0xff));
        data.push_back(static_cast<Message::DataSeq::value_type>(byte));
        num.clear();
    }

    MessagePtr msg;
    unsigned idx = 0;
    while (!msg) {
        msg = protocol.createMessage(msgId, idx);
        if (!msg) {
            break;
        }

        ++idx;
        if (msg->decodeData(data)) {
            break;
        }

        msg.reset();
    }

    if (msg) {
        protocol.updateMessage(*msg);
    }

    return msg;
}

QVariantList convertRecvMsgList(
    const MsgFileMgr::MessagesList& allMsgs)
{
    QVariantList convertedList;
    for (auto& msg : allMsgs) {
        if (!msg) {
            assert(!"Message is expected to exist");
            continue;
        }

        QVariantMap msgInfoMap;
        IdProp().setTo(msg->idAsString(), msgInfoMap);
        DataProp().setTo(encodeMsgData(msg), msgInfoMap);
        TimestampProp().setTo(property::message::Timestamp().getFrom(*msg), msgInfoMap);
        TypeProp().setTo(static_cast<unsigned>(property::message::Type().getFrom(*msg)), msgInfoMap);

        // TODO: record custom properties

        convertedList.append(QVariant::fromValue(msgInfoMap));
    }
    return convertedList;
}

MsgFileMgr::MessagesList convertRecvMsgList(
    const QVariantList& msgs,
    Protocol& protocol)
{
    MsgFileMgr::MessagesList convertedList;

    for (auto& msgMapVar : msgs) {
        auto msg = createMsgObjectFrom(msgMapVar, protocol);
        if (!msg) {
            continue;
        }

        assert(msgMapVar.isValid() && msgMapVar.canConvert<QVariantMap>());

        auto msgMap = msgMapVar.value<QVariantMap>();
        auto timestamp = TimestampProp().getFrom(msgMap);
        if (timestamp == 0) {
            // Not a receive list, skip message
            continue;
        }

        auto type = static_cast<Message::Type>(TypeProp().getFrom(msgMap));

        property::message::Timestamp().setTo(timestamp, *msg);
        property::message::Type().setTo(type, *msg);
        // TODO: support custom properties

        convertedList.push_back(std::move(msg));
    }
    return convertedList;
}

QVariantList convertSendMsgList(
    const MsgFileMgr::MessagesList& allMsgs)
{
    QVariantList convertedList;
    for (auto& msg : allMsgs) {
        if (!msg) {
            assert(!"Message is expected to exist");
            continue;
        }

        QVariantMap msgInfoMap;
        IdProp().setTo(msg->idAsString(), msgInfoMap);
        DataProp().setTo(encodeMsgData(msg), msgInfoMap);
        DelayProp().setTo(property::message::Delay().getFrom(*msg), msgInfoMap);
        DelayUnitsProp().setTo(property::message::DelayUnits().getFrom(*msg), msgInfoMap);
        RepeatProp().setTo(property::message::RepeatDuration().getFrom(*msg), msgInfoMap);
        RepeatUnitsProp().setTo(property::message::RepeatDurationUnits().getFrom(*msg), msgInfoMap);
        RepeatCountProp().setTo(property::message::RepeatCount().getFrom(*msg), msgInfoMap);

        // TODO: record custom properties

        convertedList.append(QVariant::fromValue(msgInfoMap));
    }
    return convertedList;
}

MsgFileMgr::MessagesList convertSendMsgList(
    const QVariantList& msgs,
    Protocol& protocol)
{
    MsgFileMgr::MessagesList convertedList;
    unsigned long long prevTimestamp = 0;

    for (auto& msgMapVar : msgs) {
        auto msg = createMsgObjectFrom(msgMapVar, protocol);
        if (!msg) {
            continue;
        }

        assert(msgMapVar.isValid() && msgMapVar.canConvert<QVariantMap>());

        auto msgMap = msgMapVar.value<QVariantMap>();
        auto delay = DelayProp().getFrom(msgMap);
        auto delayUnits = DelayUnitsProp().getFrom(msgMap);
        auto repeatDuration = RepeatProp().getFrom(msgMap);
        auto repeatDurationUnits = RepeatUnitsProp().getFrom(msgMap);
        auto repeatCount = RepeatCountProp().getFrom(msgMap);

        if ((repeatDuration == 0) && (repeatCount == 0)) {
            repeatCount = 1;

            do {
                if (delay != 0) {
                    break;
                }

                // Probably receive list is loaded
                auto timestamp = TimestampProp().getFrom(msgMap);
                if (timestamp == 0) {
                    break;
                }

                if (prevTimestamp == 0) {
                    prevTimestamp = timestamp;
                    break;
                }

                auto delayTmp = timestamp - prevTimestamp;
                if (delayTmp <= 0) {
                    break;
                }

                delay = delayTmp;
            } while (false);
        }

        property::message::Delay().setTo(delay, *msg);
        property::message::DelayUnits().setTo(std::move(delayUnits), *msg);
        property::message::RepeatDuration().setTo(repeatDuration, *msg);
        property::message::RepeatDurationUnits().setTo(std::move(repeatDurationUnits), *msg);
        property::message::RepeatCount().setTo(repeatCount, *msg);
        // TODO: support custom properties

        convertedList.push_back(std::move(msg));
    }
    return convertedList;
}

QVariantList convertMsgList(
    MsgFileMgr::Type type,
    const MsgFileMgr::MessagesList& allMsgs)
{
    if (type == MsgFileMgr::Type::Recv) {
        return convertRecvMsgList(allMsgs);
    }

    return convertSendMsgList(allMsgs);
}

MsgFileMgr::MessagesList convertMsgList(
    MsgFileMgr::Type type,
    const QVariantList& msgs,
    Protocol& protocol)
{
    if (type == MsgFileMgr::Type::Recv) {
        return convertRecvMsgList(msgs, protocol);
    }

    return convertSendMsgList(msgs, protocol);
}

}  // namespace

MsgFileMgr::MsgFileMgr() = default;
MsgFileMgr::~MsgFileMgr() = default;
MsgFileMgr::MsgFileMgr(const MsgFileMgr&) = default;
MsgFileMgr::MsgFileMgr(MsgFileMgr&&) = default;
MsgFileMgr& MsgFileMgr::operator=(const MsgFileMgr&) = default;
MsgFileMgr& MsgFileMgr::operator=(MsgFileMgr&&) = default;

const QString& MsgFileMgr::getLastFile() const
{
    return m_lastFile;
}

MsgFileMgr::MessagesList MsgFileMgr::load(
    Type type,
    const QString& filename,
    Protocol& protocol)
{
    MessagesList allMsgs;
    do {
        QFile msgsFile(filename);
        if (!msgsFile.open(QIODevice::ReadOnly)) {
            std::cerr << "ERROR: Failed to load the file " <<
                filename.toStdString() << std::endl;
            break;
        }

        auto data = msgsFile.readAll();

        auto jsonError = QJsonParseError();
        auto jsonDoc = QJsonDocument::fromJson(data, &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            std::cerr << "ERROR: Invalid contents of messages file!" << std::endl;
            break;
        }

        if (!jsonDoc.isArray()) {
            std::cerr << "ERROR: Invalid contents of messages file!" << std::endl;
            break;
        }

        auto topArray = jsonDoc.array();
        auto varList = topArray.toVariantList();
        allMsgs = convertMsgList(type, varList, protocol);
        m_lastFile = filename;
    } while (false);

    return allMsgs;
}

bool MsgFileMgr::save(Type type, const QString& filename, const MessagesList& msgs)
{
    QString filenameTmp(filename);
    while (true) {
        filenameTmp.append(".tmp");
        if (!QFile::exists(filenameTmp)) {
            break;
        }
    }

    QFile msgsFile(filenameTmp);
    if (!msgsFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    auto convertedList = convertMsgList(type, msgs);

    auto jsonArray = QJsonArray::fromVariantList(convertedList);
    QJsonDocument jsonDoc(jsonArray);
    auto data = jsonDoc.toJson();

    msgsFile.write(data);

    if ((QFile::exists(filename)) &&
        (!QFile::remove(filename))) {
        msgsFile.close();
        QFile::remove(filenameTmp);
        return false;
    }

    if (!msgsFile.rename(filename)) {
        return false;
    }

    m_lastFile = filename;
    return true;
}

const QString& MsgFileMgr::getFilesFilter()
{
    static const QString Str(QObject::tr("All Files (*)"));
    return Str;
}


}  // namespace comms_champion


