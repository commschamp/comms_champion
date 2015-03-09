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

#include "comms_champion/Plugin.h"

#include "ClientSocket.h"
#include "ClientConnectAction.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

class ClientSocketPlugin : public comms_champion::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "cc.TcpClientSocketPlugin" FILE "tcp_client_socket.json")
    Q_INTERFACES(comms_champion::Plugin)

public:
    ClientSocketPlugin();
    ~ClientSocketPlugin();

    virtual void applyImpl(const PluginControlInterface& controlInterface) override;
    virtual void getCurrentConfigImpl(QVariantMap& config) override;
    virtual void reconfigureImpl(const QVariantMap& config) override;
    virtual WidgetPtr getConfigWidgetImpl() override;

private slots:
    void connectStatusChangeRequest(bool connected);
    void connectionStatusChanged(bool connected);

private:

    void createSocketIfNeeded();
    void createConnectIconIfNeeded();

    std::shared_ptr<ClientSocket> m_socket;
    std::shared_ptr<ClientConnectAction> m_connectAction;
    const PluginControlInterface* m_interface = nullptr;
};

}  // namespace tcp_socket

}  // namespace plugin

}  // namespace comms_champion




