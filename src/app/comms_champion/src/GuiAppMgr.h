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
//    Q_PROPERTY(QString recvState READ recvState CONSTANT)
//    Q_PROPERTY(QString sendState READ sendState CONSTANT)

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
    static void qmlRegister();

    RecvState recvState() const;
    bool recvMsgListSelectOnAddEnabled();

    SendState sendState() const;

public slots:
    Q_INVOKABLE void configClicked();

    Q_INVOKABLE void recvStartClicked();
    Q_INVOKABLE void recvStopClicked();
    Q_INVOKABLE void recvSaveClicked();

    Q_INVOKABLE void sendStartClicked();
    Q_INVOKABLE void sendStartAllClicked();
    Q_INVOKABLE void sendStopClicked();
    Q_INVOKABLE void sendSaveClicked();

    void recvMsgClicked(ProtocolsInfoPtr msgInfo);

signals:
    void sigAddRecvMsg(ProtocolsInfoPtr msgInfo);
    void sigSetRecvState(int state);
    void sigSetSendState(int state);
    void sigDisplayMsgDetailsWidget(QWidget* widget);
    void sigRecvMsgListSelectOnAddEnabled(bool enabled);
    void sigRecvMsgListClearSelection();
    void sigDisplayMsg(ProtocolsInfoPtr msgInfo);

private:
    GuiAppMgr(QObject* parent = nullptr);
    void emitRecvStateUpdate();
    void emitSendStateUpdate();

private slots:
    void msgReceived(ProtocolsInfoPtr protocolsInfo);

private /*data*/:
    using MsgDisplayHandlerPtr = std::unique_ptr<MessageDisplayHandler>;

    void msgClicked(ProtocolsInfoPtr protocolsInfo);
    void displayMessage(ProtocolsInfoPtr protocolsInfo);
    void displayMessageIfNotClicked(ProtocolsInfoPtr protocolsInfo);

    RecvState m_recvState;
    bool m_recvListSelectOnAdd = true;

    SendState m_sendState;
    ProtocolsInfoPtr m_clickedMsg;
};

}  // namespace comms_champion


