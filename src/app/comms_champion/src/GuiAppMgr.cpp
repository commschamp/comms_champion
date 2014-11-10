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
#include <QtWidgets/QLabel>

#include "GlobalConstants.h"


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
    m_recvState = GlobalConstants::runningState();
    emitRecvStateUpdate();
}

void GuiAppMgr::recvStopClicked()
{
    m_recvState = GlobalConstants::waitingState();
    emitRecvStateUpdate();
}

void GuiAppMgr::recvSaveClicked()
{

}

void GuiAppMgr::sendStartClicked()
{
    m_sendState = GlobalConstants::sendingState();
    emitSendStateUpdate();
}

void GuiAppMgr::sendStartAllClicked()
{
    m_sendState = GlobalConstants::sendingAllState();
    emitSendStateUpdate();
}

void GuiAppMgr::sendStopClicked()
{
    m_sendState = GlobalConstants::waitingState();
    emitSendStateUpdate();
}

void GuiAppMgr::sendSaveClicked()
{

}

const QString& GuiAppMgr::recvState() const
{
    return m_recvState;
}

const QString& GuiAppMgr::sendState() const
{
    return m_recvState;
}

GuiAppMgr::GuiAppMgr(QObject* parent)
  : Base(parent),
    m_recvState(GlobalConstants::waitingState()),
    m_sendState(GlobalConstants::waitingState())
{
    connect(MsgMgr::instance(), SIGNAL(sigMsgReceived(Message*)), this, SLOT(msgReceived(Message*)));
}

void GuiAppMgr::emitRecvStateUpdate()
{
    emit sigSetRecvState(m_recvState);
}

void GuiAppMgr::emitSendStateUpdate()
{
    emit sigSetSendState(m_sendState);
}

void GuiAppMgr::msgReceived(Message* msg)
{
    emit sigAddRecvMsg(msg);
//    emit sigDisplayMsgDetailsWidget(label);
}

}  // namespace comms_champion

