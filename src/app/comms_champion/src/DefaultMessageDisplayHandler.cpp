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

#include "comms_champion/DefaultMessageDisplayHandler.h"

#include <QtCore/QVariant>

#include "GlobalConstants.h"

#include "widget/BasicIntValueFieldWidget.h"

namespace comms_champion
{

DefaultMessageDisplayHandler::MsgWidgetPtr
DefaultMessageDisplayHandler::createMsgWidgetImpl(
    Message& msg)
{
    m_widget.reset(new DefaultMessageWidget(msg));
    msg.display(*this);
    return MsgWidgetPtr(m_widget.release());
}

DefaultMessageDisplayHandler::FieldWidgetPtr
DefaultMessageDisplayHandler::createBasicIntValueFieldWidget(
    field_wrapper::BasicIntValueWrapperPtr&& fieldWrapper)
{
    return
        FieldWidgetPtr(
            new BasicIntValueFieldWidget(std::move(fieldWrapper)));
}

void DefaultMessageDisplayHandler::updateFieldIdxProperty(
    FieldWidget& field,
    std::size_t idx)
{
    field.setProperty(
        GlobalConstants::indexPropertyName(),
        static_cast<uint>(idx));
}

}  // namespace comms_champion


