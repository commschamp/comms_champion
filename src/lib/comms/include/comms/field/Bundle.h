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

#include "comms/ErrorStatus.h"
#include "comms/field/category.h"
#include "comms/options.h"
#include "basic/Bundle.h"
#include "details/AdaptBasicField.h"

namespace comms
{

namespace field
{

/// @brief Bundles multiple fields into a single field.
/// @details The class wraps nicely multiple fields and provides
///     expected single field API functions, such as length(), read(), write(),
///     valid(). It may be useful when a collection (comms::field::ArrayList) of
///     complex fields is required.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam TMembers All wrapped fields bundled together in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field.@n
///     Supported options are:
///     @li comms::option::DefaultValueInitialiser - All wrapped fields may
///         specify their independent default value initialisers. It is
///         also possible to provide initialiser for the Bundle field which
///         will set appropriate values to the fields based on some
///         internal logic.
///     @li comms::option::ContentsValidator - All wrapped fields may specify
///         their independent validators. The bundle field considered to
///         be valid if all the wrapped fields are valid. This option though,
///         provides an ability to add extra validation logic that can
///         observe value of more than one wrapped fields. For example,
///         protocol specifies that if one specific field has value X, than
///         other field is NOT allowed to have value Y.
template <typename TFieldBase, typename TMembers, typename... TOptions>
class Bundle
{
    static_assert(comms::util::IsTuple<TMembers>::Value,
        "TMembers is expected to be a tuple of BundleMember<...>");

    static_assert(
        1U < std::tuple_size<TMembers>::value,
        "Number of members is expected to be at least 2.");

    typedef basic::Bundle<TFieldBase, TMembers> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

public:
    /// @brief All the options provided to this class bundled into struct.
    typedef details::OptionsParser<TOptions...> ParsedOptions;

    /// @brief Value type.
    /// @details Same as TMemebers template argument, i.e. it is std::tuple
    ///     of all the wrapped fields.
    typedef typename ThisField::ValueType ValueType;

    /// @brief Default constructor
    /// @details Invokes default constructor of every wrapped field
    Bundle() = default;

    /// @brief Constructor
    explicit Bundle(const ValueType& val)
      : field_(val)
    {
    }

    /// @brief Constructor
    explicit Bundle(ValueType&& val)
      : field_(std::move(val))
    {
    }

    /// @brief Get access to the stored tuple of fields.
    ValueType& value()
    {
        return field_.value();
    }

    /// @brief Get access to the stored tuple of fields.
    const ValueType& value() const
    {
        return field_.value();
    }

    /// @brief Get length required to serialise bundled fields.
    /// @details Summarises all the results returned by the call to length() for
    ///     every field in the bundle.
    constexpr std::size_t length() const
    {
        return field_.length();
    }

    /// @brief Get minimal length that is required to serialise all bundled fields.
    static constexpr std::size_t minLength()
    {
        return ThisField::minLength();
    }

    /// @brief Get maximal length that is required to serialise all bundled fields.
    static constexpr std::size_t maxLength()
    {
        return ThisField::maxLength();
    }

    /// @brief Read field value from input data sequence
    /// @details Invokes read() member function over every bundled field.
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
    /// @details Invokes write() member function over every bundled field.
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return field_.write(iter, size);
    }

    /// @brief Check validity of all the bundled fields.
    constexpr bool valid() const {
        return field_.valid();
    }

private:
    ThisField field_;
};

/// @brief Equality comparison operator.
/// @related Bundle
template <typename... TArgs>
bool operator==(
    const Bundle<TArgs...>& field1,
    const Bundle<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @related Bundle
template <typename... TArgs>
bool operator!=(
    const Bundle<TArgs...>& field1,
    const Bundle<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

namespace details
{

template <typename T>
struct IsBundle
{
    static const bool Value = false;
};

template <typename TFieldBase, typename TMembers, typename... TOptions>
struct IsBundle<comms::field::Bundle<TFieldBase, TMembers, TOptions...> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::Bundle.
/// @tparam T Any type.
/// @related comms::field::Bundle
template <typename T>
constexpr bool isBundle()
{
    return details::IsBundle<T>::Value;
}


}  // namespace field

}  // namespace comms


