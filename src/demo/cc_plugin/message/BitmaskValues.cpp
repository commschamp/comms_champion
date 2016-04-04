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

#include "BitmaskValues.h"

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

typedef demo::message::BitmaskValuesFields<BitmaskValues::Field> BitmaskValuesFields;

QVariantMap createField1Properties()
{
    QVariantList bitNames;
    bitNames.append("bit0");
    bitNames.append("bit1");
    bitNames.append("bit2");
    bitNames.append("bit3");
    bitNames.append("bit4");
    assert(bitNames.size() == (int)BitmaskValuesFields::field1_NumOfValues);
    return cc::Property::createPropertiesMap("field1", std::move(bitNames));
}

QVariantMap createField2Properties()
{
    QVariantList bitNames;
    bitNames.append("bit0");
    bitNames.append(QVariant());
    bitNames.append(QVariant());
    bitNames.append("bit3");
    bitNames.append(QVariant());
    bitNames.append(QVariant());
    bitNames.append(QVariant());
    bitNames.append(QVariant());
    bitNames.append("bit8");
    bitNames.append("bit9");
    assert(bitNames.size() == (int)BitmaskValuesFields::field2_NumOfValues);
    return cc::Property::createPropertiesMap("field2", std::move(bitNames));
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(createField1Properties());
    props.append(createField2Properties());

    assert(props.size() == BitmaskValues::FieldIdx_numOfValues);
    return props;
}

}  // namespace

BitmaskValues::BitmaskValues() = default;
BitmaskValues::~BitmaskValues() = default;

BitmaskValues& BitmaskValues::operator=(const BitmaskValues&) = default;
BitmaskValues& BitmaskValues::operator=(BitmaskValues&&) = default;

const char* BitmaskValues::nameImpl() const
{
    static const char* Str = "BitmaskValues";
    return Str;
}

const QVariantList& BitmaskValues::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
