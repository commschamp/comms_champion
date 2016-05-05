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

#include "EnumValues.h"

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

typedef demo::message::EnumValuesFields<EnumValues::Field> EnumValuesFields;

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(
        cc::property::field::ForField<EnumValuesFields::field1>()
            .name("field1")
            .add("Value1")
            .add("Value2")
            .add("Value3")
            .asMap());
    assert(
        cc::property::field::EnumValue(props.back())
            .values().size() == (int)EnumValuesFields::ValuesField1::NumOfValues);

    props.append(
        cc::property::field::ForField<EnumValuesFields::field2>()
            .name("field2")
            .add("Value1", (int)EnumValuesFields::ValuesField2::Value1)
            .add("Value2", (int)EnumValuesFields::ValuesField2::Value2)
            .add("Value3", (int)EnumValuesFields::ValuesField2::Value3)
            .add("Value4", (int)EnumValuesFields::ValuesField2::Value4)
            .asMap());
    props.append(
        cc::property::field::ForField<EnumValuesFields::field3>()
            .name("field3")
            .add("Value1", (int)EnumValuesFields::ValuesField3::Value1)
            .add("Value2", (int)EnumValuesFields::ValuesField3::Value2)
            .add("Value3", (int)EnumValuesFields::ValuesField3::Value3)
            .add("Value4", (int)EnumValuesFields::ValuesField3::Value4)
            .add("Value5", (int)EnumValuesFields::ValuesField3::Value5)
            .asMap());

    assert(props.size() == EnumValues::FieldIdx_numOfValues);
    return props;
}

}  // namespace

EnumValues::EnumValues() = default;
EnumValues::~EnumValues() = default;

EnumValues& EnumValues::operator=(const EnumValues&) = default;
EnumValues& EnumValues::operator=(EnumValues&&) = default;

const char* EnumValues::nameImpl() const
{
    static const char* Str = "EnumValues";
    return Str;
}

const QVariantList& EnumValues::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
