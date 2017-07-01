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

#include "RecvMsgListWidget.h"

#include <cassert>
#include <chrono>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"
#include "RecvAreaToolBar.h"
#include "GuiAppMgr.h"
#include "MsgFileMgrG.h"

namespace comms_champion
{

RecvMsgListWidget::RecvMsgListWidget(QWidget* parentObj)
  : Base(getTitlePrefix(), new RecvAreaToolBar(), parentObj)
{
    auto* guiMgr = GuiAppMgr::instance();
    assert(guiMgr != nullptr);

    selectOnAdd(guiMgr->recvMsgListSelectOnAddEnabled());

    connect(
        guiMgr, SIGNAL(sigAddRecvMsg(MessagePtr)),
        this, SLOT(addMessage(MessagePtr)));
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
    connect(
        guiMgr, SIGNAL(sigRecvSaveMsgs(const QString&)),
        this, SLOT(saveMessages(const QString&)));

}

void RecvMsgListWidget::msgClickedImpl(MessagePtr msg, int idx)
{
    GuiAppMgr::instance()->recvMsgClicked(msg, idx);
}

void RecvMsgListWidget::msgListClearedImpl(MessagesList&& msgs)
{
    GuiAppMgr::instance()->deleteMessages(std::move(msgs));
}

QString RecvMsgListWidget::msgPrefixImpl(const Message& msg) const
{
    auto timestamp = property::message::Timestamp().getFrom(msg);
    if (timestamp == 0U) {
        return QString();
    }

    return QString("[%1]").arg(timestamp, 1, 10, QChar('0'));
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

void RecvMsgListWidget::saveMessagesImpl(const QString& filename)
{
    MsgFileMgrG::instanceRef().save(MsgFileMgr::Type::Recv, filename, allMsgs());
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
