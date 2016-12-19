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

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "details/OptionsParser.h"
#include "OptionalMode.h"
#include "basic/Optional.h"
#include "details/AdaptBasicField.h"

namespace comms
{

namespace field
{

/// @brief Adaptor class to any other field, that makes the field optional.
/// @details When field is optional, it may either exist or not. The behaviour
///     of length(), read() and write() operations depends on the current field's mode.
/// @tparam TField Proper type of the field that needs to be optional.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field.@n
///     Supported options are:
///     @li comms::option::DefaultValueInitialiser or comms::option::DefaultOptionalMode.
///     @li comms::option::ContentsValidator.
template <typename TField, typename... TOptions>
class Optional
{
    typedef basic::Optional<TField> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;
public:

    /// @brief All the options provided to this class bundled into struct.
    typedef details::OptionsParser<TOptions...> ParsedOptions;

    /// @brief Type of the field.
    typedef TField Field;

    /// @brief Value type of this field, equal to @ref Field
    typedef Field ValueType;

    /// @brief Mode of the field.
    /// @see OptionalMode
    typedef OptionalMode Mode;

    /// @brief Default constructor
    /// @details The mode it is created in is OptionalMode::Tentative.
    Optional() = default;

    /// @brief Construct the field.
    /// @param[in] fieldSrc Field to be copied from during construction.
    explicit Optional(const Field& fieldSrc)
      : field_(fieldSrc)
    {
    }

    /// @brief Construct the field.
    /// @param[in] fieldSrc Field to be moved from during construction.
    explicit Optional(Field&& fieldSrc)
      : field_(std::move(fieldSrc))
    {
    }

    /// @brief Copy constructor
    Optional(const Optional&) = default;

    /// @brief Move constructor
    Optional(Optional&&) = default;

    /// @brief Destructor
    ~Optional() = default;

    /// @brief Copy assignment
    Optional& operator=(const Optional&) = default;

    /// @brief Move assignment
    Optional& operator=(Optional&&) = default;

    /// @brief Get an access to the wrapped field object
    Field& field()
    {
        return field_.field();
    }

    /// @brief Get an access to the wrapped field object
    const Field& field() const
    {
        return field_.field();
    }

    /// @brief Get an access to the wrapped field object
    ValueType& value()
    {
        return field_.value();
    }

    /// @brief Get an access to the wrapped field object
    const ValueType& value() const
    {
        return field_.value();
    }

    /// @brief Get current optional mode
    Mode getMode() const
    {
        return field_.getMode();
    }

    /// @brief Get optional mode
    void setMode(Mode val)
    {
        GASSERT(val < Mode::NumOfModes);
        field_.setMode(val);
    }

    /// @brief Check whether mode is equivalent to Mode::Tentative
    /// @details Convenience wrapper for getMode(), equivalent to
    ///     @code return getMode() == Mode::Tentative; @endcode
    bool isTentative() const
    {
        return getMode() == Mode::Tentative;
    }

    /// @brief Set mode to Mode::Tentative
    /// @details Convenience wrapper for setMode(), equivalent to
    ///     @code setMode(Mode::Tentative); @endcode
    void setTentative()
    {
        setMode(Mode::Tentative);
    }

    /// @brief Check whether mode is equivalent to Mode::Missing
    /// @details Convenience wrapper for getMode(), equivalent to
    ///     @code return getMode() == Mode::Missing; @endcode
    bool isMissing() const
    {
        return getMode() == Mode::Missing;
    }

    /// @brief Set mode to Mode::Missing
    /// @details Convenience wrapper for setMode(), equivalent to
    ///     @code setMode(Mode::Missing); @endcode
    void setMissing()
    {
        setMode(Mode::Missing);
    }

    /// @brief Check whether mode is equivalent to Mode::Exists
    /// @details Convenience wrapper for getMode(), equivalent to
    ///     @code return getMode() == Mode::Exists; @endcode
    bool doesExist() const
    {
        return getMode() == Mode::Exists;
    }

    /// @brief Set mode to Mode::Exists
    /// @details Convenience wrapper for setMode(), equivalent to
    ///     @code setMode(Mode::Exists); @endcode
    void setExists()
    {
        setMode(Mode::Exists);
    }

    /// @brief Get length required to serialise the current field value.
    /// @return If current mode is OptionalMode::Exists, then the function
    ///     returns whatever length() member function of the wrapped field
    ///     returns. Otherwise (for both OptionalMode::Missing and
    ///     OptionalMode::Tentative) 0 is returned.
    std::size_t length() const
    {
        return field_.length();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    /// @return Same as Field::minLength()
    static constexpr std::size_t minLength()
    {
        return ThisField::minLength();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @return Same as Field::maxLength()
    static constexpr std::size_t maxLength()
    {
        return ThisField::maxLength();
    }

    /// @brief Check validity of the field value.
    /// @return If field is marked to be missing (mode is OptionalMode::Missing),
    ///     "true" is returned, otherwise valid() member function of the wrapped
    ///     field is called.
    bool valid() const
    {
        return field_.valid();
    }

    /// @brief Read field value from input data sequence
    /// @details If field is marked as missing (mode is OptionalMode::Missing),
    ///     function returns comms::ErrorStatus::Success without advancing iterator.@n
    ///     If field is marked as existing (mode is OptionalMode::Exists) the
    ///     read() member function of the wrapped field object is invoked.@n
    ///     If field is marked to be tentative (mode is OptionalMode::Tentative),
    ///     the call redirected to wrapped field's read() member function if
    ///     value of the "len" parameter is greater than 0, i.e. there are
    ///     still bytes available for reading, and field itself is marked as
    ///     existing.@n Otherwise, field is marked as missing and
    ///     comms::ErrorStatus::Success is returned.
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] len Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        return field_.read(iter, len);
    }

    /// @brief Write current field value to output data sequence
    /// @details If field is marked as missing (mode is OptionalMode::Missing),
    ///     function returns comms::ErrorStatus::Success without advancing iterator.@n
    ///     If field is marked as existing (mode is OptionalMode::Exists) the
    ///     write() member function of the wrapped field object is invoked.@n
    ///     If field is marked to be tentative (mode is OptionalMode::Tentative),
    ///     the call redirected to wrapped field's write() member function if
    ///     value of the "len" parameter is greater than 0, i.e. there is
    ///     space available for writing.@n Otherwise, comms::ErrorStatus::Success
    ///     is returned.
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] len Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return field_.write(iter, len);
    }

private:
    ThisField field_;
};

/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return Result of the equality comparison of the contained fields.
/// @related Optional
template <typename TField, typename... TOptions>
bool operator==(
    const Optional<TField, TOptions...>& field1,
    const Optional<TField, TOptions...>& field2)
{
    return field1.field() == field2.field();
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return Result of the non-equality comparison of the contained fields.
/// @related Optional
template <typename TField, typename... TOptions>
bool operator!=(
    const Optional<TField, TOptions...>& field1,
    const Optional<TField, TOptions...>& field2)
{
    return field1.field() != field2.field();
}

/// @brief Equivalence comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return Result of the equivalence comparison of the contained fields.
/// @related Optional
template <typename TField, typename... TOptions>
bool operator<(
    const Optional<TField, TOptions...>& field1,
    const Optional<TField, TOptions...>& field2)
{
    return field1.field() < field2.field();
}

namespace details
{

template <typename T>
struct IsOptional
{
    static const bool Value = false;
};

template <typename TField, typename... TOptions>
struct IsOptional<comms::field::Optional<TField, TOptions...> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::Optional.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref Optional
/// @related comms::field::Optional
template <typename T>
constexpr bool isOptional()
{
    return details::IsOptional<T>::Value;
}

}  // namespace field

}  // namespace comms


