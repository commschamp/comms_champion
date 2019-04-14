//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

class CC_API UnsignedLongValueWrapper : public NumericValueWrapper<long long unsigned>
{
    typedef NumericValueWrapper<long long unsigned> Base;
public:

    typedef Base::UnderlyingType UnderlyingType;
    typedef std::unique_ptr<UnsignedLongValueWrapper> Ptr;

    UnsignedLongValueWrapper();
    virtual ~UnsignedLongValueWrapper() noexcept;

    UnderlyingType minValue() const;

    UnderlyingType maxValue() const;

    double getScaled() const;

    void setScaled(double value);

    double scaleValue(UnderlyingType value) const;

    bool isSigned() const;

    std::size_t valueTypeSize() const;

    Ptr clone();

protected:
    virtual UnderlyingType minValueImpl() const = 0;
    virtual UnderlyingType maxValueImpl() const = 0;
    virtual double getScaledImpl() const = 0;
    virtual void setScaledImpl(double value) = 0;
    virtual double scaleValueImpl(UnderlyingType value) const = 0;
    virtual bool isSignedImpl() const = 0;
    virtual std::size_t valueTypeSizeImpl() const = 0;
    virtual Ptr cloneImpl() = 0;

    void dispatchImpl(FieldWrapperHandler& handler);
};

template <typename TField>
class UnsignedLongValueWrapperT : public NumericValueWrapperT<UnsignedLongValueWrapper, TField>
{
    using Base = NumericValueWrapperT<UnsignedLongValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isIntValue<Field>(), "Must be of IntValueField type");

public:

    typedef typename Base::UnderlyingType UnderlyingType;
    typedef typename Base::Ptr Ptr;

    explicit UnsignedLongValueWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    UnsignedLongValueWrapperT(const UnsignedLongValueWrapperT&) = default;
    UnsignedLongValueWrapperT(UnsignedLongValueWrapperT&&) = default;
    virtual ~UnsignedLongValueWrapperT() noexcept = default;

    UnsignedLongValueWrapperT& operator=(const UnsignedLongValueWrapperT&) = delete;

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

    virtual double scaleValueImpl(UnderlyingType value) const override
    {
        Field fieldTmp;
        fieldTmp.value() = value;
        return fieldTmp.template scaleAs<double>();
    }

    virtual bool isSignedImpl() const override
    {
        return std::is_signed<typename Field::ValueType>::value;
    }

    virtual std::size_t valueTypeSizeImpl() const override
    {
        return sizeof(typename Field::ValueType);
    }

    virtual Ptr cloneImpl() override
    {
        return Ptr(new UnsignedLongValueWrapperT<TField>(Base::field()));
    }
};

using UnsignedLongValueWrapperPtr = UnsignedLongValueWrapper::Ptr;

template <typename TField>
UnsignedLongValueWrapperPtr
makeUnsignedLongValueWrapper(TField& field)
{
    return
        UnsignedLongValueWrapperPtr(
            new UnsignedLongValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
