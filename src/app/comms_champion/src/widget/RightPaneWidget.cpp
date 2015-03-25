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

#include "RightPaneWidget.h"

#include <QtWidgets/QVBoxLayout>

#include "DefaultMessageDisplayWidget.h"
#include "GuiAppMgr.h"

namespace comms_champion
{

RightPaneWidget::RightPaneWidget(QWidget* parent)
  : Base(parent)
{
    auto* msgDisplayWidget = new DefaultMessageDisplayWidget();
    msgDisplayWidget->setEditEnabled(false);

    auto* guiAppMgr = GuiAppMgr::instance();
    connect(guiAppMgr, SIGNAL(sigDisplayMsg(MessageInfoPtr)),
            msgDisplayWidget, SLOT(displayMessage(MessageInfoPtr)));
    connect(guiAppMgr, SIGNAL(sigClearDisplayedMsg()),
            msgDisplayWidget, SLOT(clear()));
    auto* layout = new QVBoxLayout();
    layout->addWidget(msgDisplayWidget);
    setLayout(layout);
}

}  // namespace comms_champion


