//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/Plugin.h"

namespace comms_champion
{

namespace
{

template <typename TFunc>
auto invokeCreationFunc(TFunc&& func) -> decltype(func())
{
    typedef decltype(func()) RetType;

    if (!func) {
        return RetType();
    }

    return func();
}

}  // namespace

Plugin::Plugin() = default;
Plugin::~Plugin() noexcept = default;

void Plugin::getCurrentConfig(QVariantMap& config)
{
    getCurrentConfigImpl(config);
}

QVariantMap Plugin::getCurrentConfig()
{
    QVariantMap config;
    getCurrentConfig(config);
    return config;
}

void Plugin::reconfigure(const QVariantMap& config)
{
    reconfigureImpl(config);
}

SocketPtr Plugin::createSocket() const
{
    return invokeCreationFunc(m_props.getSocketCreateFunc());
}

Plugin::ListOfFilters Plugin::createFilters() const
{
    return invokeCreationFunc(m_props.getFiltersCreateFunc());
}

ProtocolPtr Plugin::createProtocol() const
{
    return invokeCreationFunc(m_props.getProtocolCreateFunc());
}

Plugin::ListOfGuiActions Plugin::createGuiActions() const
{
    return invokeCreationFunc(m_props.getGuiActionsCreateFunc());
}

QWidget* Plugin::createConfiguarionWidget() const
{
    auto func = m_props.getConfigWidgetCreateFunc();
    if (!func) {
        return nullptr;
    }

    return func();
}

QVariant Plugin::getCustomProperty(const QString& name)
{
    return m_props.getCustomProperty(name);
}

void Plugin::getCurrentConfigImpl(QVariantMap& config)
{
    static_cast<void>(config);
}

void Plugin::reconfigureImpl(const QVariantMap& config)
{
    static_cast<void>(config);
}

PluginProperties& Plugin::pluginProperties()
{
    return m_props;
}

}  // namespace comms_champion


