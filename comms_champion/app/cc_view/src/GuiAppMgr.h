//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
#include <QtWidgets/QAction>
CC_ENABLE_WARNINGS()

#include "comms_champion/Message.h"
#include "comms_champion/PluginMgr.h"
#include "comms_champion/MsgSendMgr.h"

#include "MsgMgrG.h"

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
    typedef std::shared_ptr<QAction> ActionPtr;
    typedef PluginMgr::ListOfPluginInfos ListOfPluginInfos;
    typedef Protocol::MessagesList MessagesList;
    enum class ActivityState
    {
        Clear,
        Inactive,
        Active,
    };

    static GuiAppMgr* instance();
    static GuiAppMgr& instanceRef();

    ~GuiAppMgr();

    bool startClean();
    bool startFromConfig(const QString& configName);
    bool startFromFile(const QString& filename);

    RecvState recvState() const;
    bool recvMsgListSelectOnAddEnabled();
    bool recvListEmpty() const;
    void recvLoadMsgsFromFile(const QString& filename);
    void recvSaveMsgsToFile(const QString& filename);
    bool recvListShowsReceived() const;
    bool recvListShowsSent() const;
    bool recvListShowsGarbage() const;
    unsigned recvListModeMask() const;

    SendState sendState() const;
    void sendAddNewMessage(MessagePtr msg);
    void sendUpdateMessage(MessagePtr msg);
    bool sendListEmpty() const;
    void sendLoadMsgsFromFile(bool clear, const QString& filename);
    void sendSaveMsgsToFile(const QString& filename);
    void sendUpdateList(const MessagesList& msgs);

    void deleteMessages(MessagesList&& msgs);
    void sendMessages(MessagesList&& msgs);

    static ActivityState getActivityState();
    bool applyNewPlugins(const ListOfPluginInfos& plugins);

public slots:
    void pluginsEditClicked();

    void recvStartClicked();
    void recvStopClicked();
    void recvLoadClicked();
    void recvSaveClicked();
    void recvDeleteClicked();
    void recvClearClicked();
    void recvShowRecvToggled(bool checked);
    void recvShowSentToggled(bool checked);
    void recvShowGarbageToggled(bool checked);

    void sendStartClicked();
    void sendStartAllClicked();
    void sendStopClicked();
    void sendLoadClicked();
    void sendSaveClicked();
    void sendAddClicked();
    void sendAddRawClicked();
    void sendEditClicked();
    void sendDeleteClicked();
    void sendClearClicked();
    void sendTopClicked();
    void sendUpClicked();
    void sendDownClicked();
    void sendBottomClicked();

    void recvMsgClicked(MessagePtr msg, int idx);

    void sendMsgClicked(MessagePtr msg, int idx);
    void sendMsgDoubleClicked(MessagePtr msg, int idx);
    void sendSelectedMsgMoved(int idx);

    void addMainToolbarAction(ActionPtr action);

    void connectSocketClicked();
    void disconnectSocketClicked();

signals:
    void sigAddRecvMsg(MessagePtr msg);
    void sigAddSendMsg(MessagePtr msg);
    void sigSendMsgUpdated(MessagePtr msg);
    void sigSetRecvState(int state);
    void sigSetSendState(int state);
    void sigDisplayMsgDetailsWidget(QWidget* widget);
    void sigRecvMsgListSelectOnAddEnabled(bool enabled);
    void sigRecvMsgListClearSelection();
    void sigSendMsgListClearSelection();
    void sigDisplayMsg(MessagePtr msg);
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
    void sigSendRawMsgDialog(ProtocolPtr protocol);
    void sigUpdateSendMsgDialog(MessagePtr msg, ProtocolPtr protocol);
    void sigLoadRecvMsgsDialog();
    void sigSaveRecvMsgsDialog();
    void sigLoadSendMsgsDialog(bool askForClear);
    void sigSaveSendMsgsDialog();
    void sigPluginsEditDialog();
    void sigActivityStateChanged(int value);
    void sigErrorReported(const QString& msg);
    void sigAddMainToolbarAction(ActionPtr action);
    void sigClearAllMainToolbarActions();
    void sigRecvSaveMsgs(const QString& filename);
    void sigSendLoadMsgs(bool clear, const QString& filename, ProtocolPtr protocol);
    void sigSendSaveMsgs(const QString& filename);
    void sigSocketConnected(bool connected);
    void sigSocketConnectEnabled(bool enabled);

private:
    enum class SelectionType
    {
        None,
        Recv,
        Send
    };

    GuiAppMgr(QObject* parentObj = nullptr);
    void emitRecvStateUpdate();
    void emitSendStateUpdate();

private slots:
    void msgAdded(MessagePtr msg);
    void errorReported(const QString& msg);
    void socketDisconnected();
    void pendingDisplayTimeout();

private /*data*/:

    void msgClicked(MessagePtr msg, SelectionType selType);
    void displayMessage(MessagePtr msg);
    void clearDisplayedMessage();
    void refreshRecvList();
    void addMsgToRecvList(MessagePtr msg);
    void clearRecvList(bool reportDeleted);
    bool canAddToRecvList(const Message& msg, MsgType type) const;
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
    MessagePtr m_clickedMsg;

    QTimer m_pendingDisplayTimer;
    MessagePtr m_pendingDisplayMsg;
    bool m_pendingDisplayWaitInProgress = false;

    MsgSendMgr m_sendMgr;
};

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::GuiAppMgr::ActionPtr);
