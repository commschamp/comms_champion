//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <utility>
#include <list>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
#include <QtCore/QVariantMap>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

class ConfigMgr
{
public:

    static ConfigMgr* instance();
    static ConfigMgr& instanceRef();

    const QString& getLastFile() const;
    static const QString& getFilesFilter();

    QVariantMap loadConfig(const QString& filename);
    QVariantMap loadConfig(const QString& filename, bool updateAsLast);
    bool saveConfig(const QString& filename, const QVariantMap& config);
    bool saveConfig(const QString& filename, const QVariantMap& config, bool updateAsLast);

private:
    ConfigMgr() = default;

    QString m_lastConfigFile;
};

}  // namespace comms_champion


