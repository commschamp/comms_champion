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


#pragma once

#include <utility>
#include <list>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

namespace comms_champion
{

class ConfigMgr : public QObject
{
    Q_OBJECT
public:

    static ConfigMgr* instance();
    static ConfigMgr& instanceRef();

    const QString& getCurrentFile() const;
    static const QString& getFilesFilter();

    typedef std::pair<QString, QString> ErrorInfo;
    typedef std::list<ErrorInfo> ListOfErrors;
    ListOfErrors loadConfig(const QString& filename);
    ListOfErrors saveConfig(const QString& filename);
    void reportConfigError(const QString& errorMsg);

    QVariantMap getConfiguration(const QString& topKey);

signals:
    void sigConfigUpdated();

private:
    ConfigMgr() = default;

    QString m_configFile;
    QVariantMap m_options;
    std::list<QString> m_reportedErrors;
};

}  // namespace comms_champion


