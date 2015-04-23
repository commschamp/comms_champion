//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <cstdint>
#include <cassert>
#include <memory>

#include "comms/field/BasicEnumValue.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class BasicEnumValueWrapper : public NumericValueWrapper<long long int>
{
    using Base = NumericValueWrapper<long long int>;
public:
    using Base::NumericValueWrapper;
    using UnderlyingType = typename Base::UnderlyingType;
};

template <typename TField>
class BasicEnumValueWrapperT : public NumericValueWrapperT<BasicEnumValueWrapper, TField>
{
    using Base = NumericValueWrapperT<BasicEnumValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isBasicEnumValue<Field>(), "Must be of BasicEnumValueField type");

    using ValueType = typename Field::ValueType;
    using UnderlyingType = typename Base::UnderlyingType;
    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(UnderlyingType)),
        "This wrapper cannot handle provided field.");

public:
    BasicEnumValueWrapperT(Field& field)
      : Base(field)
    {
    }

    BasicEnumValueWrapperT(const BasicEnumValueWrapperT&) = default;
    BasicEnumValueWrapperT(BasicEnumValueWrapperT&&) = default;
    virtual ~BasicEnumValueWrapperT() = default;

    BasicEnumValueWrapperT& operator=(const BasicEnumValueWrapperT&) = delete;

};

using BasicEnumValueWrapperPtr = std::unique_ptr<BasicEnumValueWrapper>;

template <typename TField>
BasicEnumValueWrapperPtr
makeBasicEnumValueWrapper(TField& field)
{
    return
        BasicEnumValueWrapperPtr(
            new BasicEnumValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
