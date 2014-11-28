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
#include <type_traits>

#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

template <typename TUnderlyingType>
class NumericValueWrapper : public FieldWrapper
{
public:
    typedef TUnderlyingType UnderlyingType;

    virtual ~NumericValueWrapper() {}

    UnderlyingType value() const
    {
        return valueImpl();
    }

    void setValue(UnderlyingType value)
    {
        setValueImpl(value);
    }

    UnderlyingType serialisedValue() const
    {
        return serialisedValueImpl();
    }

    void setSerialisedValue(UnderlyingType value)
    {
        setSerialisedValueImpl(value);
    }

    UnderlyingType minValue() const
    {
        return minValueImpl();
    }

    UnderlyingType maxValue() const
    {
        return maxValueImpl();
    }

private:
    virtual UnderlyingType valueImpl() const = 0;
    virtual UnderlyingType serialisedValueImpl() const = 0;
    virtual void setValueImpl(UnderlyingType value) = 0;
    virtual void setSerialisedValueImpl(UnderlyingType value) = 0;
    virtual UnderlyingType minValueImpl() const = 0;
    virtual UnderlyingType maxValueImpl() const = 0;
};

template <typename TBase, typename TField>
class NumericValueWrapperT : public FieldWrapperT<TBase, TField>
{
    using Base = FieldWrapperT<TBase, TField>;

public:
    using UnderlyingType = typename Base::UnderlyingType;

protected:
    using Field = TField;
    using ValueType = typename Field::ValueType;

    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(UnderlyingType)),
        "This wrapper cannot handle provided field.");

public:
    NumericValueWrapperT(Field& field)
      : Base(field)
    {
        static_assert(std::is_base_of<NumericValueWrapper<UnderlyingType>, NumericValueWrapperT<TBase, TField> >::value,
            "Must inherit from NumericValueWrapper");
    }

    NumericValueWrapperT(const NumericValueWrapperT&) = default;
    NumericValueWrapperT(NumericValueWrapperT&&) = default;
    virtual ~NumericValueWrapperT() = default;

    NumericValueWrapperT& operator=(const NumericValueWrapperT&) = delete;

protected:

    virtual UnderlyingType valueImpl() const override
    {
        return static_cast<UnderlyingType>(Base::field().getValue());
    }

    virtual UnderlyingType serialisedValueImpl() const override
    {
        return static_cast<UnderlyingType>(Base::field().getSerialisedValue());
    }

    virtual void setValueImpl(UnderlyingType value) override
    {
        setValueImplInternal(value, UpdateTag());
    }

    virtual void setSerialisedValueImpl(UnderlyingType value) override
    {
        setSerialisedValueImplInternal(value, UpdateTag());
    }

    virtual UnderlyingType minValueImpl() const override
    {
        return static_cast<UnderlyingType>(std::numeric_limits<ValueType>::min());
    }

    virtual UnderlyingType maxValueImpl() const override
    {
        return static_cast<UnderlyingType>(std::numeric_limits<ValueType>::max());
    }

private:
    struct Writable {};
    struct ReadOnly {};

    using UpdateTag =
        typename std::conditional<
            std::is_const<TField>::value,
            ReadOnly,
            Writable
        >::type;

    void setValueImplInternal(UnderlyingType value, Writable)
    {
        Base::field().setValue(static_cast<ValueType>(value));
    }

    void setValueImplInternal(UnderlyingType value, ReadOnly)
    {
        static_cast<void>(value);
        assert(!"Attempt to update readonly field");
    }

    void setSerialisedValueImplInternal(UnderlyingType value, Writable)
    {
        Base::field().setSerialisedValue(static_cast<ValueType>(value));
    }

    void setSerialisedValueImplInternal(UnderlyingType value, ReadOnly)
    {
        static_cast<void>(value);
        assert(!"Attempt to update readonly field");
    }

};

}  // namespace field_wrapper

}  // namespace comms_champion
