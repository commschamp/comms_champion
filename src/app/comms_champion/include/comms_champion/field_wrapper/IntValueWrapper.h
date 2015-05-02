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

#include "comms/field/IntValue.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class IntValueWrapper : public NumericValueWrapper<int>
{
    typedef NumericValueWrapper<int> Base;
public:

    typedef Base::UnderlyingType UnderlyingType;

    using Base::NumericValueWrapper;

    UnderlyingType minValue() const
    {
        return minValueImpl();
    }

    UnderlyingType maxValue() const
    {
        return maxValueImpl();
    }

protected:
    virtual UnderlyingType minValueImpl() const = 0;
    virtual UnderlyingType maxValueImpl() const = 0;
};

template <typename TField>
class IntValueWrapperT : public NumericValueWrapperT<IntValueWrapper, TField>
{
    using Base = NumericValueWrapperT<IntValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isIntValue<Field>(), "Must be of IntValueField type");

    using ValueType = typename Field::ValueType;
    using UnderlyingType = typename Base::UnderlyingType;
    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(UnderlyingType)),
        "This wrapper cannot handle provided field.");

public:
    IntValueWrapperT(Field& field)
      : Base(field)
    {
    }

    IntValueWrapperT(const IntValueWrapperT&) = default;
    IntValueWrapperT(IntValueWrapperT&&) = default;
    virtual ~IntValueWrapperT() = default;

    IntValueWrapperT& operator=(const IntValueWrapperT&) = delete;

protected:
    virtual UnderlyingType minValueImpl() const override
    {
        return std::numeric_limits<typename Field::ValueType>::min();
    }

    virtual UnderlyingType maxValueImpl() const override
    {
        return std::numeric_limits<typename Field::ValueType>::max();
    }

};

using IntValueWrapperPtr = std::unique_ptr<IntValueWrapper>;

template <typename TField>
IntValueWrapperPtr
makeIntValueWrapper(TField& field)
{
    return
        IntValueWrapperPtr(
            new IntValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
