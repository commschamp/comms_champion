//
// Copyright 2015 - 2017 (C). Alex Robenko. All rights reserved.
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

#include "comms/ErrorStatus.h"
#include "comms/options.h"
#include "basic/FloatValue.h"
#include "details/AdaptBasicField.h"
#include "tag.h"

namespace comms
{

namespace field
{

/// @brief Field that represent floating point value.
/// @details Represents IEEE 754 floating point value, which means the value is
///     serialised as is (consumes 4 bytes for float, and 8 bytes for double),
///     using big or little endian.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam T Basic underlying floating point type, must be either float or double.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field. If no option is provided The field's value is serialised as is.
///     @code
///         using MyFieldBase = comms::Field<comms::option::BigEndian>;
///         using MyField =comms::field::FloatValue<MyFieldBase, float>;
///     @endcode
///     In the example above it will
///     consume 4 bytes (because sizeof(float) == 4) and will
///     be serialised using big endian notation.@n
///     Supported options are:
///     @li comms::option::DefaultValueInitialiser or comms::option::DefaultNumValue.
///     @li comms::option::ContentsValidator or comms::option::ValidNumValueRange.
///     @li comms::option::ContentsRefresher
///     @li comms::option::FailOnInvalid
///     @li comms::option::IgnoreInvalid
///     @li comms::option::Units* - all variants of value units, see
///         @ref sec_field_tutorial_int_value_units for details.
/// @extends comms::Field
/// @headerfile comms/field/FloatValue.h
template <typename TFieldBase, typename T, typename... TOptions>
class FloatValue : public details::AdaptBasicFieldT<basic::FloatValue<TFieldBase, T>, TOptions...>
{
    using Base = details::AdaptBasicFieldT<basic::FloatValue<TFieldBase, T>, TOptions...>;
public:

    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = details::OptionsParser<TOptions...>;

    /// @brief Tag indicating type of the field
    using Tag = tag::Float;

    /// @brief Type of underlying floating point value.
    /// @details Same as template parameter T to this class.
    using ValueType = typename Base::ValueType;

    /// @brief Default constructor
    /// @details Initialises internal value to 0.
    FloatValue() = default;

    /// @brief Constructor
    explicit FloatValue(const ValueType& val)
      : Base(val)
    {
    }

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Get access to floating point value storage.
    const ValueType& value() const;

    /// @brief Get access to floating point value storage.
    ValueType& value();

    /// @brief Get length required to serialise the current field value.
    /// @return Number of bytes it will take to serialise the field value.
    constexpr std::size_t length() const;

    /// @brief Get minimal length that is required to serialise field of this type.
    /// @return Minimal number of bytes required serialise the field value.
    static constexpr std::size_t minLength();

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @return Maximal number of bytes required serialise the field value.
    static constexpr std::size_t maxLength();

    /// @brief Check validity of the field value.
    bool valid() const;

    /// @brief Refresh the field's value
    /// @return @b true if the value has been updated, @b false otherwise
    bool refresh();

    /// @brief Read field value from input data sequence
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size);

    /// @brief Write current field value to output data sequence
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const;
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY
};


/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are equal, false otherwise.
/// @related FloatValue
template <typename... TArgs>
bool operator==(
    const FloatValue<TArgs...>& field1,
    const FloatValue<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are NOT equal, false otherwise.
/// @related FloatValue
template <typename... TArgs>
bool operator!=(
    const FloatValue<TArgs...>& field1,
    const FloatValue<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case value of the first field is lower than than the value of the second.
/// @related FloatValue
template <typename... TArgs>
bool operator<(
    const FloatValue<TArgs...>& field1,
    const FloatValue<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::FloatValue.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref FloatValue
/// @related comms::field::FloatValue
template <typename T>
constexpr bool isFloatValue()
{
    return std::is_same<typename T::Tag, tag::Float>::value;
}

/// @brief Upcast type of the field definition to its parent comms::field::FloatValue type
///     in order to have access to its internal types.
/// @related comms::field::FloatValue
template <typename TFieldBase, typename T, typename... TOptions>
inline
FloatValue<TFieldBase, T, TOptions...>&
toFieldBase(FloatValue<TFieldBase, T, TOptions...>& field)
{
    return field;
}

/// @brief Upcast type of the field definition to its parent comms::field::FloatValue type
///     in order to have access to its internal types.
/// @related comms::field::FloatValue
template <typename TFieldBase, typename T, typename... TOptions>
inline
const FloatValue<TFieldBase, T, TOptions...>&
toFieldBase(const FloatValue<TFieldBase, T, TOptions...>& field)
{
    return field;
}

}  // namespace field

}  // namespace comms



