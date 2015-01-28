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

#include <QtWidgets/QVBoxLayout>

#include "SendAreaToolBar.h"
#include "GuiAppMgr.h"
#include "GlobalConstants.h"

namespace comms_champion
{

SendMsgListWidget::SendMsgListWidget(QWidget* parent)
  : Base("Messages to Send", new SendAreaToolBar(), parent)
{
    selectOnAdd(true);

    auto* guiMgr = GuiAppMgr::instance();
    assert(guiMgr != nullptr);
    connect(guiMgr, SIGNAL(sigAddSendMsg(MessageInfoPtr)),
            this, SLOT(addMessage(MessageInfoPtr)));
    connect(guiMgr, SIGNAL(sigSendMsgUpdated()),
            this, SLOT(updateCurrentMessage()));
    connect(guiMgr, SIGNAL(sigSendDeleteSelectedMsg()),
            this, SLOT(deleteCurrentMessage()));
    connect(guiMgr, SIGNAL(sigSendMsgListClearSelection()),
            this, SLOT(clearSelection()));
    connect(guiMgr, SIGNAL(sigSendClear()),
            this, SLOT(clear()));
    connect(guiMgr, SIGNAL(sigSetSendState(int)),
            this, SLOT(stateChanged(int)));
}

void SendMsgListWidget::msgClickedImpl(MessageInfoPtr msgInfo)
{
    GuiAppMgr::instance()->sendMsgClicked(std::move(msgInfo));
}

void SendMsgListWidget::msgDoubleClickedImpl(MessageInfoPtr msgInfo)
{
    GuiAppMgr::instance()->sendMsgDoubleClicked(std::move(msgInfo));
}

void SendMsgListWidget::msgDeletedImpl(MessageInfoPtr msgInfo)
{
    GuiAppMgr::instance()->sendMsgDeleted(std::move(msgInfo));
}

void SendMsgListWidget::listClearedImpl()
{
    GuiAppMgr::instance()->sendListCleared();
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
    typedef GuiAppMgr::SendState State;
    auto castedState = static_cast<State>(state);
    assert(castedState < State::NumOfStates);
    if (static_cast<State>(state) == State::Idle) {
        return;
    }

    if (static_cast<State>(state) == State::SendingSingle) {
        auto msgInfo = currentMsg();
        assert(msgInfo);
        MsgInfosList allMsgsList;
        allMsgsList.push_back(std::move(msgInfo));
        GuiAppMgr::instance()->sendMessages(std::move(allMsgsList));
        return;
    }

    assert(static_cast<State>(state) == State::SendingAll);
    auto allMsgsList = allMsgs();
    assert(!allMsgsList.empty());
    GuiAppMgr::instance()->sendMessages(std::move(allMsgsList));
}

} // namespace comms_champion
