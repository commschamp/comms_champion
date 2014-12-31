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

    enum class RecvState {
        Idle,
        Running,
        NumOfStates
    };

    enum SendState {
        Idle,
        SendingSingle,
        SendingAll,
        NumOfStates
    };

    static GuiAppMgr* instance();

    RecvState recvState() const;
    bool recvMsgListSelectOnAddEnabled();

    SendState sendState() const;

public slots:
    void configClicked();

    void recvStartClicked();
    void recvStopClicked();
    void recvSaveClicked();

    void sendStartClicked();
    void sendStartAllClicked();
    void sendStopClicked();
    void sendSaveClicked();
    void sendAddClicked();
    void sendEditClicked();
    void sendDeleteClicked();

    void recvMsgClicked(MessageInfoPtr msgInfo);

signals:
    void sigAddRecvMsg(MessageInfoPtr msgInfo);
    void sigSetRecvState(int state);
    void sigSetSendState(int state);
    void sigDisplayMsgDetailsWidget(QWidget* widget);
    void sigRecvMsgListSelectOnAddEnabled(bool enabled);
    void sigRecvMsgListClearSelection();
    void sigDisplayMsg(MessageInfoPtr msgInfo);
    void sigNewSendMsgDialog(ProtocolPtr protocol);

private:
    GuiAppMgr(QObject* parent = nullptr);
    void emitRecvStateUpdate();
    void emitSendStateUpdate();

private slots:
    void msgReceived(MessageInfoPtr msgInfo);

private /*data*/:

    void msgClicked(MessageInfoPtr msgInfo);
    void displayMessage(MessageInfoPtr msgInfo);
    void displayMessageIfNotClicked(MessageInfoPtr msgInfo);

    RecvState m_recvState;
    bool m_recvListSelectOnAdd = true;

    SendState m_sendState;
    MessageInfoPtr m_clickedMsg;
};

}  // namespace comms_champion


