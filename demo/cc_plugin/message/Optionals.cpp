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

#include "Optionals.h"

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

typedef demo::message::OptionalsFields<Optionals::Field> OptionalsFields;

QVariantMap createField1Properties()
{
    cc::property::field::ForField<OptionalsFields::field1> props;
    props.name("field1")
         .add("enable_field2")
         .add("enable_field3");

    assert(props.bits().size() == OptionalsFields::field1_numOfBits);
    return props.asMap();
}

QVariantMap createField2Properties()
{
    static const char* Name = "field2";
    return
        cc::property::field::ForField<OptionalsFields::field2>()
            .name(Name)
            .field(
                cc::property::field::ForField<OptionalsFields::field2::Field>()
                    .name(Name)
                    .asMap())
            .uncheckable()
            .asMap();
}

QVariantMap createField3Properties()
{
    static const char* Name = "field3";
    return
        cc::property::field::ForField<OptionalsFields::field3>()
            .name(Name)
            .field(
                cc::property::field::ForField<OptionalsFields::field3::Field>()
                    .name(Name)
                    .asMap())
            .uncheckable()
            .asMap();
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(createField1Properties());
    props.append(createField2Properties());
    props.append(createField3Properties());

    assert(props.size() == Optionals::FieldIdx_numOfValues);
    return props;
}

}  // namespace

Optionals::Optionals() = default;
Optionals::~Optionals() = default;

Optionals& Optionals::operator=(const Optionals&) = default;
Optionals& Optionals::operator=(Optionals&&) = default;

const char* Optionals::nameImpl() const
{
    static const char* Str = "Optionals";
    return Str;
}

const QVariantList& Optionals::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
