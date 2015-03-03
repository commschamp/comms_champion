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

#include <memory>
#include <list>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariantMap>
#include <QtCore/QPluginLoader>

#include "comms_champion/PluginControlInterface.h"
#include "comms_champion/Plugin.h"

namespace comms_champion
{

class PluginMgr : public QObject
{
    Q_OBJECT
public:

    enum class PluginsState
    {
        Clear,
        Inactive,
        Active,
    };

    typedef std::shared_ptr<QPluginLoader> PluginLoaderPtr;

    class PluginInfo
    {
        friend class PluginMgr;

    public:
        const QString& getName() const
        {
            return m_name;
        }

        const QString& getDescription() const
        {
            return m_desc;
        }

    private:
        PluginInfo() = default;

        PluginLoaderPtr m_loader;
        QString m_iid;
        QString m_name;
        QString m_desc;
        bool m_applied = false;
    };

    typedef std::shared_ptr<PluginInfo> PluginInfoPtr;
    typedef std::list<PluginInfoPtr> ListOfPluginInfos;
    typedef Plugin::WidgetPtr WidgetPtr;

    ~PluginMgr();

    static PluginMgr* instance();
    static PluginMgr& instanceRef();
    void setPluginsDir(const QString& pluginDir);
    const ListOfPluginInfos& getAvailablePlugins();
    const ListOfPluginInfos& getAppliedPlugins() const;
    PluginsState getState() const;
    ListOfPluginInfos loadPluginsFromConfig(const QVariantMap& config) const;
    bool loadPlugin(const PluginInfo& info);
    bool needsReload(const ListOfPluginInfos& infos) const;
    bool apply(const ListOfPluginInfos& infos);
    static QVariantMap getConfigForPlugins(const ListOfPluginInfos& infos);
    static WidgetPtr getPluginConfigWidget(const PluginInfo& info);

signals:
    void sigStateChanged(int value);

private:
    typedef std::list<PluginLoaderPtr> PluginLoadersList;

    PluginMgr();

    static PluginInfoPtr readPluginInfo(const QString& filename);

    QString m_pluginDir;
    ListOfPluginInfos m_plugins;
    ListOfPluginInfos m_appliedPlugins;
    PluginsState m_state = PluginsState::Clear;
    PluginControlInterface m_controlInterface;
};

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::PluginMgr::PluginInfoPtr);

