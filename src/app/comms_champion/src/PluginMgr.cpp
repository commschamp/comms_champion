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

#include "PluginMgr.h"

#include <cassert>

#include <QtCore/QString>
#include <QtCore/QVariantList>

#include "comms_champion/Plugin.h"

#include "MsgMgr.h"

#include <iostream>

namespace comms_champion
{

namespace
{

const QString ConfigTopKey("cc_plugin_mgr");
const QString PluginListKey("plugins");
const QString InvalidConfigurationStr(
        "Invalid plugins configuration.");

struct PluginLoaderDeleter
{
    void operator()(QPluginLoader* loader)
    {
        assert(loader != nullptr);
        if (loader->isLoaded()) {
            loader->unload();
        }
        delete loader;
    }
};

Plugin* getPlugin(QPluginLoader& loader)
{
    return qobject_cast<Plugin*>(loader.instance());
}

bool verifyPluginConfig(const Plugin& plugin)
{
    static_cast<void>(plugin);
    // TODO: verify
    return true;
}

void configurePlugin(const Plugin& plugin)
{
    static_cast<void>(plugin);
    // TODO: reconfigure
}


}  // namespace

PluginMgr& PluginMgr::instanceRef()
{
    static PluginMgr mgr;
    return mgr;
}

void PluginMgr::configUpdated()
{
    auto& configMgr = ConfigMgr::instanceRef();
    auto newConfig = configMgr.getConfiguration(ConfigTopKey);
    if (newConfig.isEmpty()) {
        configMgr.reportConfigError("Empty list of plugins.");
        return;
    }

    auto iter = newConfig.find(PluginListKey);
    if (iter == newConfig.end()) {
        configMgr.reportConfigError(InvalidConfigurationStr);
        return;
    }

    bool reloadPlugins = (newConfig != m_curConfig);

    auto listVar = *iter;
    if ((!listVar.isValid()) || (!listVar.canConvert<QVariantList>())) {
        configMgr.reportConfigError(InvalidConfigurationStr);
        return;
    }

    auto pluginsList = listVar.value<QVariantList>();
    if (!verifyPluginsConfiguration(pluginsList)) {
        return;
    }

    // Config is OK
    if (reloadPlugins) {
        if (!m_plugins.empty()) {
            MsgMgr::instanceRef().stop();
        }

        m_plugins.clear(); // unload all of existing ones
        MsgMgr::instanceRef().clear(); // Just in case plugins haven't cleared sockets and protocol.

        for (auto nameVar : pluginsList) {
            assert(nameVar.isValid());
            assert(nameVar.canConvert<QString>());
            auto name = nameVar.value<QString>();

            auto loader = allocPluginLoader(name);
            assert(loader);
            auto plugin = getPlugin(*loader);
            assert(plugin != nullptr);
            assert(!plugin->isApplied());
            plugin->apply(m_controlInterface);
            m_plugins.push_back(std::move(loader));
        }
    }

    for (auto& loader : m_plugins) {
        assert(loader);
        auto plugin = getPlugin(*loader);
        assert(plugin != nullptr);
        assert(plugin->isApplied());
        configurePlugin(*plugin);
    }

    assert(!m_plugins.empty());
    if (reloadPlugins) {
        MsgMgr::instanceRef().start();
    }
}

PluginMgr::PluginMgr()
{
    connect(
        ConfigMgr::instance(), SIGNAL(sigConfigUpdated()),
        this, SLOT(configUpdated()));
}

bool PluginMgr::verifyPluginsConfiguration(const QVariantList plugins)
{
    auto& configMgr = ConfigMgr::instanceRef();

    for (auto& nameVar : plugins) {
        if ((!nameVar.isValid()) || (!nameVar.canConvert<QString>())) {
            configMgr.reportConfigError(InvalidConfigurationStr);
            return false;
        }

        auto name = nameVar.value<QString>();
        auto iter = std::find_if(
            m_plugins.begin(), m_plugins.end(),
            [&name](const PluginLoaderPtr& loaderPtr)->bool
            {
                assert(loaderPtr);
                return (name == loaderPtr->fileName());
            });

        if (iter != m_plugins.end()) {
            auto& loaderPtr = *iter;
            assert(loaderPtr);
            auto* foundPlugin = getPlugin(*loaderPtr);
            if (!verifyPluginConfig(*foundPlugin)) {
                configMgr.reportConfigError(InvalidConfigurationStr);
                return false;
            }

            continue;
        }

        auto loaderPtr = allocPluginLoader(name);
        if (!loaderPtr) {
            return false;
        }

        auto* loadedPlugin = getPlugin(*loaderPtr);
        assert(loadedPlugin != nullptr);
        if (!verifyPluginConfig(*loadedPlugin)) {
            configMgr.reportConfigError(InvalidConfigurationStr);
            return false;
        }
    }
    return true;
}

PluginMgr::PluginLoaderPtr PluginMgr::allocPluginLoader(const QString& name)
{
    std::cout << __FUNCTION__ << ": " << name.toStdString() << std::endl;
    auto& configMgr = ConfigMgr::instanceRef();
    PluginLoaderPtr loaderPtr(new QPluginLoader(name), PluginLoaderDeleter());
    auto* loadedPlugin = getPlugin(*loaderPtr);
    if (loadedPlugin == nullptr) {
        if (loaderPtr->isLoaded()) {
            configMgr.reportConfigError("Wrong plugin type.");
        }
        else {
            configMgr.reportConfigError(
                QString("Plugin load error. ") + loaderPtr->errorString());
        }
        return PluginLoaderPtr();
    }

    std::cout << __FUNCTION__ << ": plugin " << name.toStdString() << " loaded and initialised" << std::endl;
    return loaderPtr;
}

}  // namespace comms_champion


