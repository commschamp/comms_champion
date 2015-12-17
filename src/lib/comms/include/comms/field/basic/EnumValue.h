//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include <type_traits>

#include "comms/ErrorStatus.h"
#include "comms/field/category.h"

#include "IntValue.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TFieldBase, typename T>
class EnumValue : public TFieldBase
{
    static_assert(std::is_enum<T>::value, "T must be enum");

    typedef typename std::underlying_type<T>::type UnderlyingType;

    typedef TFieldBase Base;

    typedef
        IntValue<
            Base,
            UnderlyingType
        > IntValueField;

    typedef typename IntValueField::ValueType IntValueType;
public:

    typedef comms::field::category::NumericValueField Category;

    typedef T ValueType;

    typedef typename IntValueField::ValueType SerialisedType;

    typedef typename IntValueField::ScalingRatio ScalingRatio;


    EnumValue() = default;

    explicit EnumValue(ValueType val)
      : value_(val)
    {
    }

    EnumValue(const EnumValue&) = default;
    EnumValue(EnumValue&&) = default;
    ~EnumValue() = default;

    EnumValue& operator=(const EnumValue&) = default;
    EnumValue& operator=(EnumValue&&) = default;

    const ValueType& value() const
    {
        return value_;
    }

    ValueType& value()
    {
        return value_;
    }

    static constexpr std::size_t length()
    {
        return IntValueField::length();
    }

    static constexpr std::size_t minLength()
    {
        return length();
    }

    static constexpr std::size_t maxLength()
    {
        return length();
    }

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return IntValueField::toSerialised(static_cast<IntValueType>(val));
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return static_cast<ValueType>(IntValueField::fromSerialised(val));
    }

    static constexpr bool valid()
    {
        return true;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        IntValueField intField;
        auto es = intField.read(iter, size);
        if (es == ErrorStatus::Success) {
            value_ = static_cast<decltype(value_)>(intField.value());
        }
        return es;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return IntValueField(static_cast<IntValueType>(value_)).write(iter, size);
    }

private:
    ValueType value_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


