//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "LeftPaneWidget.h"

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QLabel>
CC_ENABLE_WARNINGS()

#include "RecvMsgListWidget.h"
#include "SendMsgListWidget.h"

namespace comms_champion
{

LeftPaneWidget::LeftPaneWidget(QWidget* parentObj)
  : Base(parentObj)
{
    setOrientation(Qt::Vertical);
    addWidget(new RecvMsgListWidget());
    addWidget(new SendMsgListWidget());
}

}  // namespace comms_champion


