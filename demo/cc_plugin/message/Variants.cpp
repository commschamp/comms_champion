//
// Copyright 2017 - 2018 (C). Alex Robenko. All rights reserved.
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

#include "Variants.h"

namespace cc = comms_champion;

namespace demo
{

namespace cc_plugin
{

namespace message
{

namespace
{

using VariantsFields = demo::message::VariantsFields<>;

QVariantMap createProps_id()
{
    auto props =
        cc::property::field::EnumValue()
            .name("id")
            .readOnly()
            .add("elem1")
            .add("elem2")
            .add("elem3");
    assert(props.values().size() == (int)VariantsFields::VarId::NumOfValues);
    return props.asMap();
}

QVariantMap createProps_var1()
{
    using Field = VariantsFields::field1_var1;
    auto props =
        cc::property::field::ForField<Field>()
            .name("var1")
            .add(createProps_id())
            .add(cc::property::field::IntValue().name("value").asMap());
    assert(props.members().size() == Field::FieldIdx_numOfValues);
    return props.asMap();
}

QVariantMap createProps_var2()
{
    using Field = VariantsFields::field1_var2;
    auto props =
        cc::property::field::ForField<Field>()
            .name("var2")
            .add(createProps_id())
            .add(cc::property::field::IntValue().name("value").asMap());
    assert(props.members().size() == Field::FieldIdx_numOfValues);
    return props.asMap();
}

QVariantMap createProps_var3()
{
    using Field = VariantsFields::field1_var3;
    auto props =
        cc::property::field::ForField<Field>()
            .name("var3")
            .add(createProps_id())
            .add(cc::property::field::String().name("value").asMap());
    assert(props.members().size() == Field::FieldIdx_numOfValues);
    return props.asMap();
}

QVariantMap createProps_field1()
{
    using Field = VariantsFields::field1;
    auto props =
        cc::property::field::ForField<Field>()
            .name("field1")
            .serialisedHidden()
            .add(createProps_var1())
            .add(createProps_var2())
            .add(createProps_var3());
    assert(props.members().size() == Field::FieldIdx_numOfValues);
    return props.asMap();
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(createProps_field1());

    assert(props.size() == Variants::FieldIdx_numOfValues);
    return props;
}

}  // namespace

Variants::Variants() = default;
Variants::~Variants() noexcept = default;

Variants& Variants::operator=(const Variants&) = default;
Variants& Variants::operator=(Variants&&) = default;

const char* Variants::nameImpl() const
{
    static const char* Str = "Variants";
    return Str;
}

const QVariantList& Variants::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

