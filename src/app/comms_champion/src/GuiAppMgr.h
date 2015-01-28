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


#pragma once

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include "comms_champion/Message.h"

#include "MsgMgr.h"

namespace comms_champion
{

class GuiAppMgr : public QObject
{
    Q_OBJECT
    typedef QObject Base;
public:

    typedef MsgMgr::MsgInfosList MsgInfosList;

    enum class RecvState {
        Idle,
        Running,
        NumOfStates
    };

    enum class SendState {
        Idle,
        SendingSingle,
        SendingAll,
        NumOfStates
    };

    typedef MsgMgr::MsgType MsgType;

    static GuiAppMgr* instance();

    RecvState recvState() const;
    bool recvMsgListSelectOnAddEnabled();
    bool recvListEmpty() const;

    SendState sendState() const;
    void sendAddNewMessage(MessageInfoPtr msgInfo);
    void sendUpdateMessage(MessageInfoPtr msgInfo);
    bool sendListEmpty() const;
    void sendMessages(MsgInfosList&& msgs);

public slots:
    void configClicked();

    void recvStartClicked();
    void recvStopClicked();
    void recvSaveClicked();
    void recvDeleteClicked();
    void recvClearClicked();

    void sendStartClicked();
    void sendStartAllClicked();
    void sendStopClicked();
    void sendSaveClicked();
    void sendAddClicked();
    void sendEditClicked();
    void sendDeleteClicked();
    void sendClearClicked();

    void recvMsgClicked(MessageInfoPtr msgInfo);
    void recvMsgDeleted(MessageInfoPtr msgInfo);
    void recvListCleared();

    void sendMsgClicked(MessageInfoPtr msgInfo);
    void sendMsgDoubleClicked(MessageInfoPtr msgInfo);
    void sendMsgDeleted(MessageInfoPtr msgInfo);
    void sendListCleared();

signals:
    void sigAddRecvMsg(MessageInfoPtr msgInfo);
    void sigAddSendMsg(MessageInfoPtr msgInfo);
    void sigSendMsgUpdated();
    void sigSetRecvState(int state);
    void sigSetSendState(int state);
    void sigDisplayMsgDetailsWidget(QWidget* widget);
    void sigRecvMsgListSelectOnAddEnabled(bool enabled);
    void sigRecvMsgListClearSelection();
    void sigSendMsgListClearSelection();
    void sigDisplayMsg(MessageInfoPtr msgInfo);
    void sigClearDisplayedMsg();
    void sigNewSendMsgDialog(ProtocolPtr protocol);
    void sigUpdateSendMsgDialog(MessageInfoPtr msgInfo, ProtocolPtr protocol);
    void sigRecvListEmpty(bool empty);
    void sigSendListEmpty(bool empty);
    void sigRecvMsgSelected(bool selected);
    void sigSendMsgSelected(bool selected);
    void sigRecvDeleteSelectedMsg();
    void sigSendDeleteSelectedMsg();
    void sigRecvClear();
    void sigSendClear();

private:
    enum class SelectionType
    {
        None,
        Recv,
        Send
    };

    GuiAppMgr(QObject* parent = nullptr);
    void emitRecvStateUpdate();
    void emitSendStateUpdate();

private slots:
    void msgAdded(MessageInfoPtr msgInfo);
    void sendPendingAndWait();

private /*data*/:

    void msgClicked(MessageInfoPtr msgInfo, SelectionType selType);
    void displayMessage(MessageInfoPtr msgInfo);
    void displayMessageIfNotClicked(MessageInfoPtr msgInfo);
    void clearDisplayedMessage();

    RecvState m_recvState;
    bool m_recvListSelectOnAdd = true;
    unsigned m_recvListCount = 0;
    bool m_recvListContainsSent = true;

    SendState m_sendState;
    unsigned m_sendListCount = 0;

    SelectionType m_selType = SelectionType::None;
    MessageInfoPtr m_clickedMsg;
    MsgInfosList m_msgsToSend;
};

}  // namespace comms_champion


