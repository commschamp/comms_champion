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

void GuiAppMgr::sendAddClicked()
{
    emit sigNewSendMsgDialog(MsgMgr::instanceRef().getProtocol());
}

void GuiAppMgr::sendEditClicked()
{
    assert(m_clickedMsg);
    emit sigUpdateSendMsgDialog(m_clickedMsg, MsgMgr::instanceRef().getProtocol());
}

void GuiAppMgr::sendDeleteClicked()
{
    emit sigSendDeleteSelectedMsg();
}

void GuiAppMgr::recvMsgClicked(MessageInfoPtr msgInfo)
{
    emit sigSendMsgListClearSelection();
    emit sigSendMsgSelected(false);

    msgClicked(msgInfo);
    if (!m_clickedMsg) {
        emit sigRecvMsgListClearSelection();
    }
    emit sigRecvMsgSelected(static_cast<bool>(m_clickedMsg));
}

void GuiAppMgr::sendMsgClicked(MessageInfoPtr msgInfo)
{
    emit sigRecvMsgListClearSelection();
    emit sigRecvMsgSelected(false);

    msgClicked(msgInfo);
    if (!m_clickedMsg) {
        emit sigSendMsgListClearSelection();
    }
    emit sigSendMsgSelected(static_cast<bool>(m_clickedMsg));
}

void GuiAppMgr::sendMsgDoubleClicked(MessageInfoPtr msgInfo)
{
    // Equivalent to selection + edit
    assert(msgInfo);
    if (msgInfo != m_clickedMsg) {
        sendMsgClicked(msgInfo);
    }
    assert(m_clickedMsg == msgInfo);
    sendEditClicked();
}

void GuiAppMgr::sendMsgDeleted(MessageInfoPtr msgInfo)
{
    static_cast<void>(msgInfo);
    assert(!sendListEmpty());
    assert(m_clickedMsg == msgInfo);
    clearDisplayedMessage();
    --m_sendListCount;
    if (sendListEmpty()) {
        emit sigSendListEmpty(true);
    }
    emit sigSendMsgSelected(false);
}

GuiAppMgr::RecvState GuiAppMgr::recvState() const
{
    return m_recvState;
}

bool GuiAppMgr::recvMsgListSelectOnAddEnabled()
{
    return m_recvListSelectOnAdd;
}

bool GuiAppMgr::recvListEmpty() const
{
    return m_recvListCount == 0;
}

GuiAppMgr::SendState GuiAppMgr::sendState() const
{
    return m_sendState;
}

void GuiAppMgr::sendAddNewMessage(MessageInfoPtr msgInfo)
{
    bool wasEmpty = sendListEmpty();
    ++m_sendListCount;
    emit sigAddSendMsg(msgInfo);
    if (wasEmpty) {
        emit sigSendListEmpty(false);
    }
    sendMsgClicked(msgInfo);
}

void GuiAppMgr::sendUpdateMessage(MessageInfoPtr msgInfo)
{
    assert(!sendListEmpty());
    assert(m_clickedMsg);
    assert(m_clickedMsg == msgInfo);
    emit sigSendMsgUpdated();
    displayMessage(std::move(msgInfo));
}

bool GuiAppMgr::sendListEmpty() const
{
    return m_sendListCount == 0;
}

GuiAppMgr::GuiAppMgr(QObject* parent)
  : Base(parent),
    m_recvState(RecvState::Idle),
    m_sendState(SendState::Idle)
{
    connect(MsgMgr::instance(), SIGNAL(sigMsgReceived(MessageInfoPtr)),
            this, SLOT(msgReceived(MessageInfoPtr)));
}

void GuiAppMgr::emitRecvStateUpdate()
{
    emit sigSetRecvState(static_cast<int>(m_recvState));
}

void GuiAppMgr::emitSendStateUpdate()
{
    emit sigSetSendState(static_cast<int>(m_sendState));
}

void GuiAppMgr::msgReceived(MessageInfoPtr msgInfo)
{
#ifndef NDEBUG
    assert(msgInfo);
    auto msg = msgInfo->getAppMessage();
    assert(msg);
    std::cout << __FUNCTION__ << ": " << msg->name() << std::endl;
#endif
    bool wasEmpty = recvListEmpty();
    ++m_recvListCount;
    emit sigAddRecvMsg(msgInfo);
    if (wasEmpty) {
        emit sigRecvListEmpty(false);
    }
    displayMessageIfNotClicked(msgInfo);
}

void GuiAppMgr::msgClicked(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    if (m_clickedMsg == msgInfo) {
        clearDisplayedMessage();
        emit sigRecvMsgListSelectOnAddEnabled(true);
        return;
    }

    m_clickedMsg = msgInfo;
    displayMessage(m_clickedMsg);
    emit sigRecvMsgListSelectOnAddEnabled(false);
}

void GuiAppMgr::displayMessage(MessageInfoPtr msgInfo)
{
    emit sigDisplayMsg(msgInfo);
}

void GuiAppMgr::displayMessageIfNotClicked(MessageInfoPtr msgInfo)
{
    if (!m_clickedMsg) {
        displayMessage(msgInfo);
    }
}

void GuiAppMgr::clearDisplayedMessage()
{
    m_clickedMsg.reset();
    emit sigClearDisplayedMsg();
}

}  // namespace comms_champion

