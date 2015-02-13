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

#include "DummySocketPlugin.h"

#include <memory>
#include <iostream>
#include <cassert>

#include "DummySocket.h"

namespace comms_champion
{

namespace plugin
{

namespace dummy_socket
{

class DummySocketPluginImpl
{
public:
    DummySocketPluginImpl();

private:
};

DummySocketPlugin::DummySocketPlugin()
{
    std::cout << "Initialising DummySocketPlugin at " << this  << std::endl;
}

DummySocketPlugin::~DummySocketPlugin()
{
    std::cout << "Finalising DummySocketPlugin at " << this << std::endl;
    if (isApplied()) {
        assert(m_interface != nullptr);
        assert(m_socket);
        m_interface->removeSocket(m_socket);
        m_socket.reset();
    }
}

void DummySocketPlugin::applyImpl(
    const PluginControlInterface& controlInterface)
{
    assert(!isApplied());
    m_interface = &controlInterface;
    m_socket.reset(new DummySocket());
    controlInterface.addSocket(m_socket);
    std::cout << "Applied DummySocketPlugin at " << this << "; socket is at " << m_socket.get() << std::endl;
    assert(m_socket);
}

}  // namespace dummy_socket

}  // namespace plugin


}  // namespace comms_champion


