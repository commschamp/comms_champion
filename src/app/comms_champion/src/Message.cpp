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

#include <cassert>

#include <QtCore/QVariant>
#include <QtWidgets/QWidget>

#include "comms_champion/MessageDisplayHandler.h"
#include "comms_champion/MessageWidget.h"

namespace comms_champion
{

Message::~Message() = default;

const char* Message::name() const
{
    return nameImpl();
}

const QVariantList& Message::fieldsProperties() const
{
    return fieldsPropertiesImpl();
}

void Message::display(MessageDisplayHandler& handler)
{
    displayImpl(handler);
}

QString Message::idAsString() const
{
    return idAsStringImpl();
}

void Message::reset()
{
    resetImpl();
}

bool Message::assign(const Message& other)
{
    return assignImpl(other);
}

bool Message::isValid() const
{
    return isValidImpl();
}

Message::DataSeq Message::encodeData() const
{
    return encodeDataImpl();
}

bool Message::decodeData(const DataSeq& data)
{
    return decodeDataImpl(data);
}

}  // namespace comms_champion

