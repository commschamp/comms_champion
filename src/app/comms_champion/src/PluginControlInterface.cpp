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

#include "comms_champion/PluginControlInterface.h"

#include <cassert>

#include "comms_champion/version.h"

#include "MsgMgr.h"
#include "GuiAppMgr.h"

namespace comms_champion
{

unsigned PluginControlInterface::version()
{
    return COMMS_CHAMPION_VERSION;
}

void PluginControlInterface::setProtocol(ProtocolPtr protocol)
{
    MsgMgr::instanceRef().setProtocol(std::move(protocol));
}

void PluginControlInterface::clearProtocol()
{
    MsgMgr::instanceRef().setProtocol(ProtocolPtr());
}

void PluginControlInterface::addSocket(SocketPtr socket)
{
    MsgMgr::instanceRef().addSocket(std::move(socket));
}

void PluginControlInterface::removeSocket(SocketPtr socket)
{
    MsgMgr::instanceRef().removeSocket(std::move(socket));
}

void PluginControlInterface::addMainToolbarAction(ActionPtr action)
{
    auto* guiAppMgr = GuiAppMgr::instance();
    assert(guiAppMgr != nullptr);
    guiAppMgr->addMainToolbarAction(std::move(action));
}

void PluginControlInterface::removeMainToolbarAction(ActionPtr action)
{
    auto* guiAppMgr = GuiAppMgr::instance();
    if (guiAppMgr != nullptr) {
        guiAppMgr->removeMainToolbarAction(std::move(action));
    }
}

}  // namespace comms_champion


