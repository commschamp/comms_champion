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

#include "ConfigMgr.h"

#include <cassert>

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QVariantMap>

namespace comms_champion
{

ConfigMgr& ConfigMgr::instance()
{
    static ConfigMgr mgr;
    return mgr;
}

const QString& ConfigMgr::getCurrentFile() const
{
    return m_configFile;
}

const QString& ConfigMgr::getFilesFilter()
{
    static const QString Str(QObject::tr("Configuration files (*.cfg)"));
    return Str;
}

ConfigMgr::ErrorStatus ConfigMgr::loadConfig(const QString& filename)
{
    QFile configFile(filename);
    if (!configFile.open(QIODevice::ReadOnly)) {
        return ErrorStatus::BadFilename;
    }

    auto data = configFile.readAll();

    auto jsonError = QJsonParseError();
    auto jsonDoc = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        return ErrorStatus::CorruptedFile;
    }

    if (!jsonDoc.isObject()) {
        return ErrorStatus::CorruptedFile;
    }

    auto topObject = jsonDoc.object();
    auto keys = topObject.keys();
    QVariantMap configMap;
    for (auto k : keys) {
        auto jsonValue = topObject.value(k);
        if (!jsonValue.isObject()) {
            return ErrorStatus::CorruptedFile;
        }

        auto valueMap = jsonValue.toObject().toVariantMap();
        configMap.insert(k, QVariant::fromValue(std::move(valueMap)));
    }

    m_options.swap(configMap);
    emit sigConfigUpdated();
    return ErrorStatus::Success;
}

ConfigMgr::ErrorStatus ConfigMgr::saveConfig(const QString& filename)
{
    QString filenameTmp(filename);
    while (true) {
        filenameTmp.append(".tmp");
        if (!QFile::exists(filenameTmp)) {
            break;
        }
    }

    QFile configFile(filenameTmp);
    if (!configFile.open(QIODevice::WriteOnly)) {
        return ErrorStatus::BadFilename;
    }

    auto jsonObj = QJsonObject::fromVariantMap(m_options);
    QJsonDocument jsonDoc(jsonObj);
    auto data = jsonDoc.toJson();

    configFile.write(data);

    if (!QFile::remove(filename)) {
        return ErrorStatus::BadFilename;
    }

    configFile.rename(filename);
    return ErrorStatus::Success;
}

}  // namespace comms_champion


