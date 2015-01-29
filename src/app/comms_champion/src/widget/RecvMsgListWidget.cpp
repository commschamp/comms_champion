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

#include <QtWidgets/QVBoxLayout>

#include "RecvAreaToolBar.h"
#include "GuiAppMgr.h"
#include "GlobalConstants.h"

namespace comms_champion
{

RecvMsgListWidget::RecvMsgListWidget(QWidget* parent)
  : Base("Received Messages", new RecvAreaToolBar(), parent)
{
    auto* guiMgr = GuiAppMgr::instance();
    assert(guiMgr != nullptr);

    selectOnAdd(guiMgr->recvMsgListSelectOnAddEnabled());

    connect(guiMgr, SIGNAL(sigAddRecvMsg(MessageInfoPtr)),
            this, SLOT(addMessage(MessageInfoPtr)));
    connect(guiMgr, SIGNAL(sigRecvMsgListSelectOnAddEnabled(bool)),
            this, SLOT(selectOnAdd(bool)));
    connect(guiMgr, SIGNAL(sigRecvMsgListClearSelection()),
            this, SLOT(clearSelection()));
    connect(guiMgr, SIGNAL(sigRecvDeleteSelectedMsg()),
            this, SLOT(deleteCurrentMessage()));
    connect(guiMgr, SIGNAL(sigRecvClear()),
            this, SLOT(clear()));
}

void RecvMsgListWidget::msgClickedImpl(MessageInfoPtr msgInfo)
{
   GuiAppMgr::instance()->recvMsgClicked(msgInfo);
}

void RecvMsgListWidget::msgDeletedImpl(MessageInfoPtr msgInfo)
{
    GuiAppMgr::instance()->recvMsgDeleted(std::move(msgInfo));
}

void RecvMsgListWidget::listClearedImpl()
{
    GuiAppMgr::instance()->recvListCleared();
}

QString RecvMsgListWidget::msgPrefixImpl(const MessageInfo& msgInfo) const
{
    typedef long long unsigned TimestampType;

    auto timestampVar =
        msgInfo.getExtraProperty(GlobalConstants::timestampPropertyName());
    if ((!timestampVar.isValid()) || (!timestampVar.canConvert<TimestampType>())) {
        auto timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        return QString("[{%1}]").arg(timestamp, 1, 10, QChar('0'));
    }


    auto timestamp = timestampVar.value<TimestampType>();
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

} // namespace comms_champion
