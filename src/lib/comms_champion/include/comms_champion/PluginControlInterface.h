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

#include "Api.h"
#include "Protocol.h"
#include "Socket.h"

namespace comms_champion
{

class CC_API PluginControlInterface
{
public:
    typedef std::shared_ptr<QAction> ActionPtr;

    PluginControlInterface();
    PluginControlInterface(const PluginControlInterface&);
    virtual ~PluginControlInterface();

    virtual void setProtocol(ProtocolPtr protocol) = 0;
    virtual void clearProtocol() = 0;
    virtual void setSocket(SocketPtr socket) = 0;
    virtual void clearSocket() = 0;
    virtual void addMainToolbarAction(ActionPtr action) = 0;
    virtual void removeMainToolbarAction(ActionPtr action) = 0;
};

}  // namespace comms_champion


