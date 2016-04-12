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

#include "comms_champion/ConfigMgr.h"

#include <cassert>
#include <iostream>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QVariantMap>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

ConfigMgr::ConfigMgr() = default;
ConfigMgr::~ConfigMgr() = default;

const QString& ConfigMgr::getLastFile() const
{
    return m_lastConfigFile;
}

const QString& ConfigMgr::getFilesFilter()
{
    static const QString Str(QObject::tr("All Files (*)"));
    return Str;
}

QVariantMap ConfigMgr::loadConfig(const QString& filename)
{
    return loadConfig(filename, true);
}

QVariantMap ConfigMgr::loadConfig(const QString& filename, bool updateAsLast)
{
    QVariantMap loadedConfig;
    do {
        QFile configFile(filename);
        if (!configFile.open(QIODevice::ReadOnly)) {
            std::cerr << "ERROR: Failed to load the configuration file " <<
                filename.toStdString() << std::endl;
            break;
        }

        auto data = configFile.readAll();

        auto jsonError = QJsonParseError();
        auto jsonDoc = QJsonDocument::fromJson(data, &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            std::cerr << "ERROR: Invalid contents of configuration file!" << std::endl;
            break;
        }

        if (!jsonDoc.isObject()) {
            std::cerr << "ERROR: Invalid contents of configuration file!" << std::endl;
            break;
        }

        auto topObject = jsonDoc.object();
        loadedConfig = topObject.toVariantMap();
        if (updateAsLast) {
            m_lastConfigFile = filename;
        }
    } while (false);

    return loadedConfig;
}

bool ConfigMgr::saveConfig(const QString& filename, const QVariantMap& config)
{
    return saveConfig(filename, config, true);
}

bool ConfigMgr::saveConfig(
    const QString& filename,
    const QVariantMap& config,
    bool updateAsLast)
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
        return false;
    }

    auto jsonObj = QJsonObject::fromVariantMap(config);
    QJsonDocument jsonDoc(jsonObj);
    auto data = jsonDoc.toJson();

    configFile.write(data);

    if ((QFile::exists(filename)) &&
        (!QFile::remove(filename))) {
        configFile.close();
        QFile::remove(filenameTmp);
        return false;
    }

    if (!configFile.rename(filename)) {
        return false;
    }

    if (updateAsLast) {
        m_lastConfigFile = filename;
    }
    return true;
}

}  // namespace comms_champion


