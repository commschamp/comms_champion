//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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
#include <limits>
#include <algorithm>
#include <iterator>

#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

template <typename TUnderlyingType>
class NumericValueWrapper : public FieldWrapper
{
    static_assert(std::is_integral<TUnderlyingType>::value || std::is_floating_point<TUnderlyingType>::value,
        "Underlying type is expected to be integral or floating point.");
    typedef FieldWrapper Base;
public:
    typedef TUnderlyingType UnderlyingType;
    typedef Base::SerialisedSeq SerialisedSeq;

    NumericValueWrapper() {}

    virtual ~NumericValueWrapper() noexcept = default;

    UnderlyingType getValue() const
    {
        return getValueImpl();
    }

    void setValue(UnderlyingType value)
    {
        setValueImpl(value);
    }

    std::size_t minLength() const
    {
        return minLengthImpl();
    }

    std::size_t maxLength() const
    {
        return maxLengthImpl();
    }

    int minWidth() const
    {
        return static_cast<int>(minLength()) * 2;
    }

    int maxWidth() const
    {
        return static_cast<int>(maxLength()) * 2;
    }

private:
    virtual UnderlyingType getValueImpl() const = 0;
    virtual void setValueImpl(UnderlyingType value) = 0;
    virtual std::size_t minLengthImpl() const = 0;
    virtual std::size_t maxLengthImpl() const = 0;
};

template <typename TBase, typename TField>
class NumericValueWrapperT : public FieldWrapperT<TBase, TField>
{
    using Base = FieldWrapperT<TBase, TField>;

public:
    using UnderlyingType = typename Base::UnderlyingType;
    using SerialisedSeq = typename Base::SerialisedSeq;

protected:
    using Field = TField;
    using ValueType = typename Field::ValueType;

    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
    static_assert(
        std::is_signed<ValueType>::value == std::is_signed<UnderlyingType>::value ||
            (sizeof(ValueType) < sizeof(UnderlyingType)),
        "This wrapper cannot handle provided field.");

public:
    explicit NumericValueWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
        static_assert(std::is_base_of<NumericValueWrapper<UnderlyingType>, NumericValueWrapperT<TBase, TField> >::value,
            "Must inherit from NumericValueWrapper");
    }

    NumericValueWrapperT(const NumericValueWrapperT&) = default;
    NumericValueWrapperT(NumericValueWrapperT&&) = default;
    virtual ~NumericValueWrapperT() noexcept = default;

    NumericValueWrapperT& operator=(const NumericValueWrapperT&) = delete;

protected:

    virtual UnderlyingType getValueImpl() const override
    {
        return static_cast<UnderlyingType>(Base::field().value());
    }

    virtual void setValueImpl(UnderlyingType value) override
    {
        Base::field().value() = static_cast<ValueType>(value);
    }

    virtual std::size_t minLengthImpl() const override
    {
        return Base::field().minLength();
    }

    virtual std::size_t maxLengthImpl() const override
    {
        return Base::field().maxLength();
    }

};

}  // namespace field_wrapper

}  // namespace comms_champion
