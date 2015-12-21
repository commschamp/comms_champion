//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
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

class IntValueWrapper : public NumericValueWrapper<long long int>
{
    typedef NumericValueWrapper<long long int> Base;
public:

    typedef typename Base::UnderlyingType UnderlyingType;

    using Base::NumericValueWrapper;

    virtual ~IntValueWrapper() {}

    UnderlyingType minValue() const
    {
        return minValueImpl();
    }

    UnderlyingType maxValue() const
    {
        return maxValueImpl();
    }

    double getScaled() const
    {
        return getScaledImpl();
    }

    void setScaled(double value)
    {
        setScaledImpl(value);
    }

    double scaleValue(UnderlyingType value) const
    {
        return scaleValueImpl(value);
    }

    bool isShortInt() const
    {
        return isShortIntImpl();
    }

protected:
    virtual UnderlyingType minValueImpl() const = 0;
    virtual UnderlyingType maxValueImpl() const = 0;
    virtual double getScaledImpl() const = 0;
    virtual void setScaledImpl(double value) = 0;
    virtual double scaleValueImpl(UnderlyingType value) const = 0;
    virtual bool isShortIntImpl() const = 0;
};

template <typename TField>
class IntValueWrapperT : public NumericValueWrapperT<IntValueWrapper, TField>
{
    using Base = NumericValueWrapperT<IntValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isIntValue<Field>(), "Must be of IntValueField type");

public:

    typedef typename Base::UnderlyingType UnderlyingType;

    explicit IntValueWrapperT(Field& fieldRef)
      : Base(fieldRef)
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

    virtual double getScaledImpl() const override
    {
        return Base::field().template scaleAs<double>();
    }

    virtual void setScaledImpl(double value) override
    {
        Base::field().setScaled(value);
    }

    virtual double scaleValueImpl(UnderlyingType value) const
    {
        Field fieldTmp(value);
        return fieldTmp.template scaleAs<double>();
    }

    virtual bool isShortIntImpl() const override
    {
        typedef typename Field::ValueType ValueType;
        if (sizeof(ValueType) < sizeof(int)) {
            return true;
        }

        if (sizeof(int) < sizeof(ValueType)) {
            return false;
        }

        return std::is_signed<ValueType>::value;
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
