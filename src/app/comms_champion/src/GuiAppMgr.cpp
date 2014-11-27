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

#include "comms_champion/DefaultMessageDisplayHandler.h"
#include "GlobalConstants.h"

#include <iostream>

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

void GuiAppMgr::configClicked()
{
    assert(!"Config clicked");
}

void GuiAppMgr::recvStartClicked()
{
    MsgMgr::instanceRef().setRecvEnabled(true);
    m_recvState = RecvState::Running;
    emitRecvStateUpdate();
}

void GuiAppMgr::recvStopClicked()
{
    MsgMgr::instanceRef().setRecvEnabled(false);
    m_recvState = RecvState::Idle;
    emitRecvStateUpdate();
}

void GuiAppMgr::recvSaveClicked()
{
    assert(!"Recv save clicked");
}

void GuiAppMgr::sendStartClicked()
{
    assert(!"Send start clicked");
    m_sendState = SendState::SendingSingle;
    emitSendStateUpdate();
}

void GuiAppMgr::sendStartAllClicked()
{
    assert(!"Send start all clicked");
    m_sendState = SendState::SendingAll;
    emitSendStateUpdate();
}

void GuiAppMgr::sendStopClicked()
{
    assert(!"Send stop clicked");
    m_sendState = SendState::Idle;
    emitSendStateUpdate();
}

void GuiAppMgr::sendSaveClicked()
{
    assert(!"Send save clicked");
}

void GuiAppMgr::recvMsgClicked(Message* msg)
{
    msgClicked(msg);
    bool selectOnAddEnabled = false;
    if (m_clickedMsg == nullptr) {
        selectOnAddEnabled = true;
        emit sigRecvMsgListClearSelection();
    }
    emit sigRecvMsgListSelectOnAddEnabled(selectOnAddEnabled);
}

GuiAppMgr::RecvState GuiAppMgr::recvState() const
{
    return m_recvState;
}

bool GuiAppMgr::recvMsgListSelectOnAddEnabled()
{
    return m_recvListSelectOnAdd;
}

GuiAppMgr::SendState GuiAppMgr::sendState() const
{
    return m_sendState;
}

GuiAppMgr::GuiAppMgr(QObject* parent)
  : Base(parent),
    m_recvState(RecvState::Idle),
    m_sendState(SendState::Idle),
    m_msgDisplayHandler(new DefaultMessageDisplayHandler)
{
    connect(MsgMgr::instance(), SIGNAL(sigMsgReceived(Message*)), this, SLOT(msgReceived(Message*)));
}

void GuiAppMgr::emitRecvStateUpdate()
{
    emit sigSetRecvState(static_cast<int>(m_recvState));
}

void GuiAppMgr::emitSendStateUpdate()
{
    emit sigSetSendState(static_cast<int>(m_sendState));
}

void GuiAppMgr::msgReceived(Message* msg)
{
    std::cout << __FUNCTION__ << ": " << msg->name() << std::endl;
    emit sigAddRecvMsg(msg);
    displayMessageIfNotClicked(msg);
}

void GuiAppMgr::msgClicked(Message* msg)
{
    if (m_clickedMsg == msg) {
        m_clickedMsg = nullptr;
        // TODO: display dummy widget
        return;
    }

    m_clickedMsg = msg;
    displayMessage(msg);
}

void GuiAppMgr::displayMessage(Message* msg)
{
    m_msgWidget = m_msgDisplayHandler->createMsgWidget(*msg);
    m_msgWidget->setEditEnabled(false);
    emit sigDisplayMsgDetailsWidget(m_msgWidget.release());
}

void GuiAppMgr::displayMessageIfNotClicked(Message* msg)
{
    if (m_clickedMsg == nullptr) {
        displayMessage(msg);
    }
}

}  // namespace comms_champion

