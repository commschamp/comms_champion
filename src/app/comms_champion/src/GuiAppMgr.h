//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
CC_ENABLE_WARNINGS()

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
    typedef PluginControlInterface::ActionPtr ActionPtr;
    typedef PluginMgr::ListOfPluginInfos ListOfPluginInfos;
    enum class ActivityState
    {
        Clear,
        Inactive,
        Active,
    };

    static GuiAppMgr* instance();
    static GuiAppMgr& instanceRef();

    ~GuiAppMgr();

    void start();
    void clean();

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
    void sendLoadMsgsFromFile(bool clear, const QString& filename);
    void sendSaveMsgsToFile(const QString& filename);
    void sendUpdateList(const MsgInfosList& msgs);

    void deleteMessages(MsgInfosList&& msgs);
    void sendMessages(MsgInfosList&& msgs);

    static ActivityState getActivityState();
    bool applyNewPlugins(const ListOfPluginInfos& plugins);

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
    void sendLoadClicked();
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
    void sigLoadSendMsgsDialog(bool askForClear);
    void sigSaveSendMsgsDialog();
    void sigPluginsEditDialog();
    void sigActivityStateChanged(int value);
    void sigErrorReported(const QString& msg);
    void sigAddMainToolbarAction(ActionPtr action);
    void sigRemoveMainToolbarAction(ActionPtr action);
    void sigSendLoadMsgs(bool clear, const QString& filename, ProtocolPtr protocol);
    void sigSendSaveMsgs(const QString& filename);


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
    void msgAdded(MessageInfoPtr msgInfo);
    void sendPendingAndWait();
    void errorReported(const QString& msg);
    void pendingDisplayTimeout();

private /*data*/:

    void msgClicked(MessageInfoPtr msgInfo, SelectionType selType);
    void displayMessage(MessageInfoPtr msgInfo);
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

    QTimer m_pendingDisplayTimer;
    MessageInfoPtr m_pendingDisplayMsg;
    bool m_pendingDisplayWaitInProgress = false;
};

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::GuiAppMgr::ActionPtr);
