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
#include <QtCore/QString>
#include <QtCore/QVariantMap>
#include <QtCore/QPluginLoader>
CC_ENABLE_WARNINGS()

#include "Api.h"
#include "Plugin.h"

namespace comms_champion
{

class PluginMgrImpl;
class CC_API PluginMgr
{
public:

    typedef std::shared_ptr<QPluginLoader> PluginLoaderPtr;

    class PluginInfo
    {
        friend class PluginMgr;
        friend class PluginMgrImpl;

    public:
        enum class Type
        {
            Invalid,
            Socket,
            Filter,
            Protocol,
            NumOfValues
        };

        const QString& getName() const
        {
            return m_name;
        }

        const QString& getDescription() const
        {
            return m_desc;
        }

        Type getType() const
        {
            return m_type;
        }

    private:
        PluginInfo() = default;

        PluginLoaderPtr m_loader;
        QString m_iid;
        QString m_name;
        QString m_desc;
        Type m_type = Type::Invalid;
        bool m_applied = false;
    };

    typedef std::shared_ptr<PluginInfo> PluginInfoPtr;
    typedef std::list<PluginInfoPtr> ListOfPluginInfos;
    typedef Plugin::WidgetPtr WidgetPtr;

    PluginMgr();
    ~PluginMgr();

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
    bool isProtocolChanging(const ListOfPluginInfos& infos) const;
    void unloadApplied();
    static QVariantMap getConfigForPlugins(const ListOfPluginInfos& infos);
    const QString& getLastFile() const;
    static const QString& getFilesFilter();

private:
    std::unique_ptr<PluginMgrImpl> m_impl;
};

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::PluginMgr::PluginInfoPtr);

