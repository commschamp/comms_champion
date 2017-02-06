//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include <cassert>

#include "Bitfields.h"

namespace cc = comms_champion;

namespace demo
{

namespace cc_plugin
{

namespace message
{

namespace
{

typedef demo::message::BitfieldsFields<Bitfields::Field> BitfieldsFields;

QVariantMap createField1BitmaskProperties()
{
    cc::property::field::BitmaskValue props;
    props.name("field1_bitmask")
         .serialisedHidden()
         .add("bit0")
         .add("bit1")
         .add("bit2");

    assert(props.bits().size() == BitfieldsFields::field1_numOfValues);
    return props.asMap();
}

QVariantMap createField1EnumProperties()
{
    cc::property::field::EnumValue props;
    props.name("field1_enum")
         .serialisedHidden()
         .add("Value1")
         .add("Value2")
         .add("Value3");

    assert(props.values().size() == (int)BitfieldsFields::Field1Enum::NumOfValues);
    return props.asMap();
}

QVariantMap createField1Int1Properties()
{
    return
        cc::property::field::IntValue()
            .name("field1_int1")
            .serialisedHidden()
            .asMap();
}

QVariantMap createField1Int2Properties()
{
    return
        cc::property::field::IntValue()
            .name("field1_int2")
            .serialisedHidden()
            .asMap();
}

QVariantMap createField1Properties()
{
    cc::property::field::Bitfield props;
    props.name("field1")
         .add(createField1BitmaskProperties())
         .add(createField1EnumProperties())
         .add(createField1Int1Properties())
         .add(createField1Int2Properties());

    assert(props.members().size() == BitfieldsFields::field1::FieldIdx_numOfValues);
    return props.asMap();
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(createField1Properties());

    assert(props.size() == Bitfields::FieldIdx_numOfValues);
    return props;
}

}  // namespace

Bitfields::Bitfields() = default;
Bitfields::~Bitfields() = default;

Bitfields& Bitfields::operator=(const Bitfields&) = default;
Bitfields& Bitfields::operator=(Bitfields&&) = default;

const char* Bitfields::nameImpl() const
{
    static const char* Str = "Bitfields";
    return Str;
}

const QVariantList& Bitfields::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

