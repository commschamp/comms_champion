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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
CC_ENABLE_WARNINGS()

#include "comms_champion/version.h"

#include "MsgMgr.h"
#include "GuiAppMgr.h"
#include "PluginControlInterfaceImpl.h"

namespace comms_champion
{

PluginControlInterface::PluginControlInterface(PluginControlInterfaceImpl& impl)
  : m_impl(impl)
{
}

PluginControlInterface::~PluginControlInterface() = default;

unsigned PluginControlInterface::version()
{
    return COMMS_CHAMPION_VERSION;
}

void PluginControlInterface::setProtocol(ProtocolPtr protocol)
{
    m_impl.setProtocol(std::move(protocol));
}

void PluginControlInterface::clearProtocol()
{
    m_impl.clearProtocol();
}

void PluginControlInterface::setSocket(SocketPtr socket)
{
    m_impl.setSocket(std::move(socket));
}

void PluginControlInterface::clearSocket()
{
    m_impl.clearSocket();
}

void PluginControlInterface::addMainToolbarAction(ActionPtr action)
{
    m_impl.addMainToolbarAction(std::move(action));
}

void PluginControlInterface::removeMainToolbarAction(ActionPtr action)
{
    m_impl.removeMainToolbarAction(std::move(action));
}

}  // namespace comms_champion


