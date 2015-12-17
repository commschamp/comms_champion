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

#include "SendMsgListWidget.h"

#include <cassert>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "SendAreaToolBar.h"
#include "GuiAppMgr.h"
#include "GlobalConstants.h"
#include "MsgFileMgr.h"

namespace comms_champion
{

SendMsgListWidget::SendMsgListWidget(QWidget* parentObj)
  : Base("Messages to Send", new SendAreaToolBar(), parentObj),
    m_state(GuiAppMgr::instanceRef().sendState())
{
    selectOnAdd(true);

    auto* guiMgr = GuiAppMgr::instance();
    assert(guiMgr != nullptr);
    connect(
        guiMgr, SIGNAL(sigAddSendMsg(MessageInfoPtr)),
        this, SLOT(addMessage(MessageInfoPtr)));
    connect(
        guiMgr, SIGNAL(sigSendMsgUpdated()),
        this, SLOT(updateCurrentMessage()));
    connect(
        guiMgr, SIGNAL(sigSendDeleteSelectedMsg()),
        this, SLOT(deleteCurrentMessage()));
    connect(
        guiMgr, SIGNAL(sigSendMsgListClearSelection()),
        this, SLOT(clearSelection()));
    connect(
        guiMgr, SIGNAL(sigSendClear()),
        this, SLOT(clearList()));
    connect(
        guiMgr, SIGNAL(sigSetSendState(int)),
        this, SLOT(stateChanged(int)));
    connect(
        guiMgr, SIGNAL(sigSendMoveSelectedTop()),
        this, SLOT(moveSelectedTop()));
    connect(
        guiMgr, SIGNAL(sigSendMoveSelectedUp()),
        this, SLOT(moveSelectedUp()));
    connect(
        guiMgr, SIGNAL(sigSendMoveSelectedDown()),
        this, SLOT(moveSelectedDown()));
    connect(
        guiMgr, SIGNAL(sigSendMoveSelectedBottom()),
        this, SLOT(moveSelectedBottom()));
    connect(
        guiMgr, SIGNAL(sigSendLoadMsgs(bool, const QString&, ProtocolPtr)),
        this, SLOT(loadMessages(bool, const QString&, ProtocolPtr)));
    connect(
        guiMgr, SIGNAL(sigSendSaveMsgs(const QString&)),
        this, SLOT(saveMessages(const QString&)));
    connect(
        guiMgr, SIGNAL(sigSendMsgSelected(int)),
        this, SLOT(selectMsg(int)));
}

void SendMsgListWidget::msgClickedImpl(MessageInfoPtr msgInfo, int idx)
{
    GuiAppMgr::instance()->sendMsgClicked(std::move(msgInfo), idx);
}

void SendMsgListWidget::msgDoubleClickedImpl(MessageInfoPtr msgInfo, int idx)
{
    if (m_state != State::Idle) {
        return;
    }
    GuiAppMgr::instance()->sendMsgDoubleClicked(std::move(msgInfo), idx);
}

QString SendMsgListWidget::msgPrefixImpl(const MessageInfo& msgInfo) const
{
    QString str;
    do {
        auto delayVar = msgInfo.getExtraProperty(GlobalConstants::msgDelayPropertyName());
        auto repeatDurVar = msgInfo.getExtraProperty(GlobalConstants::msgRepeatDurationPropertyName());
        auto repeatCountVar = msgInfo.getExtraProperty(GlobalConstants::msgRepeatCountPropertyName());

        if ((!delayVar.isValid()) ||
            (!repeatDurVar.isValid()) ||
            (!repeatCountVar.isValid())) {
            assert(!"The message info doesn't contain expected properties");
            break;
        }

        assert(delayVar.canConvert<long long unsigned>());
        assert(repeatDurVar.canConvert<long long unsigned>());
        assert(repeatCountVar.canConvert<long long unsigned>());

        auto delay = delayVar.value<int>();
        auto repeatDur = repeatDurVar.value<int>();
        auto repeatCount = repeatCountVar.value<int>();

        str =
            QString("(%1:%2:%3)").
                arg(delay, 1, 10, QChar('0')).
                arg(repeatDur, 1, 10, QChar('0')).
                arg(repeatCount, 1, 10, QChar('0'));

    } while (false);
    return str;
}

const QString& SendMsgListWidget::msgTooltipImpl() const
{
    static const QString& Tooltip("Click to display, double click to edit");
    return Tooltip;
}

void SendMsgListWidget::stateChangedImpl(int state)
{
    m_state = static_cast<State>(state);
    assert(m_state < State::NumOfStates);
    if (m_state == State::Idle) {
        return;
    }

    if (m_state == State::SendingSingle) {
        auto msgInfo = currentMsg();
        assert(msgInfo);
        MsgInfosList allMsgsList;
        allMsgsList.push_back(std::move(msgInfo));
        GuiAppMgr::instanceRef().sendMessages(std::move(allMsgsList));
        return;
    }

    assert(m_state == State::SendingAll);
    auto allMsgsList = allMsgs();
    assert(!allMsgsList.empty());
    GuiAppMgr::instanceRef().sendMessages(std::move(allMsgsList));
}

void SendMsgListWidget::msgMovedImpl(int idx)
{
    GuiAppMgr::instanceRef().sendSelectedMsgMoved(idx);
}

void SendMsgListWidget::loadMessagesImpl(const QString& filename, Protocol& protocol)
{
    static_cast<void>(filename);
    auto msgs = MsgFileMgr::instanceRef().load(MsgFileMgr::Type::Send, filename, protocol);
    for (auto& m : msgs) {
        addMessage(m);
    }
    GuiAppMgr::instanceRef().sendUpdateList(allMsgs());
}

void SendMsgListWidget::saveMessagesImpl(const QString& filename)
{
    MsgFileMgr::instanceRef().save(MsgFileMgr::Type::Send, filename, allMsgs());
}

} // namespace comms_champion
