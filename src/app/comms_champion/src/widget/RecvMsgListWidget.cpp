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

#include "RecvMsgListWidget.h"

#include <cassert>
#include <chrono>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "RecvAreaToolBar.h"
#include "GuiAppMgr.h"
#include "GlobalConstants.h"

namespace comms_champion
{

RecvMsgListWidget::RecvMsgListWidget(QWidget* parentObj)
  : Base(getTitlePrefix(), new RecvAreaToolBar(), parentObj)
{
    auto* guiMgr = GuiAppMgr::instance();
    assert(guiMgr != nullptr);

    selectOnAdd(guiMgr->recvMsgListSelectOnAddEnabled());

    connect(
        guiMgr, SIGNAL(sigAddRecvMsg(MessageInfoPtr)),
        this, SLOT(addMessage(MessageInfoPtr)));
    connect(
        guiMgr, SIGNAL(sigRecvMsgListSelectOnAddEnabled(bool)),
        this, SLOT(selectOnAdd(bool)));
    connect(
        guiMgr, SIGNAL(sigRecvMsgListClearSelection()),
        this, SLOT(clearSelection()));
    connect(
        guiMgr, SIGNAL(sigRecvDeleteSelectedMsg()),
        this, SLOT(deleteCurrentMessage()));
    connect(
        guiMgr, SIGNAL(sigRecvClear(bool)),
        this, SLOT(clearList(bool)));
    connect(
        guiMgr, SIGNAL(sigRecvListTitleNeedsUpdate()),
        this, SLOT(titleNeedsUpdate()));

}

void RecvMsgListWidget::msgClickedImpl(MessageInfoPtr msgInfo, int idx)
{
    GuiAppMgr::instance()->recvMsgClicked(msgInfo, idx);
}

void RecvMsgListWidget::msgListClearedImpl(MsgInfosList&& msgInfosList)
{
    GuiAppMgr::instance()->deleteMessages(std::move(msgInfosList));
}

QString RecvMsgListWidget::msgPrefixImpl(const MessageInfo& msgInfo) const
{
    auto timestampVar =
        msgInfo.getExtraProperty(GlobalConstants::timestampPropertyName());

    if (!timestampVar.isValid()) {
        return QString();
    }

    typedef GuiAppMgr::Timestamp TimestampType;
    assert(timestampVar.canConvert<TimestampType>());

    auto timestamp = timestampVar.value<TimestampType>();
    auto timestampMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()).count();
    return QString("[%1]").arg(timestampMs, 1, 10, QChar('0'));
}


const QString& RecvMsgListWidget::msgTooltipImpl() const
{
    static const QString& Tooltip("Click to display");
    return Tooltip;
}

Qt::GlobalColor RecvMsgListWidget::getItemColourImpl(MsgType type, bool valid) const
{
    if (type == MsgType::Received) {
        return Base::getItemColourImpl(type, valid);
    }

    if (valid) {
        return Qt::darkGreen;
    }

    return Qt::darkRed;
}

QString RecvMsgListWidget::getTitleImpl() const
{
    return getTitlePrefix();
}

QString RecvMsgListWidget::getTitlePrefix()
{
    auto* guiAppMgr = GuiAppMgr::instance();
    assert(guiAppMgr != nullptr);
    if (guiAppMgr->recvListShowsReceived() && guiAppMgr->recvListShowsSent()) {
        static const QString Str("All Messages");
        return Str;
    }

    if (guiAppMgr->recvListShowsReceived()) {
        static const QString Str("Received Messages");
        return Str;
    }

    if (guiAppMgr->recvListShowsSent()) {
        static const QString Str("Sent Messages");
        return Str;
    }

    static const QString Str("No Messages");
    return Str;
}

} // namespace comms_champion
