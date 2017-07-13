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

#include <vector>

#include "comms/ErrorStatus.h"
#include "comms/options.h"
#include "comms/util/StaticVector.h"
#include "comms/util/ArrayView.h"
#include "basic/ArrayList.h"
#include "details/AdaptBasicField.h"
#include "details/OptionsParser.h"
#include "tag.h"

namespace comms
{

namespace field
{

namespace details
{

template <bool THasOrigDataViewStorage>
struct ArrayListOrigDataViewStorageType;

template <>
struct ArrayListOrigDataViewStorageType<true>
{
    template <typename TElement>
    using Type = comms::util::ArrayView<TElement>;
};

template <>
struct ArrayListOrigDataViewStorageType<false>
{
    template <typename TElement>
    using Type = std::vector<TElement>;
};

template <bool THasFixedSizeStorage>
struct ArrayListFixedSizeStorageType;

template <>
struct ArrayListFixedSizeStorageType<true>
{
    template <typename TElement, typename TOpt>
    using Type = comms::util::StaticVector<TElement, TOpt::FixedSizeStorage>;
};

template <>
struct ArrayListFixedSizeStorageType<false>
{
    template <typename TElement, typename TOpt>
    using Type =
        typename ArrayListOrigDataViewStorageType<
            TOpt::HasOrigDataView && std::is_integral<TElement>::value && (sizeof(TElement) == sizeof(std::uint8_t))
        >::template Type<TElement>;
};

template <bool THasCustomStorage>
struct ArrayListCustomArrayListStorageType;

template <>
struct ArrayListCustomArrayListStorageType<true>
{
    template <typename TElement, typename TOpt>
    using Type = typename TOpt::CustomStorageType;
};

template <>
struct ArrayListCustomArrayListStorageType<false>
{
    template <typename TElement, typename TOpt>
    using Type =
        typename ArrayListFixedSizeStorageType<TOpt::HasFixedSizeStorage>::template Type<TElement, TOpt>;
};

template <typename TElement, typename TOpt>
using ArrayListStorageTypeT =
    typename ArrayListCustomArrayListStorageType<TOpt::HasCustomStorageType>::template Type<TElement, TOpt>;

template <typename TFieldBase, typename TElement, typename... TOptions>
using ArrayListBase =
    AdaptBasicFieldT<
        comms::field::basic::ArrayList<
            TFieldBase,
            ArrayListStorageTypeT<TElement, OptionsParser<TOptions...> >
        >,
        TOptions...
    >;

}  // namespace details

/// @brief Field that represents a sequential collection of fields.
/// @details By default uses
///     <a href="http://en.cppreference.com/w/cpp/container/vector">std::vector</a>,
///     for internal storage, unless comms::option::FixedSizeStorage option is used,
///     which forces usage of comms::util::StaticVector instead.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam TElement Element of the collection, can be either basic integral value
///     (such as std::uint8_t) or any other field from comms::field namespace.@n
///     For example:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using RawDataSeqField =
///         comms::field::ArrayList<
///             MyFieldBase,
///             std::uint8_t
///         >;
///     using CollectionOfBundlesField =
///         comms::field::ArrayList<
///             MyFieldBase,
///             std::field::Bundle<
///                 MyFieldBase,
///                 std::tuple<
///                     comms::field::IntValue<MyFieldBase, std::uint16_t>
///                     comms::field::IntValue<MyFieldBase, std::uint8_t>
///                     comms::field::IntValue<MyFieldBase, std::uint8_t>
///                 >
///             >
///         >;
///     @endcode
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field.@n
///     Supported options are:
///     @li comms::option::FixedSizeStorage
///     @li comms::option::CustomStorageType
///     @li comms::option::SequenceSizeFieldPrefix
///     @li comms::option::SequenceSerLengthFieldPrefix
///     @li comms::option::SequenceSizeForcingEnabled
///     @li comms::option::SequenceFixedSize
///     @li comms::option::SequenceTerminationFieldSuffix
///     @li comms::option::SequenceTrailingFieldSuffix
///     @li comms::option::DefaultValueInitialiser
///     @li comms::option::ContentsValidator
///     @li comms::option::ContentsRefresher
///     @li comms::option::FailOnInvalid
///     @li comms::option::IgnoreInvalid
/// @extends comms::Field
/// @headerfile comms/field/ArrayList.h
template <typename TFieldBase, typename TElement, typename... TOptions>
class ArrayList : public details::ArrayListBase<TFieldBase, TElement, TOptions...>
{
    using Base = details::ArrayListBase<TFieldBase, TElement, TOptions...>;
public:

    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = details::OptionsParser<TOptions...>;

    /// @brief Tag indicating type of the field
    using Tag = typename std::conditional<
        std::is_integral<TElement>::value,
        tag::RawArrayList,
        tag::ArrayList
    >::type;

    /// @brief Type of underlying value.
    /// @details If comms::option::FixedSizeStorage option is NOT used, the
    ///     ValueType is std::vector<TElement>, otherwise it becomes
    ///     comms::util::StaticVector<TElement, TSize>, where TSize is a size
    ///     provided to comms::option::FixedSizeStorage option.
    using ValueType = typename Base::ValueType;

    /// @brief Default constructor
    ArrayList() = default;

    /// @brief Value constructor
    explicit ArrayList(const ValueType& val)
      : Base(val)
    {
    }

    /// @brief Value constructor
    explicit ArrayList(ValueType&& val)
      : Base(std::move(val))
    {
    }

    /// @brief Copy constructor
    ArrayList(const ArrayList&) = default;

    /// @brief Move constructor
    ArrayList(ArrayList&&) = default;

    /// @brief Destructor
    ~ArrayList() = default;

    /// @brief Copy assignment
    ArrayList& operator=(const ArrayList&) = default;

    /// @brief Move assignment
    ArrayList& operator=(ArrayList&&) = default;

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Get access to the value storage.
    ValueType& value();

    /// @brief Get access to the value storage.
    const ValueType& value() const;

    /// @brief Get length of serialised data
    constexpr std::size_t length() const;

    /// @brief Read field value from input data sequence
    /// @details By default, the read operation will try to consume all the
    ///     data available, unless size limiting option (such as
    ///     comms::option::SequenceSizeFieldPrefix, comms::option::SequenceFixedSize,
    ///     comms::option::SequenceSizeForcingEnabled) is used.
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] len Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len);

    /// @brief Write current field value to output data sequence
    /// @details By default, the write operation will write all the
    ///     elements the field contains. If comms::option::SequenceFixedSize option
    ///     is used, the number of elements, that is going to be written, is
    ///     exactly as the option specifies. If underlying vector storage
    ///     doesn't contain enough data, the default constructed elements will
    ///     be appended to the written sequence until the required amount of
    ///     elements is reached.
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] len Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const;

    /// @brief Check validity of the field value.
    /// @details The collection is valid if all the elements are valid. In case
    ///     comms::option::ContentsValidator option is used, the validator,
    ///     it provides, is invoked IN ADDITION to the validation of the elements.
    /// @return true in case the field's value is valid, false otherwise.
    bool valid() const;

    /// @brief Refresh the field.
    /// @details Calls refresh() on all the elements (if they are fields and not raw bytes).
    /// @brief Returns true if any of the elements has been updated, false otherwise.
    bool refresh();

    /// @brief Get minimal length that is required to serialise field of this type.
    static constexpr std::size_t minLength();

    /// @brief Get maximal length that is required to serialise field of this type.
    static constexpr std::size_t maxLength();

    /// @brief Force number of elements that must be read in the next read()
    ///     invocation.
    /// @details Exists only if comms::option::SequenceSizeForcingEnabled option has been
    ///     used.
    /// @param[in] count Number of elements to read during following read operation.
    void forceReadElemCount(std::size_t count);

    /// @brief Clear forcing of the number of elements that must be read in the next read()
    ///     invocation.
    /// @details Exists only if comms::option::SequenceSizeForcingEnabled option has been
    ///     used.
    void clearReadElemCount();
#endif // #ifdef FOR_DOXYGEN_DOC_ONLY

private:
    static_assert((!ParsedOptions::HasOrigDataView) || (std::is_integral<TElement>::value && (sizeof(TElement) == sizeof(std::uint8_t))),
        "Usage of comms::option::OrigDataView option is allowed only for raw binary data (std::uint8_t) types.");
};

/// @brief Equivalence comparison operator.
/// @details Performs lexicographical compare of two array fields.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case first field is less than second field.
/// @related ArrayList
template <typename... TArgs>
bool operator<(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return std::lexicographical_compare(
                field1.value().begin(), field1.value().end(),
                field2.value().begin(), field2.value().end());
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are NOT equal, false otherwise.
/// @related ArrayList
template <typename... TArgs>
bool operator!=(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return (field1 < field2) || (field2 < field1);
}

/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are equal, false otherwise.
/// @related ArrayList
template <typename... TArgs>
bool operator==(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return !(field1 != field2);
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::ArrayList.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref ArrayList
/// @related comms::field::ArrayList
template <typename T>
constexpr bool isArrayList()
{
    return std::is_same<typename T::Tag, tag::ArrayList>::value;
}

/// @brief Upcast type of the field definition to its parent comms::field::ArrayList type
///     in order to have access to its internal types.
/// @related comms::field::ArrayList
template <typename TFieldBase, typename TElement, typename... TOptions>
inline
ArrayList<TFieldBase, TElement, TOptions...>&
toFieldBase(ArrayList<TFieldBase, TElement, TOptions...>& field)
{
    return field;
}

/// @brief Upcast type of the field definition to its parent comms::field::ArrayList type
///     in order to have access to its internal types.
/// @related comms::field::ArrayList
template <typename TFieldBase, typename TElement, typename... TOptions>
inline
const ArrayList<TFieldBase, TElement, TOptions...>&
toFieldBase(const ArrayList<TFieldBase, TElement, TOptions...>& field)
{
    return field;
}


}  // namespace field

}  // namespace comms

