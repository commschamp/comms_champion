//
// Copyright 2015 - 2018 (C). Alex Robenko. All rights reserved.
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

#include "Strings.h"

namespace cc = comms_champion;

namespace demo
{

namespace cc_plugin
{

namespace message
{

namespace
{

using StringsFields = demo::message::StringsFields<>;

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(cc::property::field::ForField<StringsFields::field1>().name("field1").asMap());
    props.append(cc::property::field::ForField<StringsFields::field2>().name("field2").asMap());
    props.append(cc::property::field::ForField<StringsFields::field3>().name("field3").asMap());

    assert(props.size() == Strings::FieldIdx_numOfValues);
    return props;
}

}  // namespace

Strings::Strings() = default;
Strings::~Strings() noexcept = default;

Strings& Strings::operator=(const Strings&) = default;
Strings& Strings::operator=(Strings&&) = default;

const char* Strings::nameImpl() const
{
    static const char* Str = "Strings";
    return Str;
}

const QVariantList& Strings::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

