//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include "ClientSocketPlugin.h"

#include <memory>
#include <cassert>

#include "ClientSocketConfigWidget.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

namespace
{

const QString MainConfigKey("cc_tcp_client_socket");
const QString HostSubKey("host");
const QString PortSubKey("port");

}  // namespace

ClientSocketPlugin::ClientSocketPlugin()
{
}

ClientSocketPlugin::~ClientSocketPlugin()
{
    if (isApplied()) {
        auto& interface = getCtrlInterface();
        assert(interface);
        assert(m_socket);
        interface->clearSocket();
        m_socket.reset();

        assert(m_connectAction);
        interface->removeMainToolbarAction(m_connectAction);
    }
}

void ClientSocketPlugin::applyImpl()
{
    assert(!isApplied());
    createSocketIfNeeded();
    createConnectIconIfNeeded();

    auto& interface = getCtrlInterface();
    if (interface) {
        interface->setSocket(m_socket);
        interface->addMainToolbarAction(m_connectAction);
    }

    assert(m_socket);
}

void ClientSocketPlugin::getCurrentConfigImpl(QVariantMap& config)
{
    static_cast<void>(config);
    createSocketIfNeeded();

    QVariantMap subConfig;
    subConfig.insert(HostSubKey, QVariant::fromValue(m_socket->getHost()));
    subConfig.insert(PortSubKey, QVariant::fromValue(m_socket->getPort()));
    config.insert(MainConfigKey, QVariant::fromValue(subConfig));
}

void ClientSocketPlugin::reconfigureImpl(const QVariantMap& config)
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

    typedef ClientSocket::PortType PortType;
    auto portVar = subConfig.value(PortSubKey);
    if (portVar.isValid() && portVar.canConvert<PortType>()) {
        auto port = portVar.value<PortType>();
        m_socket->setPort(port);
    }
}

ClientSocketPlugin::WidgetPtr ClientSocketPlugin::getConfigWidgetImpl()
{
    createSocketIfNeeded();
    assert(m_socket);
    return WidgetPtr(new ClientSocketConfigWidget(*m_socket));
}

void ClientSocketPlugin::connectStatusChangeRequest(bool connected)
{
    assert(m_socket);
    m_socket->setConnected(connected);
}

void ClientSocketPlugin::connectionStatusChanged(bool connected)
{
    assert(m_connectAction);
    m_connectAction->setConnected(connected);
}

void ClientSocketPlugin::createSocketIfNeeded()
{
    if (!m_socket) {
        m_socket.reset(new ClientSocket());
        connect(
            m_socket.get(), SIGNAL(sigConnectionStatus(bool)),
            this, SLOT(connectionStatusChanged(bool)));
    }
}

void ClientSocketPlugin::createConnectIconIfNeeded()
{
    if (!m_connectAction) {
        m_connectAction.reset(new ClientConnectAction());
        connect(
            m_connectAction.get(), SIGNAL(sigConnectStateChangeReq(bool)),
            this, SLOT(connectStatusChangeRequest(bool)));
    }
}

}  // namespace tcp_socket

}  // namespace plugin


}  // namespace comms_champion


