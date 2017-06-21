//
// Copyright 2014 - 2017 (C). Alex Robenko. All rights reserved.
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

#include "comms/options.h"
#include "details/OptionsParser.h"
#include "basic/EnumValue.h"
#include "details/AdaptBasicField.h"
#include "tag.h"

namespace comms
{

namespace field
{

/// @brief Enumerator value field.
/// @details Sometimes dealing with enum values is much more convenient that
///     using integral values. comms::field::EnumValue is very similar to
///     comms::field::IntValue, but receives underlying enum type in its
///     template parameters instead of integral one.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam TEnum Enderlying enum type, can be either unscoped or scoped (enum class).
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field. If no option is provided, the field's value is serialised as is,
///     where the length of the field is equal to the length of the underlying
///     enum type. For example:
///     @code
///         enum class MyEnum : std::uint16_t
///         {
///             Value1,
///             Value2,
///             Value3
///         }
///         using MyFieldBase = comms::Field<comms::option::BigEndian>;
///         using MyField =comms::field::EnumValue<MyFieldBase, MyEnum>;
///     @endcode
///     The serialised value of the field in the example above will consume
///     2 bytes, because the underlying type of MyEnum is
///     defined to be std::uint16_t. The value is serialised using big endian
///     notation because base field class receives comms::option::BigEndian option.@n
///     Supported options are:
///     @li comms::option::FixedLength
///     @li comms::option::FixedBitLength
///     @li comms::option::VarLength
///     @li comms::option::NumValueSerOffset
///     @li comms::option::DefaultValueInitialiser or comms::option::DefaultNumValue.
///     @li comms::option::ContentsValidator or comms::option::ValidNumValueRange.
///     @li comms::option::FailOnInvalid
///     @li comms::option::IgnoreInvalid
template <typename TFieldBase, typename TEnum, typename... TOptions>
class EnumValue : public details::AdaptBasicFieldT<basic::EnumValue<TFieldBase, TEnum>, TOptions...>
{
    using Base = details::AdaptBasicFieldT<basic::EnumValue<TFieldBase, TEnum>, TOptions...>;
    static_assert(std::is_enum<TEnum>::value, "TEnum must be enum type");

public:

    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = details::OptionsParser<TOptions...>;

    /// @brief Tag indicating type of the field
    using Tag = tag::Enum;

    /// @brief Type of underlying enum value.
    /// @details Same as template parameter TEnum to this class.
    using ValueType = typename Base::ValueType;

    /// @brief Default constructor.
    EnumValue() = default;

    /// @brief Constructor
    explicit EnumValue(const ValueType& val)
      : Base(val)
    {
    }

    /// @brief Copy constructor
    EnumValue(const EnumValue&) = default;

    /// @brief Destructor
    ~EnumValue() = default;

    /// @brief Copy assignment
    EnumValue& operator=(const EnumValue&) = default;

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Get access to enum value storage.
    const ValueType& value() const;

    /// @brief Get access to enum value storage.
    ValueType& value();

    /// @brief Get length required to serialise the current field value.
    /// @return Number of bytes it will take to serialise the field value.
    constexpr std::size_t length() const
    {;
    /// @brief Get minimal length that is required to serialise field of this type.
    /// @return Minimal number of bytes required serialise the field value.
    static constexpr std::size_t minLength();

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @return Maximal number of bytes required serialise the field value.
    static constexpr std::size_t maxLength();

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

    /// @brief Check validity of the field value.
    bool valid() const;

    /// @brief Refresh the field's value
    /// @return @b true if the value has been updated, @b false otherwise
    bool refresh();

#endif // #ifdef FOR_DOXYGEN_DOC_ONLY
};

// Implementation

/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are equal, false otherwise.
/// @related EnumValue
template <typename... TArgs>
bool operator==(
    const EnumValue<TArgs...>& field1,
    const EnumValue<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are NOT equal, false otherwise.
/// @related EnumValue
template <typename... TArgs>
bool operator!=(
    const EnumValue<TArgs...>& field1,
    const EnumValue<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case value of the first field is lower than than the value of the second.
/// @related EnumValue
template <typename... TArgs>
bool operator<(
    const EnumValue<TArgs...>& field1,
    const EnumValue<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::EnumValue.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref EnumValue
/// @related comms::field::EnumValue
template <typename T>
constexpr bool isEnumValue()
{
    return std::is_same<typename T::Tag, tag::Enum>::value;
}

/// @brief Upcast type of the field definition to its parent comms::field::EnumValue type
///     in order to have access to its internal types.
/// @related comms::field::EnumValue
template <typename TFieldBase, typename TEnum, typename... TOptions>
inline
EnumValue<TFieldBase, TEnum, TOptions...>&
toFieldBase(EnumValue<TFieldBase, TEnum, TOptions...>& field)
{
    return field;
}

/// @brief Upcast type of the field definition to its parent comms::field::EnumValue type
///     in order to have access to its internal types.
/// @related comms::field::EnumValue
template <typename TFieldBase, typename TEnum, typename... TOptions>
inline
const EnumValue<TFieldBase, TEnum, TOptions...>&
toFieldBase(const EnumValue<TFieldBase, TEnum, TOptions...>& field)
{
    return field;
}

}  // namespace field

}  // namespace comms





