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

namespace comms
{

namespace field
{

/// @brief Mode to be used by comms::field::Optional
/// @related comms::field::Optional
enum class OptionalMode
{
    Tentative, ///< The field existence is tentative, i.e. If there is enough bytes
               /// to read the field's value, than field exists, if not
               /// then it doesn't exist.
    Exists, ///< Field must exist
    Missing, ///< Field doesn't exist
    NumOfModes ///< Number of possible modes, must be last
};

/// @brief Adaptor class to any other field, that makes the field optional.
/// @details When field is optional, it may either exist or not. The behaviour
///     of length(), read() and write() operations depends on the current field's mode.
/// @tparam TField Proper type of the field that needs to be optional.
template <typename TField>
class Optional
{
public:

    /// @brief All the options provided to this class bundled into struct.
    typedef details::OptionsParser<> ParsedOptions;

    /// @brief Type of the field.
    typedef TField Field;

    /// @brief Mode of the field.
    /// @see OptionalMode
    typedef OptionalMode Mode;

    /// @brief Default constructor
    /// @details The mode it is created in is OptionalMode::Tentative.
    Optional() = default;

    /// @brief Construct the field.
    /// @param[in] fieldSrc Field to be copied from during construction.
    /// @param[in] mode Mode of the field.
    explicit Optional(const Field& fieldSrc, Mode mode = Mode::Tentative)
      : field_(fieldSrc),
        mode_(mode)
    {
    }

    /// @brief Construct the field.
    /// @param[in] fieldSrc Field to be moved from during construction.
    /// @param[in] mode Mode of the field.
    explicit Optional(Field&& fieldSrc, Mode mode = Mode::Tentative)
      : field_(std::move(fieldSrc)),
        mode_(mode)
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
        return field_;
    }

    /// @brief Get an access to the wrapped field object
    const Field& field() const
    {
        return field_;
    }

    /// @brief Get current optional mode
    Mode getMode() const
    {
        return mode_;
    }

    /// @brief Get optional mode
    void setMode(Mode val)
    {
        GASSERT(val < Mode::NumOfModes);
        mode_ = val;
    }

    /// @brief Get length required to serialise the current field value.
    /// @return If current mode is OptionalMode::Exists, then the function
    ///     returns whatever length() member function of the wrapped field
    ///     returns. Otherwise (for both OptionalMode::Missing and
    ///     OptionalMode::Tentative) 0 is returned.
    std::size_t length() const
    {
        if (mode_ != Mode::Exists) {
            return 0U;
        }

        return field_.length();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    /// @details Same as Field::minLength()
    static constexpr std::size_t minLength()
    {
        return Field::minLength();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @details Same as Field::maxLength()
    static constexpr std::size_t maxLength()
    {
        return Field::maxLength();
    }

    /// @brief Check validity of the field value.
    /// @return If field is marked to be missing (mode is OptionalMode::Missing),
    ///     "true" is returned, otherwise valid() member function of the wrapped
    ///     field is called.
    bool valid() const
    {
        if (mode_ == Mode::Missing) {
            return true;
        }

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
        if (mode_ == Mode::Missing) {
            return comms::ErrorStatus::Success;
        }

        if ((mode_ == Mode::Tentative) && (0U == len)) {
            mode_ = Mode::Missing;
            return comms::ErrorStatus::Success;
        }

        auto es = field_.read(iter, len);
        if (es == comms::ErrorStatus::Success) {
            mode_ = Mode::Exists;
        }
        return es;
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
        if (mode_ == Mode::Missing) {
            return comms::ErrorStatus::Success;
        }

        if ((mode_ == Mode::Tentative) && (0U == len)) {
            return comms::ErrorStatus::Success;
        }

        return field_.write(iter, len);
    }

private:
    Field field_;
    Mode mode_ = Mode::Tentative;
};

/// @brief Equality comparison operator.
/// @related Optional
template <typename... TArgs>
bool operator==(
    const Optional<TArgs...>& field1,
    const Optional<TArgs...>& field2)
{
    return field1.field() == field2.field();
}

/// @brief Non-equality comparison operator.
/// @related Optional
template <typename... TArgs>
bool operator!=(
    const Optional<TArgs...>& field1,
    const Optional<TArgs...>& field2)
{
    return field1.field() != field2.field();
}

/// @brief Equivalence comparison operator.
/// @related Optional
template <typename... TArgs>
bool operator<(
    const Optional<TArgs...>& field1,
    const Optional<TArgs...>& field2)
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

template <typename TField>
struct IsOptional<comms::field::Optional<TField> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::Optional.
/// @tparam T Any type.
/// @related comms::field::Optional
template <typename T>
constexpr bool isOptional()
{
    return details::IsOptional<T>::Value;
}


}  // namespace field

}  // namespace comms


