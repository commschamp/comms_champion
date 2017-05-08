//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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

typedef demo::message::VariantsFields<Variants::Field> VariantsFields;

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(cc::property::field::IntValue().name("field1").asMap());

    assert(props.size() == Variants::FieldIdx_numOfValues);
    return props;
}

}  // namespace

Variants::Variants() = default;
Variants::~Variants() = default;

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

