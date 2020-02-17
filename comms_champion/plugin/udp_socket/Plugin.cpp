//
// Copyright 2016 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "Plugin.h"

#include <memory>
#include <cassert>

#include "SocketConfigWidget.h"

namespace comms_champion
{

namespace plugin
{

namespace udp_socket
{

namespace client
{

namespace
{

const QString MainConfigKey("cc_udp_socket");
const QString HostSubKey("host");
const QString PortSubKey("port");
const QString LocalPortSubKey("local_port");
const QString BroadcastPropName("broadcast_prop");

}  // namespace

Plugin::Plugin()
{
    pluginProperties()
        .setSocketCreateFunc(
            [this]() -> SocketPtr
            {
                createSocketIfNeeded();
                return m_socket;
            })
        .setConfigWidgetCreateFunc(
            [this]() -> QWidget*
            {
                createSocketIfNeeded();
                return new SocketConfigWidget(*m_socket);
            });
}

Plugin::~Plugin() noexcept = default;

void Plugin::getCurrentConfigImpl(QVariantMap& config)
{
    static_cast<void>(config);
    createSocketIfNeeded();

    QVariantMap subConfig;
    subConfig.insert(HostSubKey, m_socket->getHost());
    subConfig.insert(PortSubKey, m_socket->getPort());
    subConfig.insert(LocalPortSubKey, m_socket->getLocalPort());
    subConfig.insert(BroadcastPropName, m_socket->getBroadcastPropName());
    config.insert(MainConfigKey, QVariant::fromValue(subConfig));
}

void Plugin::reconfigureImpl(const QVariantMap& config)
{
    static_cast<void>(config);
    auto subConfigVar = config.value(MainConfigKey);
    if ((!subConfigVar.isValid()) || (!subConfigVar.canConvert<QVariantMap>())) {
        return;
    }

    createSocketIfNeeded();
    assert(m_socket);

    auto subConfig = subConfigVar.value<QVariantMap>();
    auto hostVar = subConfig.value(HostSubKey);
    if (hostVar.isValid() && hostVar.canConvert<QString>()) {
        auto host = hostVar.value<QString>();
        m_socket->setHost(host);
    }

    typedef Socket::PortType PortType;
    auto portVar = subConfig.value(PortSubKey);
    if (portVar.isValid() && portVar.canConvert<PortType>()) {
        auto port = portVar.value<PortType>();
        m_socket->setPort(port);
    }

    auto localPortVar = subConfig.value(LocalPortSubKey);
    if (localPortVar.isValid() && localPortVar.canConvert<PortType>()) {
        auto port = localPortVar.value<PortType>();
        m_socket->setLocalPort(port);
    }

    auto broadcastBroadcastNameVar = subConfig.value(BroadcastPropName);
    if (broadcastBroadcastNameVar.isValid() && broadcastBroadcastNameVar.canConvert<QString>()) {
        auto propName = broadcastBroadcastNameVar.value<QString>();
        m_socket->setBroadcastPropName(propName);
    }
}

void Plugin::createSocketIfNeeded()
{
    if (!m_socket) {
        m_socket.reset(new Socket());
    }
}

}  // namespace client

}  // namespace udp_socket

}  // namespace plugin

}  // namespace comms_champion


