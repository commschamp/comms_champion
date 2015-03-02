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

#include "ServerSocketPlugin.h"

#include <memory>
#include <iostream>
#include <cassert>

#include "ServerSocket.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

ServerSocketPlugin::ServerSocketPlugin()
{
    std::cout << "Initialising TCP ServerSocketPlugin at " << this  << std::endl;
}

ServerSocketPlugin::~ServerSocketPlugin()
{
    std::cout << "Finalising TCP ServerSocketPlugin at " << this << std::endl;
    if (isApplied()) {
        assert(m_interface != nullptr);
        assert(m_socket);
        m_interface->removeSocket(m_socket);
        m_socket.reset();
    }
}

void ServerSocketPlugin::applyImpl(
    const PluginControlInterface& controlInterface)
{
    assert(!isApplied());
    m_interface = &controlInterface;
    m_socket.reset(new ServerSocket());

    // TODO: do proper configuration
    m_socket->setPort(20000);

    controlInterface.addSocket(m_socket);
    std::cout << "Applied TCP ServerSocketPlugin at " << this << "; socket is at " << m_socket.get() << std::endl;
    assert(m_socket);
}

}  // namespace tcp_socket

}  // namespace plugin


}  // namespace comms_champion


