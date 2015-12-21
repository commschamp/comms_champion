//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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


#include "comms_champion/MessageWidget.h"

namespace comms_champion
{

MessageWidget::MessageWidget(QWidget* parentObj)
  : Base(parentObj)
{
}

void MessageWidget::refresh()
{
    refreshImpl();
}

void MessageWidget::setEditEnabled(bool enabled)
{
    setEditEnabledImpl(enabled);
}

void MessageWidget::enableEdit()
{
    setEditEnabled(true);
}

void MessageWidget::disableEdit()
{
    setEditEnabled(false);
}

void MessageWidget::emitMsgUpdated()
{
    emit sigMsgUpdated();
}

}  // namespace comms_champion

