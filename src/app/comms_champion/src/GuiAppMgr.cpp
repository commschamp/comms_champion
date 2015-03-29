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

#include <QtCore/QTimer>

#include "comms_champion/DefaultMessageDisplayHandler.h"
#include "GlobalConstants.h"

#include <iostream>

namespace comms_champion
{

namespace
{

GuiAppMgr::MsgType getMsgType(const MessageInfo& msgInfo)
{
    auto msgTypeVar =
        msgInfo.getExtraProperty(GlobalConstants::msgTypePropertyName());
    assert(msgTypeVar.isValid());
    assert(msgTypeVar.canConvert<int>());
    return static_cast<GuiAppMgr::MsgType>(msgTypeVar.value<int>());
}

}  // namespace

GuiAppMgr* GuiAppMgr::instance()
{
    return &(instanceRef());
}

GuiAppMgr& GuiAppMgr::instanceRef()
{
    static GuiAppMgr mgr;
    return mgr;
}

GuiAppMgr::~GuiAppMgr() = default;

void GuiAppMgr::pluginsEditClicked()
{
    emit sigPluginsEditDialog();
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

void GuiAppMgr::recvDeleteClicked()
{
    assert(!recvListEmpty());
    assert(m_selType == SelectionType::Recv);
    assert(m_clickedMsg);

    MsgMgr::instanceRef().deleteMsg(m_clickedMsg);

    clearDisplayedMessage();
    emit sigRecvDeleteSelectedMsg();
    decRecvListCount();
}

void GuiAppMgr::recvClearClicked()
{
    assert(0 < m_recvListCount);
    clearRecvList(true);
}

void GuiAppMgr::recvShowRecvToggled(bool checked)
{
    updateRecvListMode(RecvListMode_ShowReceived, checked);
}

void GuiAppMgr::recvShowSentToggled(bool checked)
{
    updateRecvListMode(RecvListMode_ShowSent, checked);
}

void GuiAppMgr::recvShowGarbageToggled(bool checked)
{
    updateRecvListMode(RecvListMode_ShowGarbage, checked);
}

void GuiAppMgr::sendStartClicked()
{
    m_sendState = SendState::SendingSingle;
    emitSendStateUpdate();
}

void GuiAppMgr::sendStartAllClicked()
{
    m_sendState = SendState::SendingAll;
    emitSendStateUpdate();
}

void GuiAppMgr::sendStopClicked()
{
    m_sendState = SendState::Idle;
    m_msgsToSend.clear();
    emitSendStateUpdate();
}

void GuiAppMgr::sendLoadClicked()
{
    emit sigLoadSendMsgsDialog(0 < m_sendListCount);
}

void GuiAppMgr::sendSaveClicked()
{
    emit sigSaveSendMsgsDialog();
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
    assert(!sendListEmpty());
    assert(m_selType == SelectionType::Send);
    assert(m_clickedMsg);

    clearDisplayedMessage();
    emit sigSendDeleteSelectedMsg();

    decSendListCount();
}

void GuiAppMgr::sendClearClicked()
{
    emit sigSendClear();
    assert(0 < m_sendListCount);
    bool wasSelected = (m_selType == SelectionType::Send);
    assert((!wasSelected) || (m_clickedMsg));

    m_sendListCount = 0;

    if (wasSelected) {
        clearDisplayedMessage();
        emitSendNotSelected();
    }

    emit sigSendListCountReport(m_sendListCount);
}

void GuiAppMgr::sendTopClicked()
{
    emit sigSendMoveSelectedTop();
}

void GuiAppMgr::sendUpClicked()
{
    emit sigSendMoveSelectedUp();
}

void GuiAppMgr::sendDownClicked()
{
    emit sigSendMoveSelectedDown();
}

void GuiAppMgr::sendBottomClicked()
{
    emit sigSendMoveSelectedBottom();
}

void GuiAppMgr::recvMsgClicked(MessageInfoPtr msgInfo, int idx)
{
    emit sigSendMsgListClearSelection();
    emitSendNotSelected();

    msgClicked(msgInfo, SelectionType::Recv);
    if (!m_clickedMsg) {
        emit sigRecvMsgListClearSelection();
        emitRecvNotSelected();
    }
    else {
        emit sigRecvMsgSelected(idx);
    }
}

void GuiAppMgr::sendMsgClicked(MessageInfoPtr msgInfo, int idx)
{
    emit sigRecvMsgListClearSelection();
    emitRecvNotSelected();

    msgClicked(msgInfo, SelectionType::Send);
    if (!m_clickedMsg) {
        emit sigSendMsgListClearSelection();
        emitSendNotSelected();
    }
    else {
        emit sigSendMsgSelected(idx);
    }
}

void GuiAppMgr::sendMsgDoubleClicked(MessageInfoPtr msgInfo, int idx)
{
    // Equivalent to selection + edit
    assert(msgInfo);
    if (msgInfo != m_clickedMsg) {
        sendMsgClicked(msgInfo, idx);
    }
    assert(m_clickedMsg == msgInfo);
    sendEditClicked();
}

void GuiAppMgr::sendSelectedMsgMoved(int idx)
{
    assert(0 <= idx);
    assert(m_clickedMsg);
    assert(m_selType == SelectionType::Send);
    emit sigSendMsgSelected(idx);
}

void GuiAppMgr::addMainToolbarAction(ActionPtr action)
{
    emit sigAddMainToolbarAction(std::move(action));
}

void GuiAppMgr::removeMainToolbarAction(ActionPtr action)
{
    emit sigRemoveMainToolbarAction(std::move(action));
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

bool GuiAppMgr::recvListShowsReceived() const
{
    return (m_recvListMode & RecvListMode_ShowReceived) != 0;
}

bool GuiAppMgr::recvListShowsSent() const
{
    return (m_recvListMode & RecvListMode_ShowSent) != 0;
}

bool GuiAppMgr::recvListShowsGarbage() const
{
    return (m_recvListMode & RecvListMode_ShowGarbage) != 0;
}

unsigned GuiAppMgr::recvListModeMask() const
{
    return m_recvListMode;
}

GuiAppMgr::SendState GuiAppMgr::sendState() const
{
    return m_sendState;
}

void GuiAppMgr::sendAddNewMessage(MessageInfoPtr msgInfo)
{
    ++m_sendListCount;
    emit sigAddSendMsg(msgInfo);
    emit sigSendListCountReport(m_sendListCount);
    sendMsgClicked(msgInfo, m_sendListCount - 1);
    assert(m_selType == SelectionType::Send);
    assert(m_clickedMsg);
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

void GuiAppMgr::sendLoadMsgsFromFile(bool clear, const QString& filename)
{
    emit sigSendLoadMsgs(clear, filename, MsgMgr::instanceRef().getProtocol());
}

void GuiAppMgr::sendSaveMsgsToFile(const QString& filename)
{
    emit sigSendSaveMsgs(filename);
}

void GuiAppMgr::sendUpdateList(const MsgInfosList& msgs)
{
    decltype(m_clickedMsg) clickedMsg;
    if (m_selType == SelectionType::Send) {
        assert(m_clickedMsg);
        assert(0 < m_sendListCount);
        clickedMsg = m_clickedMsg;
        sendMsgClicked(m_clickedMsg, -1);
        assert(!m_clickedMsg);
    }

    int clickedIdx = 0;
    for (auto& msgInfo : msgs) {
        if (msgInfo == clickedMsg) {
            break;
        }
        ++clickedIdx;
    }

    m_sendListCount = msgs.size();
    emit sigSendListCountReport(m_sendListCount);
    if ((clickedMsg) && (static_cast<std::size_t>(clickedIdx) < msgs.size())) {
        sendMsgClicked(clickedMsg, clickedIdx);
    }
}

void GuiAppMgr::deleteMessages(MsgInfosList&& msgs)
{
    auto& msgMgr = MsgMgr::instanceRef();
    for (auto& msgInfo : msgs) {
        assert(msgInfo);
        assert(msgInfo != m_clickedMsg);

        msgMgr.deleteMsg(std::move(msgInfo));
    }
}

void GuiAppMgr::sendMessages(MsgInfosList&& msgs)
{
    assert(m_msgsToSend.empty());
    for (auto& msgInfo : msgs) {
        m_msgsToSend.push_back(makeMessageInfoCopy(*msgInfo));
    }

    assert(!m_msgsToSend.empty());
    sendPendingAndWait();
}

GuiAppMgr::ActivityState GuiAppMgr::getActivityState()
{
    return PluginMgr::instanceRef().getState();
}

GuiAppMgr::GuiAppMgr(QObject* parent)
  : Base(parent),
    m_recvState(RecvState::Idle),
    m_sendState(SendState::Idle)
{
    connect(
        MsgMgr::instance(), SIGNAL(sigMsgAdded(MessageInfoPtr)),
        this, SLOT(msgAdded(MessageInfoPtr)));
    connect(
        MsgMgr::instance(), SIGNAL(sigErrorReported(const QString&)),
        this, SLOT(errorReported(const QString&)));
    connect(
        PluginMgr::instance(), SIGNAL(sigStateChanged(int)),
        this, SLOT(activeStateChanged(int)));
}

void GuiAppMgr::emitRecvStateUpdate()
{
    emit sigSetRecvState(static_cast<int>(m_recvState));
}

void GuiAppMgr::emitSendStateUpdate()
{
    emit sigSetSendState(static_cast<int>(m_sendState));
}

void GuiAppMgr::msgAdded(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    auto type = getMsgType(*msgInfo);
    assert((type == MsgType::Received) || (type == MsgType::Sent));

#ifndef NDEBUG

    static const char* const RecvPrefix = "<-- ";
    static const char* const SentPrefix = "--> ";

    const char* prefix = RecvPrefix;
    if (type == MsgType::Sent) {
        prefix = SentPrefix;
    }

    auto msg = msgInfo->getAppMessage();
    if (msg) {
        std::cout << prefix << msg->name() << std::endl;
    }
    else if (msgInfo->getTransportMessage()) {
        std:: cout << prefix << "???" << std::endl;
    }
    else if (msgInfo->getRawDataMessage()) {
        std:: cout << prefix << "-#-" << std::endl;
    }
    else {
        assert(!"Should not happen");
    }
#endif

    if (canAddToRecvList(*msgInfo, type)) {
        addMsgToRecvList(msgInfo);
        displayMessageIfNotClicked(msgInfo);
    }
}

void GuiAppMgr::sendPendingAndWait()
{
    auto retrieveIntPropertyFunc =
        [](const MessageInfo& mInfo, const char* property) -> int
        {
            auto delayVar =
                mInfo.getExtraProperty(property);
            assert(delayVar.isValid());
            assert(delayVar.canConvert<int>());
            return delayVar.value<int>();
        };

    auto retrieveDelayFunc =
        [&](const MessageInfo& mInfo) -> int
        {
            return retrieveIntPropertyFunc(
                mInfo, GlobalConstants::msgDelayPropertyName());
        };

    auto iter = m_msgsToSend.begin();
    for (; iter != m_msgsToSend.end(); ++iter) {
        auto& msgInfo = *iter;
        assert(msgInfo);
        auto delay = retrieveDelayFunc(*msgInfo);
        if (delay != 0) {
            break;
        }
    }

    MsgInfosList nextMsgsToSend;
    nextMsgsToSend.splice(
        nextMsgsToSend.end(), m_msgsToSend, m_msgsToSend.begin(), iter);

    MsgMgr::instanceRef().sendMsgs(nextMsgsToSend);

    for (auto& msgToSend : nextMsgsToSend) {
        auto repeatMs =
            retrieveIntPropertyFunc(
                *msgToSend,
                GlobalConstants::msgRepeatDurationPropertyName());

        auto repeatCount =
            retrieveIntPropertyFunc(
                *msgToSend,
                GlobalConstants::msgRepeatCountPropertyName());

        bool reinsert =
            (0 < repeatMs) &&
            ((repeatCount == 0) || (1 < repeatCount));

        if (reinsert) {
            auto newDelay = repeatMs;
            auto iter =
                std::find_if(
                    m_msgsToSend.begin(), m_msgsToSend.end(),
                    [&newDelay, &retrieveDelayFunc](MessageInfoPtr mInfo) mutable -> bool
                    {
                        assert(mInfo);
                        auto mDelay = retrieveDelayFunc(*mInfo);
                        if (newDelay < mDelay) {
                            return true;
                        }
                        newDelay -= mDelay;
                        return false;
                    });

            if (iter != m_msgsToSend.end()) {
                auto& msgToUpdate = *iter;
                assert(msgToUpdate);
                auto mDelay = retrieveDelayFunc(*msgToUpdate);
                msgToUpdate->setExtraProperty(
                    GlobalConstants::msgDelayPropertyName(),
                    QVariant::fromValue(mDelay - newDelay));
            }

            msgToSend->setExtraProperty(
                GlobalConstants::msgDelayPropertyName(),
                QVariant::fromValue(newDelay));

            if (repeatCount != 0) {
                msgToSend->setExtraProperty(
                    GlobalConstants::msgRepeatCountPropertyName(),
                    QVariant::fromValue(repeatCount - 1));
            }

            m_msgsToSend.insert(iter, std::move(msgToSend));
        }
    }

    if (!m_msgsToSend.empty()) {
        auto& msgInfo = m_msgsToSend.front();
        assert(msgInfo);
        auto delay = retrieveDelayFunc(*msgInfo);
        assert(0 < delay);
        msgInfo->setExtraProperty(
            GlobalConstants::msgDelayPropertyName(), QVariant::fromValue(0));
        QTimer::singleShot(delay, this, SLOT(sendPendingAndWait()));
    }
    else {
        sendStopClicked();
    }
}

void GuiAppMgr::activeStateChanged(int state)
{
    auto castedState = static_cast<ActivityState>(state);
    auto& msgMgr = MsgMgr::instanceRef();
    if (castedState == ActivityState::Active) {
        msgMgr.start();
    }
    else if (castedState == ActivityState::Clear) {
        msgMgr.clear();
    }
    else {
        assert(castedState == ActivityState::Inactive);
        msgMgr.stop();
    }
    emit sigActivityStateChanged(state);
}

void GuiAppMgr::errorReported(const QString& msg)
{
    emit sigErrorReported(msg + tr("\nThe tool may not work properly!"));
}

void GuiAppMgr::msgClicked(MessageInfoPtr msgInfo, SelectionType selType)
{
    assert(msgInfo);
    if (m_clickedMsg == msgInfo) {

        assert(selType == m_selType);
        clearDisplayedMessage();
        emit sigRecvMsgListSelectOnAddEnabled(true);
        return;
    }

    m_selType = selType;
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
    m_selType = SelectionType::None;
    m_clickedMsg.reset();
    emit sigClearDisplayedMsg();
}

void GuiAppMgr::refreshRecvList()
{
    auto clickedMsg = m_clickedMsg;
    if (m_selType == SelectionType::Recv) {
        assert(m_clickedMsg);
        assert(0 < m_recvListCount);
        recvMsgClicked(m_clickedMsg, m_recvListCount - 1);
        assert(!m_clickedMsg);
    }
    else if (m_selType != SelectionType::Send) {
        emit sigClearDisplayedMsg();
    }

    clearRecvList(false);

    auto& allMsgs = MsgMgr::instanceRef().getAllMsgs();
    for (auto& msgInfo : allMsgs) {
        auto type = getMsgType(*msgInfo);

        if (canAddToRecvList(*msgInfo, type)) {
            addMsgToRecvList(msgInfo);
            if (msgInfo == clickedMsg) {
                assert(0 < m_recvListCount);
                recvMsgClicked(msgInfo, m_recvListCount - 1);
            }
        }
    }

    if (!m_clickedMsg) {
        emit sigRecvMsgListClearSelection();
    }
}

void GuiAppMgr::addMsgToRecvList(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    ++m_recvListCount;
    emit sigAddRecvMsg(msgInfo);
    emit sigRecvListCountReport(m_recvListCount);
}

void GuiAppMgr::clearRecvList(bool reportDeleted)
{
    bool wasSelected = (m_selType == SelectionType::Recv);
    bool sendSelected = (m_selType == SelectionType::Send);
    assert((!wasSelected) || (m_clickedMsg));
    assert((!sendSelected) || (m_clickedMsg));

    m_recvListCount = 0;

    if (!sendSelected) {
        clearDisplayedMessage();
    }

    if (wasSelected) {
        emit sigRecvMsgListSelectOnAddEnabled(true);
        emitRecvNotSelected();
    }

    emit sigRecvListCountReport(m_recvListCount);
    emit sigRecvClear(reportDeleted);
}

bool GuiAppMgr::canAddToRecvList(
    const MessageInfo& msgInfo,
    MsgType type) const
{
    assert((type == MsgType::Received) || (type == MsgType::Sent));

    bool typeBasedResult =
        ((type == MsgType::Received) && recvListShowsReceived()) ||
        ((type == MsgType::Sent) && recvListShowsSent());

    if (!typeBasedResult) {
        return false;
    }

    if (type == MsgType::Sent) {
        assert(msgInfo.getAppMessage()); // Cannot be garbage
        return true;
    }

    if (msgInfo.getAppMessage()) {
        return true; // Show valid message
    }

    // Garbage
    return recvListShowsGarbage();
}

void GuiAppMgr::decRecvListCount()
{
    --m_recvListCount;
    if (recvListEmpty()) {
        emitRecvNotSelected();
    }
    emit sigRecvListCountReport(m_recvListCount);
}

void GuiAppMgr::decSendListCount()
{
    --m_sendListCount;
    if (sendListEmpty()) {
        emitSendNotSelected();
    }
    emit sigSendListCountReport(m_sendListCount);
}

void GuiAppMgr::emitRecvNotSelected()
{
    emit sigRecvMsgSelected(-1);
}

void GuiAppMgr::emitSendNotSelected()
{
    emit sigSendMsgSelected(-1);
}

void GuiAppMgr::updateRecvListMode(RecvListMode mode, bool checked)
{
    auto mask =
        static_cast<decltype(m_recvListMode)>(mode);
    if (checked) {
        m_recvListMode |= mask;
    }
    else {
        m_recvListMode &= (~mask);
    }

    if (mode != RecvListMode_ShowGarbage) {
        emit sigRecvListTitleNeedsUpdate();
    }
    refreshRecvList();
}

}  // namespace comms_champion

