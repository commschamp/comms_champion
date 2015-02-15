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
#include <QtCore/QVariantMap>
#include <QtCore/QVariantList>
#include <QtCore/QPluginLoader>

#include "comms_champion/PluginControlInterface.h"

#include "ConfigMgr.h"

namespace comms_champion
{

class PluginMgr // : public QObject
{
//    Q_OBJECT
public:
    static PluginMgr& instanceRef();

//private slots:
//    void configUpdated();

private:
    typedef std::shared_ptr<QPluginLoader> PluginLoaderPtr;
    typedef std::list<PluginLoaderPtr> PluginLoadersList;

    PluginMgr();

//    bool verifyPluginsConfiguration(const QVariantList plugins);
//    static PluginLoaderPtr allocPluginLoader(const QString& name);

    QVariantMap m_curConfig;
    PluginLoadersList m_plugins;
    PluginControlInterface m_controlInterface;
};

}  // namespace comms_champion


