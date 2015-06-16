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

    typedef ValueType ParamValueType;

    typedef typename IntValueField::ValueType SerialisedType;


    EnumValue() = default;

    explicit EnumValue(ParamValueType value)
      : intField_(static_cast<IntValueType>(value))
    {
    }

    EnumValue(const EnumValue&) = default;
    EnumValue(EnumValue&&) = default;
    ~EnumValue() = default;

    EnumValue& operator=(const EnumValue&) = default;
    EnumValue& operator=(EnumValue&&) = default;

    ParamValueType getValue() const
    {
        return static_cast<ValueType>(intField_.getValue());
    }

    void setValue(ParamValueType value)
    {
        intField_.setValue(static_cast<IntValueType>(value));
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

    static constexpr SerialisedType toSerialised(ParamValueType value)
    {
        return IntValueField::toSerialised(static_cast<IntValueType>(value));
    }

    static constexpr ParamValueType fromSerialised(SerialisedType value)
    {
        return static_cast<ValueType>(IntValueField::fromSerialised(value));
    }

    static constexpr bool valid()
    {
        return true;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return intField_.read(iter, size);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return intField_.write(iter, size);
    }

private:
    IntValueField intField_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


