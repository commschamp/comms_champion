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
#include "comms/field/category.h"
#include "comms/options.h"
#include "basic/Bundle.h"
#include "details/AdaptBasicField.h"
#include "tag.h"

namespace comms
{

namespace field
{

/// @brief Bundles multiple fields into a single field.
/// @details The class wraps nicely multiple fields and provides
///     expected single field API functions, such as length(), read(), write(),
///     valid(). It may be useful when a collection (comms::field::ArrayList) of
///     complex fields is required.
///
///     Refer to @ref sec_field_tutorial_bundle for tutorial and usage examples.
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
///     @li comms::option::CustomValueReader - It may be required to implement
///         custom reading functionality instead of default behaviour of
///         invoking read() member function of every member field. It is possible
///         to provide cusom reader functionality using comms::option::CustomValueReader
///         option.
template <typename TFieldBase, typename TMembers, typename... TOptions>
class Bundle : public details::AdaptBasicFieldT<basic::Bundle<TFieldBase, TMembers>, TOptions...>
{
    using Base = details::AdaptBasicFieldT<basic::Bundle<TFieldBase, TMembers>, TOptions...>;
    static_assert(comms::util::IsTuple<TMembers>::Value,
        "TMembers is expected to be a tuple of std::tuple<...>");

    static_assert(
        1U < std::tuple_size<TMembers>::value,
        "Number of members is expected to be at least 2.");

public:
    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = details::OptionsParser<TOptions...>;

    /// @brief Tag indicating type of the field
    using Tag = tag::Bundle;

    /// @brief Value type.
    /// @details Same as TMemebers template argument, i.e. it is std::tuple
    ///     of all the wrapped fields.
    using ValueType = typename Base::ValueType;

    /// @brief Default constructor
    /// @details Invokes default constructor of every wrapped field
    Bundle() = default;

    /// @brief Constructor
    explicit Bundle(const ValueType& val)
      : Base(val)
    {
    }

    /// @brief Constructor
    explicit Bundle(ValueType&& val)
      : Base(std::move(val))
    {
    }

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Get access to the stored tuple of fields.
    ValueType& value();

    /// @brief Get access to the stored tuple of fields.
    const ValueType& value() const;

    /// @brief Get length required to serialise bundled fields.
    /// @details Summarises all the results returned by the call to length() for
    ///     every field in the bundle.
    /// @return Number of bytes it will take to serialise the field value.
    constexpr std::size_t length() const;

    /// @brief Get minimal length that is required to serialise all bundled fields.
    /// @return Minimal number of bytes required serialise the field value.
    static constexpr std::size_t minLength();

    /// @brief Get maximal length that is required to serialise all bundled fields.
    /// @return Maximal number of bytes required serialise the field value.
    static constexpr std::size_t maxLength();

    /// @brief Read field value from input data sequence
    /// @details Invokes read() member function over every bundled field.
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size);

    /// @brief Write current field value to output data sequence
    /// @details Invokes write() member function over every bundled field.
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const;

    /// @brief Check validity of all the bundled fields.
    constexpr bool valid() const;
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY
};

/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are equal, false otherwise.
/// @related Bundle
template <typename TFieldBase, typename TMembers, typename... TOptions>
bool operator==(
    const Bundle<TFieldBase, TMembers, TOptions...>& field1,
    const Bundle<TFieldBase, TMembers, TOptions...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are NOT equal, false otherwise.
/// @related Bundle
template <typename TFieldBase, typename TMembers, typename... TOptions>
bool operator!=(
    const Bundle<TFieldBase, TMembers, TOptions...>& field1,
    const Bundle<TFieldBase, TMembers, TOptions...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::Bundle.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref Bundle
/// @related comms::field::Bundle
template <typename T>
constexpr bool isBundle()
{
    return std::is_same<typename T::Tag, tag::Bundle>::value;
}

/// @brief Upcast type of the field definition to its parent comms::field::Bundle type
///     in order to have access to its internal types.
/// @related comms::field::Bundle
template <typename TFieldBase, typename TMembers, typename... TOptions>
inline
Bundle<TFieldBase, TMembers, TOptions...>&
toFieldBase(Bundle<TFieldBase, TMembers, TOptions...>& field)
{
    return field;
}

/// @brief Upcast type of the field definition to its parent comms::field::Bundle type
///     in order to have access to its internal types.
/// @related comms::field::Bundle
template <typename TFieldBase, typename TMembers, typename... TOptions>
inline
const Bundle<TFieldBase, TMembers, TOptions...>&
toFieldBase(const Bundle<TFieldBase, TMembers, TOptions...>& field)
{
    return field;
}

}  // namespace field

}  // namespace comms


