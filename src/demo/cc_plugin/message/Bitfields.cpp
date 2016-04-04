//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

namespace comms_champion
{

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
    QVariantList bitNames;
    bitNames.append("bit0");
    bitNames.append("bit1");
    bitNames.append("bit2");
    assert(bitNames.size() == BitfieldsFields::field1_numOfValues);
    auto props = cc::Property::createPropertiesMap("field1_bitmask", std::move(bitNames));
    cc::Property::setSerialisedHidden(props);
    return props;
}

QVariantMap createField1EnumProperties()
{
    QVariantList enumValues;
    cc::Property::appendEnumValue(enumValues, "Value1");
    cc::Property::appendEnumValue(enumValues, "Value2");
    cc::Property::appendEnumValue(enumValues, "Value3");
    assert(enumValues.size() == (int)BitfieldsFields::Field1Enum::NumOfValues);
    auto props = cc::Property::createPropertiesMap("field1_enum", std::move(enumValues));
    cc::Property::setSerialisedHidden(props);
    return props;
}

QVariantMap createField1Int1Properties()
{
    auto props = cc::Property::createPropertiesMap("field1_int1");
    cc::Property::setSerialisedHidden(props);
    return props;
}

QVariantMap createField1Int2Properties()
{
    auto props = cc::Property::createPropertiesMap("field1_int2");
    cc::Property::setSerialisedHidden(props);
    return props;
}

QVariantMap createField1Properties()
{
    QVariantList members;
    members.append(createField1BitmaskProperties());
    members.append(createField1EnumProperties());
    members.append(createField1Int1Properties());
    members.append(createField1Int2Properties());
    assert(members.size() == BitfieldsFields::field1_numOfMembers);
    return cc::Property::createPropertiesMap("field1", std::move(members));
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

}  // namespace comms_champion
