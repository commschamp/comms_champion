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

#include <type_traits>

#include "comms/options.h"
#include "details/OptionsParser.h"
#include "basic/EnumValue.h"
#include "details/AdaptBasicField.h"

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
class EnumValue : public TFieldBase
{
    static_assert(std::is_enum<TEnum>::value, "TEnum must be enum type");
    typedef TFieldBase Base;

    typedef basic::EnumValue<TFieldBase, TEnum> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

public:

    /// @brief All the options provided to this class bundled into struct.
    typedef details::OptionsParser<TOptions...> ParsedOptions;

    /// @brief Type of underlying enum value.
    /// @details Same as template parameter TEnum to this class.
    typedef typename ThisField::ValueType ValueType;


    /// @brief Default constructor.
    EnumValue() = default;

    /// @brief Constructor
    explicit EnumValue(const ValueType& val)
      : field_(val)
    {
    }

    /// @brief Copy constructor
    EnumValue(const EnumValue&) = default;

    /// @brief Destructor
    ~EnumValue() = default;

    /// @brief Copy assignment
    EnumValue& operator=(const EnumValue&) = default;

    /// @brief Get access to enum value storage.
    const ValueType& value() const
    {
        return field_.value();
    }

    /// @brief Get access to enum value storage.
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

    /// @brief Check validity of the field value.
    bool valid() const
    {
        return field_.valid();
    }

private:

    ThisField field_;
};

// Implementation

/// @brief Equality comparison operator.
/// @related EnumValue
template <typename... TArgs>
bool operator==(
    const EnumValue<TArgs...>& field1,
    const EnumValue<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @related EnumValue
template <typename... TArgs>
bool operator!=(
    const EnumValue<TArgs...>& field1,
    const EnumValue<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @related EnumValue
template <typename... TArgs>
bool operator<(
    const EnumValue<TArgs...>& field1,
    const EnumValue<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

namespace details
{

template <typename T>
struct IsEnumValue
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsEnumValue<comms::field::EnumValue<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::EnumValue.
/// @tparam T Any type.
/// @related comms::field::EnumValue
template <typename T>
constexpr bool isEnumValue()
{
    return details::IsEnumValue<T>::Value;
}


}  // namespace field

}  // namespace comms





