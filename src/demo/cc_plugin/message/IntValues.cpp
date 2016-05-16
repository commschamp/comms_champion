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

#include "IntValues.h"

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

typedef demo::message::IntValuesFields<IntValues::Field> IntValuesFields;

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(cc::property::field::ForField<IntValuesFields::field1>().name("field1").asMap());
    props.append(cc::property::field::ForField<IntValuesFields::field2>().name("field2").asMap());
    props.append(cc::property::field::ForField<IntValuesFields::field2>().name("field3").asMap());
    props.append(cc::property::field::ForField<IntValuesFields::field2>().name("field4").asMap());

    assert(props.size() == IntValues::FieldIdx_numOfValues);
    return props;
}

}  // namespace

IntValues::IntValues() = default;
IntValues::~IntValues() = default;

IntValues& IntValues::operator=(const IntValues&) = default;
IntValues& IntValues::operator=(IntValues&&) = default;

const char* IntValues::nameImpl() const
{
    static const char* Str = "IntValues";
    return Str;
}

const QVariantList& IntValues::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
