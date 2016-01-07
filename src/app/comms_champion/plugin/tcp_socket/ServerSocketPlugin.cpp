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

#include "ServerSocketPlugin.h"

#include <memory>
#include <cassert>

#include "ServerSocket.h"
#include "ServerSocketConfigWidget.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

namespace
{

const QString MainConfigKey("cc_tcp_server_socket");
const QString PortSubKey("port");

}  // namespace

ServerSocketPlugin::ServerSocketPlugin()
{
}

ServerSocketPlugin::~ServerSocketPlugin()
{
    if (isApplied()) {
        auto& interface = getCtrlInterface();
        assert(m_socket);
        interface.clearSocket();
        m_socket.reset();
    }
}

void ServerSocketPlugin::applyImpl()
{
    assert(!isApplied());
    createSocketIfNeeded();

    auto& interface = getCtrlInterface();
    interface.setSocket(m_socket);
    assert(m_socket);
}

void ServerSocketPlugin::getCurrentConfigImpl(QVariantMap& config)
{
    createSocketIfNeeded();

    QVariantMap subConfig;
    subConfig.insert(PortSubKey, QVariant::fromValue(m_socket->getPort()));
    config.insert(MainConfigKey, QVariant::fromValue(subConfig));
}

void ServerSocketPlugin::reconfigureImpl(const QVariantMap& config)
{
    auto subConfigVar = config.value(MainConfigKey);
    if ((!subConfigVar.isValid()) || (!subConfigVar.canConvert<QVariantMap>())) {
        return;
    }

    typedef ServerSocket::PortType PortType;
    auto subConfig = subConfigVar.value<QVariantMap>();
    auto portVar = subConfig.value(PortSubKey);
    if ((!portVar.isValid()) || (!portVar.canConvert<PortType>())) {
        return;
    }

    auto port = portVar.value<PortType>();

    createSocketIfNeeded();

    m_socket->setPort(port);
}

ServerSocketPlugin::WidgetPtr ServerSocketPlugin::getConfigWidgetImpl()
{
    createSocketIfNeeded();
    assert(m_socket);
    return WidgetPtr(new ServerSocketConfigWidget(*m_socket));
}

void ServerSocketPlugin::createSocketIfNeeded()
{
    if (!m_socket) {
        m_socket.reset(new ServerSocket());
    }
}

}  // namespace tcp_socket

}  // namespace plugin

}  // namespace comms_champion


