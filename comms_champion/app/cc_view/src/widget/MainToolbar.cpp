//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "MainToolbar.h"

#include "GuiAppMgr.h"
#include "icon.h"

namespace comms_champion
{

MainToolbar::MainToolbar()
{
    auto* config = addAction(icon::pluginEdit(), "Manage and configure plugins");
    QObject::connect(
        config, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(pluginsEditClicked()));

    m_socketConnect = addAction(icon::connect(), "Connect socket");
    QObject::connect(
        m_socketConnect, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(connectSocketClicked()));
    m_socketConnect->setEnabled(false);

    m_socketDisconnect = addAction(icon::disconnect(), "Disconnect socket");
    QObject::connect(
        m_socketDisconnect, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(disconnectSocketClicked()));

    m_socketDisconnect->setVisible(false);
    m_socketConnect->setEnabled(false);
    addSeparator();

    QObject::connect(
        GuiAppMgr::instance(), SIGNAL(sigSocketConnected(bool)),
        this, SLOT(socketConnected(bool)));

    QObject::connect(
        GuiAppMgr::instance(), SIGNAL(sigSocketConnectEnabled(bool)),
        this, SLOT(socketConnectEnabled(bool)));
}

void MainToolbar::socketConnected(bool connected)
{
    m_socketConnect->setVisible(!connected);
    m_socketDisconnect->setVisible(connected);
}

void MainToolbar::socketConnectEnabled(bool enabled)
{
    m_socketConnect->setEnabled(enabled);
    m_socketDisconnect->setEnabled(enabled);
}

}  // namespace comms_champion


