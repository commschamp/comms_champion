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
#include "comms_champion/PluginControlInterface.h"

#include "MsgMgr.h"
#include "PluginMgr.h"

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

    enum RecvListMode : unsigned {
        RecvListMode_ShowReceived = 1U << 0,
        RecvListMode_ShowSent = 1U << 1,
        RecvListMode_ShowGarbage = 1U << 2
    };

    typedef MsgMgr::MsgType MsgType;
    typedef MsgMgr::Timestamp Timestamp;
    typedef PluginMgr::PluginsState ActivityState;
    typedef PluginControlInterface::ActionPtr ActionPtr;

    static GuiAppMgr* instance();

    ~GuiAppMgr();

    RecvState recvState() const;
    bool recvMsgListSelectOnAddEnabled();
    bool recvListEmpty() const;
    bool recvListShowsReceived() const;
    bool recvListShowsSent() const;
    bool recvListShowsGarbage() const;
    unsigned recvListModeMask() const;

    SendState sendState() const;
    void sendAddNewMessage(MessageInfoPtr msgInfo);
    void sendUpdateMessage(MessageInfoPtr msgInfo);
    bool sendListEmpty() const;

    void deleteMessages(MsgInfosList&& msgs);
    void sendMessages(MsgInfosList&& msgs);

    static ActivityState getActivityState();

public slots:
    void pluginsEditClicked();

    void recvStartClicked();
    void recvStopClicked();
    void recvSaveClicked();
    void recvDeleteClicked();
    void recvClearClicked();
    void recvShowRecvToggled(bool checked);
    void recvShowSentToggled(bool checked);
    void recvShowGarbageToggled(bool checked);

    void sendStartClicked();
    void sendStartAllClicked();
    void sendStopClicked();
    void sendSaveClicked();
    void sendAddClicked();
    void sendEditClicked();
    void sendDeleteClicked();
    void sendClearClicked();
    void sendTopClicked();
    void sendUpClicked();
    void sendDownClicked();
    void sendBottomClicked();

    void recvMsgClicked(MessageInfoPtr msgInfo, int idx);

    void sendMsgClicked(MessageInfoPtr msgInfo, int idx);
    void sendMsgDoubleClicked(MessageInfoPtr msgInfo, int idx);
    void sendSelectedMsgMoved(int idx);

    void addMainToolbarAction(ActionPtr action);
    void removeMainToolbarAction(ActionPtr action);


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
    void sigRecvMsgSelected(int index);
    void sigSendMsgSelected(int index);
    void sigRecvDeleteSelectedMsg();
    void sigSendDeleteSelectedMsg();
    void sigRecvClear(bool reportDeleted);
    void sigSendClear();
    void sigRecvListCountReport(unsigned count);
    void sigSendListCountReport(unsigned count);
    void sigSendMoveSelectedTop();
    void sigSendMoveSelectedUp();
    void sigSendMoveSelectedDown();
    void sigSendMoveSelectedBottom();
    void sigRecvListTitleNeedsUpdate();
    void sigNewSendMsgDialog(ProtocolPtr protocol);
    void sigUpdateSendMsgDialog(MessageInfoPtr msgInfo, ProtocolPtr protocol);
    void sigPluginsEditDialog();
    void sigActivityStateChanged(int value);
    void sigErrorReported(const QString& msg);
    void sigAddMainToolbarAction(ActionPtr action);
    void sigRemoveMainToolbarAction(ActionPtr action);


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
    void activeStateChanged(int state);
    void errorReported(const QString& msg);

private /*data*/:

    void msgClicked(MessageInfoPtr msgInfo, SelectionType selType);
    void displayMessage(MessageInfoPtr msgInfo);
    void displayMessageIfNotClicked(MessageInfoPtr msgInfo);
    void clearDisplayedMessage();
    void refreshRecvList();
    void addMsgToRecvList(MessageInfoPtr msgInfo);
    void clearRecvList(bool reportDeleted);
    bool canAddToRecvList(const MessageInfo& msgInfo, MsgType type) const;
    void decRecvListCount();
    void decSendListCount();
    void emitRecvNotSelected();
    void emitSendNotSelected();
    void updateRecvListMode(RecvListMode mode, bool checked);

    RecvState m_recvState;
    bool m_recvListSelectOnAdd = true;
    unsigned m_recvListCount = 0;
    unsigned m_recvListMode =
        RecvListMode_ShowReceived |
        RecvListMode_ShowSent |
        RecvListMode_ShowGarbage;

    SendState m_sendState;
    unsigned m_sendListCount = 0;

    SelectionType m_selType = SelectionType::None;
    MessageInfoPtr m_clickedMsg;
    MsgInfosList m_msgsToSend;
};

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::GuiAppMgr::ActionPtr);
