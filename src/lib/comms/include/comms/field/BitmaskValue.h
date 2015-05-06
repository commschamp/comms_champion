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
#include "ComplexIntValue.h"

#include "comms/util/SizeToType.h"
#include "details/AdaptBasicField.h"
#include "details/OptionsParser.h"
#include "IntValue.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TOptionsBundle, bool THasFixedLength>
struct BitmaskUndertlyingType;

template <typename TOptionsBundle>
struct BitmaskUndertlyingType<TOptionsBundle, true>
{
    typedef typename comms::util::SizeToType<TOptionsBundle::FixedLength, false>::Type Type;
};

template <typename TOptionsBundle>
struct BitmaskUndertlyingType<TOptionsBundle, false>
{
    static_assert(!TOptionsBundle::HasVarLengthLimits,
        "Bitmask with variable length is not supported.");
    typedef unsigned Type;
};

template <typename TOptionsBundle>
using BitmaskUndertlyingTypeT =
    typename BitmaskUndertlyingType<TOptionsBundle, TOptionsBundle::HasFixedLengthLimit>::Type;


}  // namespace details

/// @addtogroup comms
/// @{

/// @brief Defines "Bitmask Value Field".
/// @details The class uses ComplexIntValue as its underlying type while providing
///          additional API functions to access/set/clear bits in the bitmask.
/// @tparam TField Base (interface) class for this field.
/// @tparam TLen Length of serialised data in bytes.
/// @headerfile comms/field/BitmaskValue.h
template <typename TFieldBase,
          typename... TOptions>
class BitmaskValue : public TFieldBase
{
    typedef TFieldBase Base;

    typedef details::OptionsParser<TOptions...> OptionsBundle;

    typedef details::BitmaskUndertlyingTypeT<OptionsBundle> IntValueType;

    typedef
        IntValue<
            TFieldBase,
            IntValueType,
            TOptions...
        > IntValueField;

public:

    typedef typename IntValueField::ValueType ValueType;

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

    BitmaskValue(BitmaskValue&&) = default;

    /// @brief Destructor is default
    ~BitmaskValue() = default;

    /// @brief Copy assignment is default
    BitmaskValue& operator=(const BitmaskValue&) = default;
    BitmaskValue& operator=(BitmaskValue&&) = default;

    /// @copydoc ComplexIntValue::getValue()
    const ValueType getValue() const
    {
        return intValue_.getValue();
    }

    /// @copydoc ComplexIntValue::setValue()
    void setValue(ValueType value)
    {
        intValue_.setValue(value);
    }

    constexpr std::size_t length() const
    {
        return intValue_.length();
    }

    static constexpr std::size_t maxLength()
    {
        return IntValueField::maxLength();
    }

    static constexpr std::size_t minLength()
    {
        return IntValueField::minLength();
    }

    /// @copydoc ComplexIntValue::read()
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return intValue_.read(iter, size);
    }

    /// @copydoc ComplexIntValue::write()
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return intValue_.write(iter, size);
    }

    constexpr bool valid() const
    {
        return intValue_.valid();
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

    bool getBitValue(unsigned bitNum) const
    {
        return hasAllBitsSet(
            static_cast<ValueType>(1U) << bitNum);
    }

    void setBitValue(unsigned bitNum, bool value)
    {
        auto mask = static_cast<ValueType>(1U) << bitNum;
        if (value) {
            setBits(mask);
        }
        else {
            clearBits(mask);
        }
    }

private:

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
    return field1.getValue() == field2.getValue();
}

/// @brief Non-equality comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator!=(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.getValue() != field2.getValue();
}

/// @brief Equivalence comparison operator.
/// @related BitmaskValue
template <typename... TArgs>
bool operator<(
    const BitmaskValue<TArgs...>& field1,
    const BitmaskValue<TArgs...>& field2)
{
    return field1.getValue() < field2.getValue();
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


