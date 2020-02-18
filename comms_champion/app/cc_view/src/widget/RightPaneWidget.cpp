//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "RightPaneWidget.h"

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "DefaultMessageDisplayWidget.h"
#include "GuiAppMgr.h"

namespace comms_champion
{

RightPaneWidget::RightPaneWidget(QWidget* parentObj)
  : Base(parentObj)
{
    auto* msgDisplayWidget = new DefaultMessageDisplayWidget();
    msgDisplayWidget->setEditEnabled(false);

    auto* guiAppMgr = GuiAppMgr::instance();
    connect(guiAppMgr, SIGNAL(sigDisplayMsg(MessagePtr)),
            msgDisplayWidget, SLOT(displayMessage(MessagePtr)));
    connect(guiAppMgr, SIGNAL(sigClearDisplayedMsg()),
            msgDisplayWidget, SLOT(clear()));
    auto* paneLayout = new QVBoxLayout();
    paneLayout->addWidget(msgDisplayWidget);
    setLayout(paneLayout);
}

}  // namespace comms_champion


