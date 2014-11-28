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


#include "comms_champion/Message.h"

#include <QtCore/QVariant>
#include <QtWidgets/QWidget>
#include <QtQml/QtQml>

#include "GlobalConstants.h"

namespace comms_champion
{

Message::~Message() = default;

const char* Message::name() const
{
    return nameImpl();
}

void Message::updateFieldProperties(QWidget& fieldWidget, uint idx) const
{
    updateFieldPropertiesImpl(fieldWidget, idx);
}

void Message::display(MessageDisplayHandler& handler)
{
    displayImpl(handler);
}

void Message::qmlRegister()
{
    qmlRegisterUncreatableType<Message>(
        "cc.protocol.Message", 1, 0, "Message", "Message is an Abstract class");
}

void Message::setNameProperty(QWidget& widget, const QString& value)
{
    widget.setProperty(GlobalConstants::namePropertyName(), QVariant(value));
}

}  // namespace comms_champion




