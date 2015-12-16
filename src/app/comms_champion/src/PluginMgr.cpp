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
#include <algorithm>
#include <type_traits>
#include <iostream>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QVariantList>
CC_ENABLE_WARNINGS()

#include "comms_champion/Plugin.h"

namespace comms_champion
{

namespace
{

const QString PluginsKey("cc_plugins_list");
const QString IidMetaKey("IID");
const QString MetaDataMetaKey("MetaData");
const QString NameMetaKey("name");
const QString DescMetaKey("desc");
const QString TypeMetaKey("type");

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

PluginMgr::PluginInfo::Type parseType(const QString& val)
{
    static const QString Values[] = {
        QString(),
        "socket",
        "filter",
        "protocol"
    };

    static_assert(std::extent<decltype(Values)>::value == (std::size_t)PluginMgr::PluginInfo::Type::NumOfValues,
        "The Values array must be adjusted.");

    auto iter = std::find(std::begin(Values), std::end(Values), val);
    if (iter == std::end(Values)) {
        return PluginMgr::PluginInfo::Type::Invalid;
    }

    return static_cast<PluginMgr::PluginInfo::Type>(std::distance(std::begin(Values), iter));
}

}  // namespace

PluginMgr::~PluginMgr()
{
    for (auto& pluginInfoPtr : m_plugins) {
        assert(pluginInfoPtr);
        assert(pluginInfoPtr->m_loader);
        if (pluginInfoPtr->m_loader->isLoaded()) {
            pluginInfoPtr->m_loader->unload();
        }
    }
}

PluginMgr* PluginMgr::instance()
{
    return &(instanceRef());
}

PluginMgr& PluginMgr::instanceRef()
{
    static PluginMgr mgr;
    return mgr;
}

void PluginMgr::setPluginsDir(const QString& pluginDir)
{
    m_pluginDir = pluginDir;
}

const PluginMgr::ListOfPluginInfos& PluginMgr::getAvailablePlugins()
{
    if (!m_plugins.empty()) {
        return m_plugins;
    }

    do {
        QDir pluginDir(m_pluginDir);
        auto files =
            pluginDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

        for (auto& f : files) {
            auto infoPtr = readPluginInfo(f);
            if (!infoPtr) {
                continue;
            }

            if (infoPtr->getType() == PluginInfo::Type::Invalid) {
                std::cerr << "WARNING: plugin " << f.toStdString() << " doesn't specify its type, use either "
                    "\"socket\", or \"filter\", or \"protocol\"."<< std::endl;
                continue;
            }

            m_plugins.push_back(std::move(infoPtr));
        }
    } while (false);

    return m_plugins;
}

const PluginMgr::ListOfPluginInfos& PluginMgr::getAppliedPlugins() const
{
    return m_appliedPlugins;
}

PluginMgr::PluginsState PluginMgr::getState() const
{
    return m_state;
}

PluginMgr::ListOfPluginInfos PluginMgr::loadPluginsFromConfig(
    const QVariantMap& config) const
{
    ListOfPluginInfos pluginInfos;
    do {
        auto listVar = config.value(PluginsKey);
        if ((!listVar.isValid()) || (!listVar.canConvert<QVariantList>())) {
            break;
        }

        auto varList = listVar.value<QVariantList>();
        for (auto& iidVar : varList) {
            if ((!iidVar.isValid()) || (!iidVar.canConvert<QString>())) {
                continue;
            }

            auto iid = iidVar.toString();
            auto iter =
                std::find_if(
                    m_plugins.begin(), m_plugins.end(),
                    [&iid](const PluginInfoPtr& i) -> bool
                    {
                        return i->m_iid == iid;
                    });

            if (iter == m_plugins.end()) {
                continue;
            }

            auto pluginInfoPtr = *iter;
            assert(pluginInfoPtr);
            assert(pluginInfoPtr->m_loader);
            auto* pluginPtr = getPlugin(*pluginInfoPtr->m_loader);
            assert(pluginPtr != nullptr);
            pluginPtr->reconfigure(config);

            pluginInfos.push_back(*iter);
        }

    } while (false);

    return pluginInfos;
}

bool PluginMgr::loadPlugin(const PluginInfo& info)
{
    assert(info.m_loader);
    if (info.m_loader->isLoaded()) {
        return true;
    }

    auto* plugin = getPlugin(*info.m_loader);
    return plugin != nullptr;
}

bool PluginMgr::needsReload(const ListOfPluginInfos& infos) const
{
    assert(!infos.empty());
    return (!m_appliedPlugins.empty()) &&
           (m_appliedPlugins != infos);
}

bool PluginMgr::apply(const ListOfPluginInfos& infos)
{
    if (!m_appliedPlugins.empty()) {
        emit sigStateChanged(static_cast<int>(PluginsState::Inactive));
    }

    bool reapply = needsReload(infos);
    if (reapply) {
        for (auto& pluginInfo : m_appliedPlugins) {
            assert(pluginInfo);
            assert(pluginInfo->m_loader);
            assert (pluginInfo->m_loader->isLoaded());
            pluginInfo->m_loader->unload();
        }
        emit sigStateChanged(static_cast<int>(PluginsState::Clear));
    }

    for (auto& reqInfo : infos) {
        assert(reqInfo);
        assert(reqInfo->m_loader);
        auto* pluginPtr = getPlugin(*reqInfo->m_loader);

        auto* ctrlInterface = getPluginControl(reqInfo->getType());
        if (ctrlInterface == nullptr) {
            continue;
        }

        if (m_appliedPlugins.empty() || reapply) {
            pluginPtr->apply(PluginControlInterface(*ctrlInterface));
        }
    }

    m_appliedPlugins = infos;

    emit sigStateChanged(static_cast<int>(PluginsState::Active));
    return true;
}

QVariantMap PluginMgr::getConfigForPlugins(
    const ListOfPluginInfos& infos)
{
    QVariantMap config;
    QVariantList pluginsList;
    for (auto& pluginInfoPtr : infos) {
        assert(pluginInfoPtr);
        assert(!pluginInfoPtr->m_iid.isEmpty());
        pluginsList.append(QVariant::fromValue(pluginInfoPtr->m_iid));

        assert(pluginInfoPtr->m_loader);
        auto* pluginPtr = getPlugin(*pluginInfoPtr->m_loader);
        assert(pluginPtr != nullptr);
        pluginPtr->getCurrentConfig(config);
    }

    config.insert(PluginsKey, QVariant::fromValue(pluginsList));
    return config;
}

PluginMgr::WidgetPtr PluginMgr::getPluginConfigWidget(const PluginInfo& info)
{
    assert(info.m_loader);
    auto* pluginPtr = getPlugin(*info.m_loader);
    assert(pluginPtr != nullptr);
    return pluginPtr->getConfigWidget();
}

PluginMgr::PluginControls::PluginControls()
{
    std::fill(m_ctrlInterfaces.begin(), m_ctrlInterfaces.end(), nullptr);
    m_ctrlInterfaces[(unsigned)PluginInfo::Type::Socket] = &m_socketCtrlInterface;
    m_ctrlInterfaces[(unsigned)PluginInfo::Type::Protocol] = &m_protocolCtrlInterface;
}

PluginMgr::PluginMgr() = default;

PluginMgr::PluginInfoPtr PluginMgr::readPluginInfo(const QString& filename)
{
    PluginInfoPtr ptr;

    do {
        PluginLoaderPtr loader(new QPluginLoader(filename));
        assert(!loader->isLoaded());
        auto metaData = loader->metaData();
        assert(!loader->isLoaded());

        if (metaData.isEmpty()) {
            break;
        }

        auto iidJsonVal = metaData.value(IidMetaKey);
        if (!iidJsonVal.isString()) {
            break;
        }

        ptr.reset(new PluginInfo());
        ptr->m_iid = iidJsonVal.toString();
        ptr->m_loader = std::move(loader);

        auto extraMeta = metaData.value(MetaDataMetaKey);
        if (!extraMeta.isObject()) {
            ptr->m_name = ptr->m_iid;
            break;
        }

        auto extraMetaObj = extraMeta.toObject();
        auto nameJsonVal = extraMetaObj.value(NameMetaKey);
        if (!nameJsonVal.isString()) {
            ptr->m_name = ptr->m_iid;
        }

        auto nameStr = nameJsonVal.toString();
        if (nameStr.isEmpty()) {
            ptr->m_name = ptr->m_iid;
        }
        else {
            ptr->m_name = nameStr;
        }

        auto descJsonVal = extraMetaObj.value(DescMetaKey);
        if (descJsonVal.isString()) {
            ptr->m_desc = descJsonVal.toString();
        }
        else if (descJsonVal.isArray()) {
            auto descVarList = descJsonVal.toArray().toVariantList();
            for (auto& descPartVar : descVarList) {
                if ((descPartVar.isValid()) &&
                    (descPartVar.canConvert<QString>())) {
                    ptr->m_desc.append(descPartVar.toString());
                }
            }
        }

        auto typeJsonVal = extraMetaObj.value(TypeMetaKey);
        ptr->m_type = parseType(typeJsonVal.toString().toLower());

    } while (false);
    return ptr;
}

PluginControlInterfaceImpl* PluginMgr::getPluginControl(PluginInfo::Type type)
{
    if (!m_pluginControls) {
        m_pluginControls.reset(new PluginControls);
    }

    auto typeIdx = static_cast<unsigned>(type);
    assert(typeIdx < m_pluginControls->m_ctrlInterfaces.size());
    return m_pluginControls->m_ctrlInterfaces[typeIdx];
}


}  // namespace comms_champion


