//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
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

#include "dir.h"

#include <cassert>

CC_DISABLE_WARNINGS()
#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
CC_ENABLE_WARNINGS()

namespace comms_champion {

QString getRootDir()
{
    QDir appDir(qApp->applicationDirPath());
    QDir binDir(CC_BINDIR);
    while (true) {
        auto appDirName = appDir.dirName();
        if (appDirName.isEmpty()) {
            break;
        }

        auto binDirName = binDir.dirName();
        if (binDirName.isEmpty()) {
            break;
        }

        if (appDirName != binDirName) {
            break;
        }

        appDir.cdUp();
        binDir.cdUp();
    }

    return appDir.path();
}

QString getPluginsDir()
{
    QDir dir(getRootDir());
    if (!dir.cd(CC_PLUGINDIR)) {
        return QString();
    }

    return dir.path();
}

QString getConfigDir()
{
    QDir dir(getRootDir());
    if (!dir.cd(CC_CONFIGDIR)) {
        return QString();
    }

    return dir.path();
}

QString getAppDataDir()
{
    return 
        QDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation))
            .absoluteFilePath("CommsChampion");
}

QString getGlobalDataDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
}

} // namespace comms_champion
