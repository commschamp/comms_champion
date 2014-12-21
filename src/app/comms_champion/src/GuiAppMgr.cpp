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
#include <memory>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QtQml>
#include <QtWidgets/QLabel>

#include "comms_champion/DefaultMessageDisplayHandler.h"
#include "GlobalConstants.h"

#include <iostream>

namespace comms_champion
{

GuiAppMgr* GuiAppMgr::instance()
{
    static std::unique_ptr<GuiAppMgr> mgr(new GuiAppMgr());
    return mgr.get();
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

void GuiAppMgr::recvMsgClicked(ProtocolsInfoPtr msgInfo)
{
    msgClicked(msgInfo);
    bool selectOnAddEnabled = false;
    if (!m_clickedMsg) {
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
    m_sendState(SendState::Idle)
{
    connect(MsgMgr::instance(), SIGNAL(sigMsgReceived(ProtocolsInfoPtr)),
            this, SLOT(msgReceived(ProtocolsInfoPtr)));
}

void GuiAppMgr::emitRecvStateUpdate()
{
    emit sigSetRecvState(static_cast<int>(m_recvState));
}

void GuiAppMgr::emitSendStateUpdate()
{
    emit sigSetSendState(static_cast<int>(m_sendState));
}

void GuiAppMgr::msgReceived(ProtocolsInfoPtr protocolsInfo)
{
    assert(protocolsInfo);
#ifndef NDEBUG
    auto msgInfo = protocolsInfo->back();
    assert(msgInfo);
    auto msg = msgInfo->getAppMessage();
    assert(msg);
    std::cout << __FUNCTION__ << ": " << msg->name() << std::endl;
#endif
    emit sigAddRecvMsg(protocolsInfo);
    displayMessageIfNotClicked(protocolsInfo);
}

void GuiAppMgr::msgClicked(ProtocolsInfoPtr protocolsInfo)
{
    assert(protocolsInfo);
    if (m_clickedMsg == protocolsInfo) {
        m_clickedMsg.reset();
        // TODO: display dummy widget
        return;
    }

    m_clickedMsg = protocolsInfo;
    displayMessage(protocolsInfo);
}

void GuiAppMgr::displayMessage(ProtocolsInfoPtr protocolsInfo)
{
    emit sigDisplayMsg(protocolsInfo);
}

void GuiAppMgr::displayMessageIfNotClicked(ProtocolsInfoPtr protocolsInfo)
{
    if (!m_clickedMsg) {
        displayMessage(protocolsInfo);
    }
}

}  // namespace comms_champion

