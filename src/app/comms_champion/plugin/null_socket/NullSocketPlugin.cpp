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

#include "NullSocketPlugin.h"

#include <memory>
#include <cassert>

#include "NullSocket.h"

namespace comms_champion
{

namespace plugin
{

namespace dummy_socket
{

class NullSocketPluginImpl
{
public:
    NullSocketPluginImpl();

private:
};

NullSocketPlugin::NullSocketPlugin()
{
}

NullSocketPlugin::~NullSocketPlugin()
{
    if (isApplied()) {
        auto& interface = getCtrlInterface();
        assert(m_socket);
        interface.clearSocket();
        m_socket.reset();
    }
}

void NullSocketPlugin::applyImpl()
{
    assert(!isApplied());
    auto& interface = getCtrlInterface();
    m_socket = makeNullSocket();
    interface.setSocket(m_socket);
    assert(m_socket);
}

}  // namespace dummy_socket

}  // namespace plugin


}  // namespace comms_champion


