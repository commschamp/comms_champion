//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "SocketPlugin.h"

#include <memory>
#include <cassert>

#include "Socket.h"
#include "SocketConfigWidget.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

namespace proxy
{

namespace
{

const QString MainConfigKey("cc_tcp_proxy_socket");
const QString LocalPortSubKey("local_port");
const QString RemoteHostSubKey("remote_host");
const QString RemotePortSubKey("remote_port");

}  // namespace

SocketPlugin::SocketPlugin()
{
    pluginProperties()
        .setSocketCreateFunc(
            [this]()
            {
                createSocketIfNeeded();
                return m_socket;
            })
        .setConfigWidgetCreateFunc(
            [this]()
            {
                createSocketIfNeeded();
                return new SocketConfigWidget(*m_socket);
            });
}

SocketPlugin::~SocketPlugin() noexcept = default;

void SocketPlugin::getCurrentConfigImpl(QVariantMap& config)
{
    createSocketIfNeeded();

    QVariantMap subConfig;
    subConfig.insert(LocalPortSubKey, QVariant::fromValue(m_socket->getPort()));
    subConfig.insert(RemoteHostSubKey, QVariant::fromValue(m_socket->getRemoteHost()));
    subConfig.insert(RemotePortSubKey, QVariant::fromValue(m_socket->getRemotePort()));
    config.insert(MainConfigKey, QVariant::fromValue(subConfig));
}

void SocketPlugin::reconfigureImpl(const QVariantMap& config)
{
    auto subConfigVar = config.value(MainConfigKey);
    if ((!subConfigVar.isValid()) || (!subConfigVar.canConvert<QVariantMap>())) {
        return;
    }

    typedef Socket::PortType PortType;
    auto subConfig = subConfigVar.value<QVariantMap>();
    auto localPortVar = subConfig.value(LocalPortSubKey);
    if ((!localPortVar.isValid()) || (!localPortVar.canConvert<PortType>())) {
        return;
    }

    auto remoteHostVar = subConfig.value(RemoteHostSubKey);
    if ((!remoteHostVar.isValid()) || (!remoteHostVar.canConvert<QString>())) {
        return;
    }

    auto remotePortVar = subConfig.value(RemotePortSubKey);
    if ((!remotePortVar.isValid()) || (!remotePortVar.canConvert<PortType>())) {
        return;
    }

    auto localPort = localPortVar.value<PortType>();
    auto remoteHost = remoteHostVar.value<QString>();
    auto remotePort = remotePortVar.value<PortType>();

    createSocketIfNeeded();

    m_socket->setPort(localPort);
    m_socket->setRemoteHost(remoteHost);
    m_socket->setRemotePort(remotePort);
}

void SocketPlugin::createSocketIfNeeded()
{
    if (!m_socket) {
        m_socket.reset(new Socket());
    }
}

}  // namespace proxy

}  // namespace tcp_socket

}  // namespace plugin

}  // namespace comms_champion


