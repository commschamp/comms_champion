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

ConfigMgr* ConfigMgr::instance()
{
    static ConfigMgr mgr;
    return &mgr;
}

ConfigMgr& ConfigMgr::instanceRef()
{
    return *(instance());
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

ConfigMgr::ListOfErrors ConfigMgr::loadConfig(const QString& filename)
{
    ListOfErrors errors;
    do {
        QFile configFile(filename);
        if (!configFile.open(QIODevice::ReadOnly)) {
            errors.push_back(
                ListOfErrors::value_type(
                    tr("Bad filename"),
                    tr("Failed to load the configuration file.")));
            break;
        }

        auto data = configFile.readAll();

        auto badConfigErrorFunc =
            [&errors](const QString& msg)
            {
                errors.push_back(
                    std::make_pair(
                        tr("Bad configuration file"),
                        msg));
            };

        auto badContentsErrorFunc =
            [&badConfigErrorFunc]()
            {
                badConfigErrorFunc(
                    tr("Failed to load the configuration file. Bad contents."));
            };


        auto jsonError = QJsonParseError();
        auto jsonDoc = QJsonDocument::fromJson(data, &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            badContentsErrorFunc();
            break;
        }

        if (!jsonDoc.isObject()) {
            badContentsErrorFunc();
            break;
        }

        auto topObject = jsonDoc.object();
        auto keys = topObject.keys();
        QVariantMap configMap;
        for (auto k : keys) {
            auto jsonValue = topObject.value(k);
            if (!jsonValue.isObject()) {
                badContentsErrorFunc();
            }

            auto valueMap = jsonValue.toObject().toVariantMap();
            configMap.insert(k, QVariant::fromValue(std::move(valueMap)));
        }

        m_options.swap(configMap);
        m_reportedErrors.clear();
        emit sigConfigUpdated();
        if (!m_reportedErrors.empty()) {
            m_options.swap(configMap);
            for (auto e : m_reportedErrors) {
                badConfigErrorFunc(e);
            }
            break;
        }
    } while (false);
    return errors;
}

ConfigMgr::ListOfErrors ConfigMgr::saveConfig(const QString& filename)
{
    ListOfErrors errors;
    do {
        QString filenameTmp(filename);
        while (true) {
            filenameTmp.append(".tmp");
            if (!QFile::exists(filenameTmp)) {
                break;
            }
        }

        auto reportFilesystemErrorFunc =
            [&errors]()
            {
                errors.push_back(
                    std::make_pair(
                        tr("Filesystem error"),
                        tr("Failed to save the configuration file.")));
            };

        QFile configFile(filenameTmp);
        if (!configFile.open(QIODevice::WriteOnly)) {
            reportFilesystemErrorFunc();
            break;
        }

        auto jsonObj = QJsonObject::fromVariantMap(m_options);
        QJsonDocument jsonDoc(jsonObj);
        auto data = jsonDoc.toJson();

        configFile.write(data);

        if (!QFile::remove(filename)) {
            reportFilesystemErrorFunc();
            break;
        }

        configFile.rename(filename);
    } while (false);
    return errors;
}

void ConfigMgr::reportConfigError(const QString& errorMsg)
{
    m_reportedErrors.push_back(errorMsg);
}

QVariantMap ConfigMgr::getConfiguration(const QString& topKey)
{
    auto iter = m_options.find(topKey);
    if (iter == m_options.end()) {
        return QVariantMap();
    }

    auto mapVar = *iter;
    if ((!mapVar.isValid()) || (!mapVar.canConvert<QVariantMap>())) {
        assert(!"Incorrect internal map, should not happen");
        return QVariantMap();
    }

    return mapVar.value<QVariantMap>();
}

}  // namespace comms_champion


