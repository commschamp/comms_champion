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

#include "MsgFileMgr.h"

#include <cassert>
#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QVariantMap>

#include "GlobalConstants.h"

namespace comms_champion
{

namespace
{

const QString& getIdKeyStr()
{
    static const QString Str("id");
    return Str;
}

const QString& getDataKeyStr()
{
    static const QString Str("data");
    return Str;
}

const QString& getDelayKeyStr()
{
    static const QString Str("delay_ms");
    return Str;
}

const QString& getDelayUnitsKeyStr()
{
    static const QString Str("orig_delay_units");
    return Str;
}

const QString& getRepeatDurationKeyStr()
{
    static const QString Str("repeat_duration_ms");
    return Str;
}

const QString& getRepeatUnitsKeyStr()
{
    static const QString Str("orig_repeat_units");
    return Str;
}

const QString& getRepeatCountKeyStr()
{
    static const QString Str("orig_repeat_count");
    return Str;
}

}  // namespace

MsgFileMgr* MsgFileMgr::instance()
{
    static MsgFileMgr mgr;
    return &mgr;
}

MsgFileMgr& MsgFileMgr::instanceRef()
{
    return *(instance());
}

const QString& MsgFileMgr::getLastFile() const
{
    return m_lastFile;
}

bool MsgFileMgr::save(Type type, const QString& filename, const MsgInfosList& msgs)
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

QVariantList MsgFileMgr::convertMsgList(Type type, const MsgInfosList& msgs)
{
    static_cast<void>(type);
    QVariantList convertedList;
    for (auto& msgInfo : msgs) {
        assert(msgInfo);
        auto appMsg = msgInfo->getAppMessage();
        if (!appMsg) {
            continue;
        }

        auto msgData = appMsg->serialiseData();
        QString msgDataStr;
        for (auto dataByte : msgData) {
            if (!msgDataStr.isEmpty()) {
                msgDataStr.append(' ');
            }
            msgDataStr.append(
                QString("%1").arg(
                    static_cast<unsigned>(dataByte), 2, 16, QChar('0')));
        }

        auto delayVar = msgInfo->getExtraProperty(GlobalConstants::msgDelayPropertyName());
        unsigned long long delay = 0;
        if (delayVar.isValid() && delayVar.canConvert<decltype(delay)>()) {
            delay = delayVar.value<decltype(delay)>();
        }

        auto delayUnitsVar =
            msgInfo->getExtraProperty(GlobalConstants::msgDelayUnitsPropertyName());
        int delayUnits = 0;
        if (delayUnitsVar.isValid() && delayUnitsVar.canConvert<decltype(delayUnits)>()) {
            delayUnits = delayUnitsVar.value<decltype(delayUnits)>();
        }

        auto repeatDurationVar =
            msgInfo->getExtraProperty(GlobalConstants::msgRepeatDurationPropertyName());
        unsigned long long repeatDuration = 0;
        if (repeatDurationVar.isValid() && repeatDurationVar.canConvert<decltype(repeatDuration)>()) {
            repeatDuration = repeatDurationVar.value<decltype(repeatDuration)>();
        }

        auto repeatUnitsVar =
            msgInfo->getExtraProperty(GlobalConstants::msgRepeatUnitsPropertyName());
        int repeatUnits = 0;
        if (repeatUnitsVar.isValid() && repeatUnitsVar.canConvert<decltype(repeatUnits)>()) {
            repeatUnits = repeatUnitsVar.value<decltype(repeatUnits)>();
        }

        auto repeatCountVar =
            msgInfo->getExtraProperty(GlobalConstants::msgRepeatCountPropertyName());
        int repeatCount = 0;
        if (repeatCountVar.isValid() && repeatCountVar.canConvert<decltype(repeatCount)>()) {
            repeatCount = repeatCountVar.value<decltype(repeatCount)>();
        }

        QVariantMap msgInfoMap;
        msgInfoMap.insert(
            getIdKeyStr(), QVariant::fromValue(appMsg->idAsString()));
        msgInfoMap.insert(
            getDataKeyStr(), QVariant::fromValue(msgDataStr));
        msgInfoMap.insert(
            getDelayKeyStr(), QVariant::fromValue(delay));
        msgInfoMap.insert(
            getDelayUnitsKeyStr(), QVariant::fromValue(delayUnits));
        msgInfoMap.insert(
            getRepeatDurationKeyStr(), QVariant::fromValue(repeatDuration));
        msgInfoMap.insert(
            getRepeatUnitsKeyStr(), QVariant::fromValue(repeatUnits));
        msgInfoMap.insert(
            getRepeatCountKeyStr(), QVariant::fromValue(repeatCount));

        convertedList.append(QVariant::fromValue(msgInfoMap));
    }
    return convertedList;
}

}  // namespace comms_champion


