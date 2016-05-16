//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include <ratio>
#include <limits>
#include <type_traits>

#include "comms/ErrorStatus.h"
#include "comms/field/category.h"
#include "comms/options.h"
#include "basic/IntValue.h"
#include "details/AdaptBasicField.h"

namespace comms
{

namespace field
{

/// @brief Field that represent integral value.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam T Basic underlying integral type.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field. If no option is provided The field's value is serialised as is.
///     @code
///         using MyFieldBase = comms::Field<comms::option::BigEndian>;
///         using MyField =comms::field::IntValue<MyFieldBase, std::uint16_t>;
///     @endcode
///     In the example above it will
///     consume 2 bytes (because sizeof(std::uint16_t) == 2) and will
///     be serialised using big endian notation.@n
///     Supported options are:
///     @li comms::option::FixedLength
///     @li comms::option::FixedBitLength
///     @li comms::option::VarLength
///     @li comms::option::NumValueSerOffset
///     @li comms::option::DefaultValueInitialiser or comms::option::DefaultNumValue.
///     @li comms::option::ContentsValidator or comms::option::ValidNumValueRange.
///     @li comms::option::FailOnInvalid
///     @li comms::option::IgnoreInvalid
///     @li comms::option::ScalingRatio
template <typename TFieldBase, typename T, typename... TOptions>
class IntValue : public TFieldBase
{
    typedef TFieldBase Base;

    typedef basic::IntValue<TFieldBase, T> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

    static_assert(std::is_base_of<comms::field::category::NumericValueField, typename ThisField::Category>::value,
        "ThisField is expected to be of NumericFieldCategory");
public:

    /// @brief All the options provided to this class bundled into struct.
    typedef details::OptionsParser<TOptions...> ParsedOptions;

    /// @brief Type of underlying integral value.
    /// @details Same as template parameter T to this class.
    typedef typename ThisField::ValueType ValueType;

    /// @brief Default constructor
    /// @details Initialises internal value to 0.
    IntValue() = default;

    /// @brief Constructor
    explicit IntValue(const ValueType& val)
      : field_(val)
    {
    }

    /// @brief Copy constructor
    IntValue(const IntValue&) = default;

    /// @brief Copy assignment
    IntValue& operator=(const IntValue&) = default;

    /// @brief Get access to integral value storage.
    const ValueType& value() const
    {
        return field_.value();
    }

    /// @brief Get access to integral value storage.
    ValueType& value()
    {
        return field_.value();
    }

    /// @brief Get length required to serialise the current field value.
    constexpr std::size_t length() const
    {
        return field_.length();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    static constexpr std::size_t minLength()
    {
        return ThisField::minLength();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    static constexpr std::size_t maxLength()
    {
        return ThisField::maxLength();
    }

    /// @brief Check validity of the field value.
    constexpr bool valid() const
    {
        return field_.valid();
    }

    /// @brief Read field value from input data sequence
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return field_.read(iter, size);
    }

    /// @brief Write current field value to output data sequence
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return field_.write(iter, size);
    }

    /// @brief Scales value according to ratio specified in provided
    ///     comms::option::ScalingRatio option.
    /// @details If comms::option::ScalingRatio option wasn't used, then
    ///     comms::option::ScalingRatio<1,1> is assumed.
    /// @tparam TRet Return type for the scaled value.
    /// @return "(value() * Scaling_Num) / Scaling_Denom" when all values are
    ///     casted to TRet type.
    template <typename TRet>
    constexpr TRet scaleAs() const
    {
        static_assert(std::is_floating_point<TRet>::value,
            "TRet is expected to be floating point type");
        return static_cast<TRet>(value()) * (static_cast<TRet>(ThisField::ScalingRatio::num) / static_cast<TRet>(ThisField::ScalingRatio::den));
    }

    /// @brief Opposite operation to scaleAs().
    /// @details Allows to assign scaled value, assigns "(val * Scaling_Denom) / Scaling_Num"
    ///     to the value of the field.
    /// @param[in] val Scaled value.
    template <typename TScaled>
    void setScaled(TScaled val)
    {
        typedef typename std::decay<decltype(val)>::type DecayedType;
        auto mul = 10;
        if ((ThisField::ScalingRatio::den & (ThisField::ScalingRatio::den - 1)) == 0) {
            mul = 2;
        }

        auto epsilon = static_cast<TScaled>(ThisField::ScalingRatio::num) / static_cast<TScaled>(ThisField::ScalingRatio::den * mul);
        if (val < static_cast<DecayedType>(0)) {
            epsilon = -epsilon;
        }

        value() =
            static_cast<ValueType>(
                ((val + epsilon) * static_cast<TScaled>(ThisField::ScalingRatio::den)) / static_cast<TScaled>(ThisField::ScalingRatio::num));
    }

private:
    ThisField field_;
};


/// @brief Equality comparison operator.
/// @related IntValue
template <typename... TArgs>
bool operator==(
    const IntValue<TArgs...>& field1,
    const IntValue<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @related IntValue
template <typename... TArgs>
bool operator!=(
    const IntValue<TArgs...>& field1,
    const IntValue<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @related IntValue
template <typename... TArgs>
bool operator<(
    const IntValue<TArgs...>& field1,
    const IntValue<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

namespace details
{

template <typename T>
struct IsIntValue
{
    static const bool Value = false;
};

template <typename TFieldBase, typename T, typename... TOptions>
struct IsIntValue<comms::field::IntValue<TFieldBase, T, TOptions...> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::IntValue.
/// @tparam T Any type.
/// @related comms::field::IntValue
template <typename T>
constexpr bool isIntValue()
{
    return details::IsIntValue<T>::Value;
}


}  // namespace field

}  // namespace comms



