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

#include "FloatValues.h"

namespace cc = comms_champion;

namespace demo
{

namespace cc_plugin
{

namespace message
{

namespace
{

using FloatValuesFields = demo::message::FloatValuesFields<>;

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(
        cc::property::field::ForField<FloatValuesFields::field1>()
            .name("field1")
            .addSpecial("S1", 0.1)
            .addSpecial("S2", double(std::numeric_limits<float>::quiet_NaN()))
            .addSpecial("S3", double(std::numeric_limits<float>::infinity()))
            .addSpecial("S4", double(-std::numeric_limits<float>::infinity()))
            .asMap());
    props.append(
        cc::property::field::ForField<FloatValuesFields::field2>()
            .name("field2")
            .addSpecial("S1", 0.5)
            .addSpecial("S2", std::numeric_limits<double>::quiet_NaN())
            .addSpecial("S3", std::numeric_limits<double>::infinity())
            .addSpecial("S4", -std::numeric_limits<double>::infinity())
            .asMap());
    props.append(
        cc::property::field::ForField<FloatValuesFields::field3>()
            .name("field3")
            .scaledDecimals(2)
            .addSpecial("S1", 123)
            .addSpecial("S2", 22)
            .asMap());
    props.append(
        cc::property::field::ForField<FloatValuesFields::field4>()
            .name("field4")
            .scaledDecimals(11)
            .asMap());

    assert(props.size() == FloatValues::FieldIdx_numOfValues);
    return props;
}

}  // namespace

FloatValues::FloatValues() = default;
FloatValues::~FloatValues() noexcept = default;

FloatValues& FloatValues::operator=(const FloatValues&) = default;
FloatValues& FloatValues::operator=(FloatValues&&) = default;

const QVariantList& FloatValues::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

