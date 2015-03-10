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

#include <QtWidgets/QAction>

#include "Protocol.h"
#include "Socket.h"

namespace comms_champion
{

class PluginControlInterfaceImpl;
class PluginControlInterface
{
public:
    typedef std::shared_ptr<QAction> ActionPtr;

    PluginControlInterface();
    ~PluginControlInterface();

    static unsigned version();
    void setProtocol(ProtocolPtr protocol);
    void clearProtocol();
    void addSocket(SocketPtr socket);
    void removeSocket(SocketPtr socket);
    void addMainToolbarAction(ActionPtr action);
    void removeMainToolbarAction(ActionPtr action);

private:
    std::unique_ptr<PluginControlInterfaceImpl> m_pImpl;
};

}  // namespace comms_champion


