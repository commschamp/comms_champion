//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "ConnectAction.h"

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtGui/QIcon>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

namespace client
{

ConnectAction::ConnectAction(
    bool connected,
    QWidget* parentObj)
  : Base(parentObj),
    m_connected(connected)
{
    refresh();

    connect(
        this, SIGNAL(triggered()),
        this, SLOT(iconClicked()));
}

void ConnectAction::setConnected(bool connected)
{
    m_connected = connected;
    refresh();
}

void ConnectAction::iconClicked()
{
    emit sigConnectStateChangeReq(!m_connected);
}

void ConnectAction::refresh()
{
    if (m_connected) {
        static const QIcon disconnectIcon(":/image/disconnect.png");
        static const QString disconnectTooltip("Disconnect from server");
        setIcon(disconnectIcon);
        setToolTip(disconnectTooltip);
    }
    else {
        static const QIcon connectIcon(":/image/server_connect.png");
        static const QString connectTooltip("Connect to server");
        setIcon(connectIcon);
        setToolTip(connectTooltip);
    }
}

}  // namespace client

}  // namespace tcp_socket

}  // namespace plugin

}  // namespace comms_champion


