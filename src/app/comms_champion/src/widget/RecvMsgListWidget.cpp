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

#include <QtWidgets/QVBoxLayout>

#include "RecvAreaToolBar.h"
#include "GuiAppMgr.h"

namespace comms_champion
{

RecvMsgListWidget::RecvMsgListWidget(QWidget* parent)
  : Base("Received Messages", new RecvAreaToolBar(), parent)
{
    auto* guiMgr = GuiAppMgr::instance();
    assert(guiMgr != nullptr);

    selectOnAdd(guiMgr->recvMsgListSelectOnAddEnabled());

    connect(guiMgr, SIGNAL(sigAddRecvMsg(Message*)),
            this, SLOT(addMessage(Message*)));
    connect(guiMgr, SIGNAL(sigRecvMsgListSelectOnAddEnabled(bool)),
            this, SLOT(selectOnAdd(bool)));
    connect(guiMgr, SIGNAL(sigRecvMsgListClearSelection()),
            this, SLOT(clearSelection()));
}

void RecvMsgListWidget::msgClickedImpl(Message* msg)
{
   GuiAppMgr::instance()->recvMsgClicked(msg);
}

} // namespace comms_champion
