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
}

DummySocketPlugin::~DummySocketPlugin()
{
    if (isApplied()) {
        auto interface = getCtrlInterface();
        assert(interface != nullptr);
        assert(m_socket);
        interface->clearSocket();
        m_socket.reset();
    }
}

void DummySocketPlugin::applyImpl()
{
    assert(!isApplied());
    auto interface = getCtrlInterface();
    m_socket.reset(new DummySocket());
    if (interface != nullptr) {
        interface->setSocket(m_socket);
    }

    assert(m_socket);
}

}  // namespace dummy_socket

}  // namespace plugin


}  // namespace comms_champion


