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

class LongIntValueWrapper : public NumericValueWrapper<long long int>
{
    typedef NumericValueWrapper<long long int> Base;
public:

    typedef typename Base::UnderlyingType UnderlyingType;

    using Base::NumericValueWrapper;

    virtual ~LongIntValueWrapper() {}

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
class LongIntValueWrapperT : public NumericValueWrapperT<LongIntValueWrapper, TField>
{
    using Base = NumericValueWrapperT<LongIntValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isIntValue<Field>(), "Must be of IntValueField type");

public:

    typedef typename Base::UnderlyingType UnderlyingType;

    LongIntValueWrapperT(Field& field)
      : Base(field)
    {
    }

    LongIntValueWrapperT(const LongIntValueWrapperT&) = default;
    LongIntValueWrapperT(LongIntValueWrapperT&&) = default;
    virtual ~LongIntValueWrapperT() = default;

    LongIntValueWrapperT& operator=(const LongIntValueWrapperT&) = delete;

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

using LongIntValueWrapperPtr = std::unique_ptr<LongIntValueWrapper>;

template <typename TField>
LongIntValueWrapperPtr
makeLongIntValueWrapper(TField& field)
{
    return
        LongIntValueWrapperPtr(
            new LongIntValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
