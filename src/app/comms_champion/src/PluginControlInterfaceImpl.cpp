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

#include "PluginControlInterfaceImpl.h"

#include <cassert>

#include "MsgMgr.h"
#include "GuiAppMgr.h"

namespace comms_champion
{

PluginControlInterfaceImpl::PluginControlInterfaceImpl()
{
    auto* msgMgr = MsgMgr::instance();
    assert(msgMgr != nullptr);
    connect(
        this, SIGNAL(sigSetProtocol(ProtocolPtr)),
        msgMgr, SLOT(setProtocol(ProtocolPtr)));
    connect(
        this, SIGNAL(sigAddSocket(SocketPtr)),
        msgMgr, SLOT(addSocket(SocketPtr)));
    connect(
        this, SIGNAL(sigRemoveSocket(SocketPtr)),
        msgMgr, SLOT(removeSocket(SocketPtr)));

    auto* guiAppMgr = GuiAppMgr::instance();
    assert(guiAppMgr != nullptr);
    connect(
        this, SIGNAL(sigAddMainToolbarAction(ActionPtr)),
        guiAppMgr, SLOT(addMainToolbarAction(ActionPtr)));
    connect(
        this, SIGNAL(sigRemoveMainToolbarAction(ActionPtr)),
        guiAppMgr, SLOT(removeMainToolbarAction(ActionPtr)));
}

void PluginControlInterfaceImpl::setProtocol(ProtocolPtr protocol)
{
    emit sigSetProtocol(std::move(protocol));
}

void PluginControlInterfaceImpl::clearProtocol()
{
    emit sigSetProtocol(ProtocolPtr());
}

void PluginControlInterfaceImpl::addSocket(SocketPtr socket)
{
    emit sigAddSocket(std::move(socket));
}

void PluginControlInterfaceImpl::removeSocket(SocketPtr socket)
{
    emit sigRemoveSocket(std::move(socket));
}

void PluginControlInterfaceImpl::addMainToolbarAction(ActionPtr action)
{
    emit sigAddMainToolbarAction(std::move(action));
}

void PluginControlInterfaceImpl::removeMainToolbarAction(ActionPtr action)
{
    emit sigRemoveMainToolbarAction(std::move(action));
}



}  // namespace comms_champion

