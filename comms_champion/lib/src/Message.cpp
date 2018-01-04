//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariant>
#include <QtWidgets/QWidget>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

Message::~Message() noexcept = default;

const char* Message::name() const
{
    return nameImpl();
}

const QVariantList& Message::extraTransportFieldsProperties() const
{
    return extraTransportFieldsPropertiesImpl();
}

const QVariantList& Message::fieldsProperties() const
{
    return fieldsPropertiesImpl();
}

void Message::dispatch(MessageHandler& handler)
{
    dispatchImpl(handler);
}

bool Message::refreshMsg()
{
    return refreshMsgImpl();
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

const QVariantList& Message::extraTransportFieldsPropertiesImpl() const
{
    static const QVariantList Props;
    return Props;
}

const QVariantList& Message::fieldsPropertiesImpl() const
{
    static const QVariantList Props;
    return Props;
}

}  // namespace comms_champion

