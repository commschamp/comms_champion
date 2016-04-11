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


#pragma once

#include <memory>
#include <list>
#include <array>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariantMap>
#include <QtCore/QPluginLoader>
CC_ENABLE_WARNINGS()

#include "comms_champion/Plugin.h"
#include "PluginMgr.h"
#include "ConfigMgr.h"


namespace comms_champion
{

class PluginMgrImpl
{
public:

    typedef PluginMgr::PluginLoaderPtr PluginLoaderPtr;
    typedef PluginMgr::PluginInfo PluginInfo;
    typedef PluginMgr::PluginInfoPtr PluginInfoPtr;
    typedef PluginMgr::ListOfPluginInfos ListOfPluginInfos;
    typedef Plugin::WidgetPtr WidgetPtr;

    PluginMgrImpl();
    ~PluginMgrImpl();

    void setPluginsDir(const QString& pluginDir);
    const ListOfPluginInfos& getAvailablePlugins();
    const ListOfPluginInfos& getAppliedPlugins() const;
    void setAppliedPlugins(const ListOfPluginInfos& plugins);
    ListOfPluginInfos loadPluginsFromConfig(const QVariantMap& config);
    ListOfPluginInfos loadPluginsFromConfigFile(const QString& filename);
    bool savePluginsToConfigFile(const ListOfPluginInfos& infos, const QString& filename);
    Plugin* loadPlugin(const PluginInfo& info);
    bool hasAppliedPlugins() const;
    bool needsReload(const ListOfPluginInfos& infos) const;
    void unloadApplied();
    static QVariantMap getConfigForPlugins(const ListOfPluginInfos& infos);
    const QString& getLastFile() const;
    static const QString& getFilesFilter();

private:
    typedef std::list<PluginLoaderPtr> PluginLoadersList;

    PluginInfoPtr readPluginInfo(const QString& filename);

    QString m_pluginDir;
    ListOfPluginInfos m_plugins;
    ListOfPluginInfos m_appliedPlugins;
    ConfigMgr m_configMgr;
};

}  // namespace comms_champion


