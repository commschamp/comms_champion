//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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


#include "GuiAppMgr.h"

#include <cassert>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QtQml>


namespace comms_champion
{

namespace
{

QObject *getGuiAppMgr(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return GuiAppMgr::instance();
}

}  // namespace

GuiAppMgr* GuiAppMgr::instance()
{
    static GuiAppMgr* mgr = new GuiAppMgr();
    return mgr;
}

void GuiAppMgr::qmlRegister()
{
    qmlRegisterSingletonType<GuiAppMgr>("cc.GuiAppMgr", 1, 0, "GuiAppMgr", &getGuiAppMgr);
}


void GuiAppMgr::recvStartClicked()
{

}

void GuiAppMgr::recvStopClicked()
{

}

GuiAppMgr::GuiAppMgr(QObject* parent)
  : Base(parent)
{
    connect(MsgMgr::instance(), SIGNAL(msgReceived(Message*)), this, SLOT(msgReceived(Message*)));
}

void GuiAppMgr::msgReceived(Message* msg)
{
    emit sigAddRecvMsg(msg);
}

}  // namespace comms_champion

