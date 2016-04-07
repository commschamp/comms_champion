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

#include "PluginMgr.h"

#include "PluginMgrImpl.h"

template class comms_champion::StaticSingleton<comms_champion::PluginMgr>;

namespace comms_champion
{

PluginMgr::PluginMgr()
  : m_impl(new PluginMgrImpl())
{
}

PluginMgr::~PluginMgr() = default;

void PluginMgr::setPluginsDir(const QString& pluginDir)
{
    m_impl->setPluginsDir(pluginDir);
}

const PluginMgr::ListOfPluginInfos& PluginMgr::getAvailablePlugins()
{
    return m_impl->getAvailablePlugins();
}

const PluginMgr::ListOfPluginInfos& PluginMgr::getAppliedPlugins() const
{
    return m_impl->getAppliedPlugins();
}

PluginMgr::ListOfPluginInfos PluginMgr::loadPluginsFromConfig(
    const QVariantMap& config)
{
    return m_impl->loadPluginsFromConfig(config);
}

PluginMgr::ListOfPluginInfos PluginMgr::loadPluginsFromConfigFile(
    const QString& filename)
{
    return m_impl->loadPluginsFromConfigFile(filename);
}

bool PluginMgr::savePluginsToConfigFile(
    const ListOfPluginInfos& infos,
    const QString& filename)
{
    return m_impl->savePluginsToConfigFile(infos, filename);
}

bool PluginMgr::loadPlugin(const PluginInfo& info)
{
    return m_impl->loadPlugin(info);
}

bool PluginMgr::hasAppliedPlugins() const
{
    return m_impl->hasAppliedPlugins();
}

bool PluginMgr::needsReload(const ListOfPluginInfos& infos) const
{
    return m_impl->needsReload(infos);
}

void PluginMgr::unloadApplied()
{
    m_impl->unloadApplied();
}

bool PluginMgr::apply(const ListOfPluginInfos& infos)
{
    return m_impl->apply(infos);
}

QVariantMap PluginMgr::getConfigForPlugins(
    const ListOfPluginInfos& infos)
{
    return PluginMgrImpl::getConfigForPlugins(infos);
}

PluginMgr::WidgetPtr PluginMgr::getPluginConfigWidget(const PluginInfo& info)
{
    return PluginMgrImpl::getPluginConfigWidget(info);
}

const QString& PluginMgr::getLastFile() const
{
    return m_impl->getLastFile();
}

const QString& PluginMgr::getFilesFilter()
{
    return PluginMgrImpl::getFilesFilter();
}

}  // namespace comms_champion


