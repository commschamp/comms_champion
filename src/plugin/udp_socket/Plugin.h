//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <memory>

#include "comms_champion/Plugin.h"

#include "Socket.h"

namespace comms_champion
{

namespace plugin
{

namespace udp_socket
{

namespace client
{

class Plugin : public comms_champion::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "cc.UdpSocketPlugin" FILE "udp_socket.json")
    Q_INTERFACES(comms_champion::Plugin)

public:
    Plugin();
    ~Plugin();

    virtual void getCurrentConfigImpl(QVariantMap& config) override;
    virtual void reconfigureImpl(const QVariantMap& config) override;

private:

    void createSocketIfNeeded();

    std::shared_ptr<Socket> m_socket;
};

}  // namespace client

}  // namespace udp_socket

}  // namespace plugin

}  // namespace comms_champion




