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

#include "MsgDetailsWidget.h"

#include <cassert>

#include "comms_champion/DefaultMessageDisplayHandler.h"
#include "GuiAppMgr.h"

namespace comms_champion
{

MsgDetailsWidget::MsgDetailsWidget(QWidget* parent)
  : Base(parent),
    m_msgDisplayHandler(new DefaultMessageDisplayHandler())
{
    m_ui.setupUi(this);
}

void MsgDetailsWidget::displayMessage(ProtocolsInfoPtr protocolsInfo)
{
    assert(protocolsInfo);
    auto msgInfo = protocolsInfo->back();
    assert(msgInfo);
    auto msg = msgInfo->getAppMessage();
    assert(msg);
    auto msgWidget = m_msgDisplayHandler->createMsgWidget(*msg);
    msgWidget->setEditEnabled(false);
    m_ui.m_scrollArea->setWidget(msgWidget.release());
}

} /* namespace comms_champion */
