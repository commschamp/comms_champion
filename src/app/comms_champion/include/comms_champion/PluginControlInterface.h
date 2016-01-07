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


#pragma once

#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QAction>
CC_ENABLE_WARNINGS()

#include "Protocol.h"
#include "Socket.h"

namespace comms_champion
{

class PluginControlInterfaceImpl;
class PluginControlInterface
{
public:
    typedef std::shared_ptr<QAction> ActionPtr;

    explicit PluginControlInterface(PluginControlInterfaceImpl& impl);
    PluginControlInterface(const PluginControlInterface&) = default;
    ~PluginControlInterface();

    void setProtocol(ProtocolPtr protocol);
    void clearProtocol();
    void setSocket(SocketPtr socket);
    void clearSocket();
    void addMainToolbarAction(ActionPtr action);
    void removeMainToolbarAction(ActionPtr action);

private:
    PluginControlInterfaceImpl& m_impl;
};

}  // namespace comms_champion


