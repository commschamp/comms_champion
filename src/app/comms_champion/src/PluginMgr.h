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
//#include <QtCore/QVariantMap>
//#include <QtCore/QVariantList>
#include <QtCore/QPluginLoader>

#include "comms_champion/PluginControlInterface.h"

//#include "ConfigMgr.h"

namespace comms_champion
{

class PluginMgr : public QObject
{
    Q_OBJECT
public:

    enum class PluginsState
    {
        Inactive,
        Active,
    };

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

        QString m_filename;
        QString m_name;
        QString m_desc;
    };

    typedef std::shared_ptr<PluginInfo> PluginInfoPtr;
    typedef std::list<PluginInfoPtr> ListOfPluginInfos;

    virtual ~PluginMgr() = default;

    static PluginMgr* instance();
    static PluginMgr& instanceRef();
    void setPluginsDir(const QString& pluginDir);
    const ListOfPluginInfos& getAvailablePlugins();
    PluginsState getState() const;

signals:
    void sigStateChanged(int value);


//private slots:
//    void configUpdated();

private:
    typedef std::shared_ptr<QPluginLoader> PluginLoaderPtr;
    typedef std::list<PluginLoaderPtr> PluginLoadersList;

    PluginMgr();

    static PluginInfoPtr readPluginInfo(const QString& filename);

//    bool verifyPluginsConfiguration(const QVariantList plugins);
//    static PluginLoaderPtr allocPluginLoader(const QString& name);

    QString m_pluginDir;
    ListOfPluginInfos m_availablePlugins;
    PluginsState m_state = PluginsState::Inactive;
//    QVariantMap m_curConfig;
//    PluginLoadersList m_plugins;
//    PluginControlInterface m_controlInterface;
};

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::PluginMgr::PluginInfoPtr);

