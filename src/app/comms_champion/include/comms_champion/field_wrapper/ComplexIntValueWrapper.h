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

#include "comms/field/ComplexIntValue.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class ComplexIntValueWrapper : public NumericValueWrapper<int>
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

    void forceValuesRange(UnderlyingType min, UnderlyingType max)
    {
        forceValuesRangeImpl(min, max);
    }

protected:
    virtual UnderlyingType minValueImpl() const = 0;
    virtual UnderlyingType maxValueImpl() const = 0;
    virtual void forceValuesRangeImpl(UnderlyingType min, UnderlyingType max) = 0;
};

template <typename TField>
class ComplexIntValueWrapperT : public NumericValueWrapperT<ComplexIntValueWrapper, TField>
{
    using Base = NumericValueWrapperT<ComplexIntValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isComplexIntValue<Field>(), "Must be of ComplexIntValueField type");

    using ValueType = typename Field::ValueType;
    using UnderlyingType = typename Base::UnderlyingType;
    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(UnderlyingType)),
        "This wrapper cannot handle provided field.");

public:
    ComplexIntValueWrapperT(Field& field)
      : Base(field)
    {
    }

    ComplexIntValueWrapperT(const ComplexIntValueWrapperT&) = default;
    ComplexIntValueWrapperT(ComplexIntValueWrapperT&&) = default;
    virtual ~ComplexIntValueWrapperT() = default;

    ComplexIntValueWrapperT& operator=(const ComplexIntValueWrapperT&) = delete;

protected:
    virtual UnderlyingType minValueImpl() const override
    {
        return std::max(static_cast<UnderlyingType>(Base::field().minValue()), m_forcedMin);
    }

    virtual UnderlyingType maxValueImpl() const override
    {
        return std::min(static_cast<UnderlyingType>(Base::field().maxValue()), m_forcedMax);
    }

    virtual void forceValuesRangeImpl(UnderlyingType min, UnderlyingType max) override
    {
        m_forcedMin = min;
        m_forcedMax = max;
    }

private:
    UnderlyingType m_forcedMin = std::numeric_limits<UnderlyingType>::min();
    UnderlyingType m_forcedMax = std::numeric_limits<UnderlyingType>::max();
};

using ComplexIntValueWrapperPtr = std::unique_ptr<ComplexIntValueWrapper>;

template <typename TField>
ComplexIntValueWrapperPtr
makeComplexIntValueWrapper(TField& field)
{
    return
        ComplexIntValueWrapperPtr(
            new ComplexIntValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
