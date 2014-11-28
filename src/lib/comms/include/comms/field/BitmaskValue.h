//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

/// @file comms/field/BitmaskValue.h
/// This file contains definition of bitmask value field that
/// can be used in message definition.

#pragma once

#include <limits>
#include "comms/Field.h"
#include "BasicIntValue.h"

#include "details/BitmaskValueBase.h"

namespace comms
{

namespace field
{

/// @addtogroup comms
/// @{

/// @brief Defines "Bitmask Value Field".
/// @details The class uses BasicIntValue as its underlying type while providing
///          additional API functions to access/set/clear bits in the bitmask.
/// @tparam TField Base (interface) class for this field.
/// @tparam TLen Length of serialised data in bytes.
/// @headerfile comms/field/BitmaskValue.h
template <typename TField,
          typename... TOptions>
class BitmaskValue : details::BitmaskValueBase<TField, TOptions...>
{
    typedef details::BitmaskValueBase<TField, TOptions...> Base;

public:

    /// @brief Type of the stored value
    typedef typename Base::ValueType ValueType;

    /// @brief Default value to be set in default constructor
    static const auto DefaultValue = Base::DefaultValue;

    /// @brief Length of serialised data
    static const std::size_t SerialisedLen = Base::SerialisedLen;

    /// @brief Reserved bits mask
    static const auto ReservedMask = Base::ReservedMask;

    /// @brief Valid value for reserved bits.
    static const bool ReservedValue = Base::ReservedValue;

    static const bool BitOrderMsbFirst = Base::BitOrderMsbFirst;

    /// @brief Definition of underlying BasicIntValue field type
    typedef
        BasicIntValue<
            TField,
            ValueType,
            option::LengthLimitImpl<SerialisedLen>,
            option::DefaultValueImpl<DefaultValue>
        > IntValueField;

    /// @brief Serialised Type
    typedef typename IntValueField::SerialisedType SerialisedType;

    /// @brief Default constructor.
    /// @brief Initial bitmask has all bits cleared (equals 0)
    BitmaskValue() = default;

    /// @brief Constructor
    explicit BitmaskValue(ValueType value)
       : intValue_(value)
    {
    }

    /// @brief Copy constructor is default
    BitmaskValue(const BitmaskValue&) = default;

    /// @brief Destructor is default
    ~BitmaskValue() = default;

    /// @brief Copy assignment is default
    BitmaskValue& operator=(const BitmaskValue&) = default;

    /// @brief Retrieve underlying BasicIntValue field.
    const IntValueField asIntValueField() const
    {
        return intValue_;
    }

    /// @copydoc BasicIntValue::getValue()
    const ValueType getValue() const
    {
        return intValue_.getValue();
    }

    /// @copydoc BasicIntValue::setValue()
    void setValue(ValueType value)
    {
        intValue_.setValue(value);
    }

    /// @copydoc BasicIntValue::getSerialisedValue()
    const SerialisedType getSerialisedValue() const
    {
        return intValue_.getSerialisedValue();
    }

    /// @copydoc BasicIntValue::setSerialisedValue()
    void setSerialisedValue(SerialisedType value)
    {
        intValue_.setSerialisedValue(value);
    }

    /// @copydoc BasicIntValue::toSerialised()
    static constexpr const SerialisedType toSerialised(ValueType value)
    {
        return IntValueField::toSerialised(value);
    }

    /// @copydoc BasicIntValue::fromSerialised()
    static constexpr const ValueType fromSerialised(SerialisedType value)
    {
        return IntValueField::fromSerialised(value);
    }

    /// @copydoc BasicIntValue::length()
    static constexpr std::size_t length()
    {
        return IntValueField::length();
    }

    /// @copydoc BasicIntValue::read()
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return intValue_.read(iter, size);
    }

    /// @copydoc BasicIntValue::write()
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return intValue_.write(iter, size);
    }

    constexpr bool valid() const
    {
        if (ReservedValue) {
            return (getValue() & ReservedMask) == ReservedMask;
        }
        return (getValue() & ReservedMask) == 0;
    }

    /// @brief Check whether all bits from provided mask are set.
    /// @param[in] mask Mask to check against
    /// @return true in case all the bits are set, false otherwise
    bool hasAllBitsSet(ValueType mask) const
    {
        return (getValue() & mask) == mask;
    }

    /// @brief Check whether any bits from provided mask are set.
    /// @param[in] mask Mask to check against
    /// @return true in case at least one of the bits is set, false otherwise.
    bool hasAnyBitsSet(ValueType mask) const
    {
        return (getValue() & mask) != 0;
    }

    /// @brief Set all the provided bits.
    /// @details Equivalent to @code setValue(getValue() | mask); @endcode
    /// @param[in] mask Mask of bits to set.
    void setBits(ValueType mask)
    {
        setValue(getValue() | mask);
    }

    /// @brief Set all the provided bits.
    /// @details Equivalent to @code setValue(getValue() & (~mask)); @endcode
    /// @param[in] mask Mask of bits to clear.
    void clearBits(ValueType mask)
    {
        setValue(getValue() & (~mask));
    }

    const bool getBitValue(unsigned bitNum) const
    {
        return hasAllBitsSet(calcMask(bitNum, BitOrderTag()));
    }

    void setBitValue(unsigned bitNum, bool value)
    {
        auto mask = calcMask(bitNum, BitOrderTag());
        if (value) {
            setBits(mask);
        }
        else {
            clearBits(mask);
        }
    }

private:
    struct BitOrderMsbFirstTag {};
    struct BitOrderLsbFirstTag {};
    using BitOrderTag = typename
        std::conditional<
            BitOrderMsbFirst,
            BitOrderMsbFirstTag,
            BitOrderLsbFirstTag
        >::type;


    static ValueType calcMask(unsigned bitNum, BitOrderMsbFirstTag)
    {
        GASSERT(bitNum < std::numeric_limits<ValueType>::digits);
        auto shift = (std::numeric_limits<ValueType>::digits - 1) - bitNum;
        auto mask = static_cast<ValueType>(1) << shift;
        return mask;
    }

    static ValueType calcMask(unsigned bitNum, BitOrderLsbFirstTag)
    {
        GASSERT(bitNum < std::numeric_limits<ValueType>::digits);
        auto mask = static_cast<ValueType>(1) << bitNum;
        return mask;
    }

    IntValueField intValue_;
};

// Implementation

/// @brief Equality comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator==(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.asIntValueField() == field2.asIntValueField();
}

/// @brief Non-equality comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator!=(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.asIntValueField() != field2.asIntValueField();
}

/// @brief Equivalence comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator<(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.asIntValueField() < field2.asIntValueField();
}

namespace details
{

template <typename T>
struct IsBitmaskValue
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsBitmaskValue<comms::field::BitmaskValue<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isBitmaskValue()
{
    return details::IsBitmaskValue<T>::Value;
}


/// @}

}  // namespace field

}  // namespace comms


