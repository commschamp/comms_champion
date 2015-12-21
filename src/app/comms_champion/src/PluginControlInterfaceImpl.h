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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtWidgets/QAction>
CC_ENABLE_WARNINGS()

#include "comms_champion/Protocol.h"
#include "comms_champion/Socket.h"

namespace comms_champion
{

class PluginControlInterfaceImpl : public QObject
{
    Q_OBJECT
public:
    typedef std::shared_ptr<QAction> ActionPtr;

    PluginControlInterfaceImpl();

    void setProtocol(ProtocolPtr protocol)
    {
        setProtocolImpl(std::move(protocol));
    }

    void clearProtocol()
    {
        clearProtocolImpl();
    }

    void setSocket(SocketPtr socket)
    {
        setSocketImpl(std::move(socket));
    }

    void clearSocket()
    {
        clearSocketImpl();
    }

    void addMainToolbarAction(ActionPtr action);
    void removeMainToolbarAction(ActionPtr action);

signals:
    void sigSetProtocol(ProtocolPtr protocol);
    void sigSetSocket(SocketPtr socket);
    void sigAddMainToolbarAction(ActionPtr action);
    void sigRemoveMainToolbarAction(ActionPtr action);

protected:
    virtual void setProtocolImpl(ProtocolPtr protocol);
    virtual void clearProtocolImpl();
    virtual void setSocketImpl(SocketPtr socket);
    virtual void clearSocketImpl();

    void emitSigSetProtocol(ProtocolPtr protocol);
    void emitSigSetSocket(SocketPtr socket);
};

}  // namespace comms_champion




