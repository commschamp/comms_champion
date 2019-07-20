//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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
#include "comms/util/StaticString.h"
#include "comms/util/StringView.h"
#include "basic/String.h"
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
struct StringOrigDataViewStorageType;

template <>
struct StringOrigDataViewStorageType<true>
{
    using Type = comms::util::StringView;
};

template <>
struct StringOrigDataViewStorageType<false>
{
    using Type = std::string;
};

template <bool THasSequenceFixedSizeUseFixedSizeStorage>
struct StringFixedSizeUseFixedSizeStorageType;

template <>
struct StringFixedSizeUseFixedSizeStorageType<true>
{
    template <typename TOpt>
    using Type = comms::util::StaticString<TOpt::SequenceFixedSize>;
};

template <>
struct StringFixedSizeUseFixedSizeStorageType<false>
{
    template <typename TOpt>
    using Type = typename StringOrigDataViewStorageType<TOpt::HasOrigDataView>::Type;
};

template <bool THasFixedSizeStorage>
struct StringFixedSizeStorageType;

template <>
struct StringFixedSizeStorageType<true>
{
    template <typename TOpt>
    using Type = comms::util::StaticString<TOpt::FixedSizeStorage>;
};

template <>
struct StringFixedSizeStorageType<false>
{
    template <typename TOpt>
    using Type = typename StringFixedSizeUseFixedSizeStorageType<TOpt::HasSequenceFixedSizeUseFixedSizeStorage>
        ::template Type<TOpt>;
};

template <bool THasCustomStorage>
struct StringCustomStringStorageType;

template <>
struct StringCustomStringStorageType<true>
{
    template <typename TOpt>
    using Type = typename TOpt::CustomStorageType;
};

template <>
struct StringCustomStringStorageType<false>
{
    template <typename TOpt>
    using Type =
        typename StringFixedSizeStorageType<TOpt::HasFixedSizeStorage>::template Type<TOpt>;
};

template <typename TOpt>
using StringStorageTypeT =
    typename StringCustomStringStorageType<TOpt::HasCustomStorageType>::template Type<TOpt>;

template <typename TFieldBase, typename... TOptions>
using StringBase =
    AdaptBasicFieldT<
        basic::String<TFieldBase, StringStorageTypeT<OptionsParser<TOptions...> > >,
        TOptions...
    >;

} // namespace details

/// @brief Field that represents a string.
/// @details By default uses
///     <a href="http://en.cppreference.com/w/cpp/string/basic_string">std::string</a>,
///     for internal storage, unless @ref comms::option::app::FixedSizeStorage option is used,
///     which forces usage of comms::util::StaticString instead.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field.@n
///     Supported options are:
///     @li @ref comms::option::app::FixedSizeStorage
///     @li @ref comms::option::app::CustomStorageType
///     @li @ref comms::option::app::OrigDataView
///     @li @ref comms::option::def::SequenceSizeFieldPrefix
///     @li @ref comms::option::def::SequenceSerLengthFieldPrefix
///     @li @ref comms::option::def::SequenceSizeForcingEnabled
///     @li @ref comms::option::def::SequenceLengthForcingEnabled
///     @li @ref comms::option::def::SequenceFixedSize
///     @li @ref comms::option::def::SequenceTerminationFieldSuffix
///     @li @ref comms::option::def::SequenceTrailingFieldSuffix
///     @li @ref comms::option::def::DefaultValueInitialiser
///     @li @ref comms::option::def::ContentsValidator
///     @li @ref comms::option::def::ContentsRefresher
///     @li @ref comms::option::def::HasCustomRead
///     @li @ref comms::option::def::HasCustomRefresh
///     @li @ref comms::option::def::FailOnInvalid
///     @li @ref comms::option::def::IgnoreInvalid
///     @li @ref comms::option::def::EmptySerialization
///     @li @ref comms::option::def::InvalidByDefault
///     @li @ref comms::option::def::VersionStorage
/// @extends comms::Field
/// @headerfile comms/field/String.h
template <typename TFieldBase, typename... TOptions>
class String : private details::StringBase<TFieldBase, TOptions...>
{
    using BaseImpl = details::StringBase<TFieldBase, TOptions...>;
public:

    /// @brief Endian used for serialisation.
    using Endian = typename BaseImpl::Endian;

    /// @brief Version type
    using VersionType = typename BaseImpl::VersionType;

    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = details::OptionsParser<TOptions...>;

    /// @brief Tag indicating type of the field
    using Tag = tag::String;

    /// @brief Type of underlying value.
    /// @details If @ref comms::option::app::FixedSizeStorage option is NOT used, the
    ///     ValueType is std::string, otherwise it becomes
    ///     comms::util::StaticString<TSize>, where TSize is a size
    ///     provided to @ref comms::option::app::FixedSizeStorage option.
    using ValueType = typename BaseImpl::ValueType;

    /// @brief Default constructor
    String() = default;

    /// @brief Constructor
    explicit String(const ValueType& val)
      : BaseImpl(val)
    {
    }

    /// @brief Constructor
    explicit String(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    /// @brief Constructor
    explicit String(const char* str)
    {
        BaseImpl::value() = str;
    }

    /// @brief Copy constructor
    String(const String&) = default;

    /// @brief Move constructor
    String(String&&) = default;

    /// @brief Destructor
    ~String() noexcept = default;

    /// @brief Copy assignment
    String& operator=(const String&) = default;

    /// @brief Move assignment
    String& operator=(String&&) = default;

    /// @brief Read field value from input data sequence
    /// @details By default, the read operation will try to consume all the
    ///     data available, unless size limiting option (such as
    ///     @ref comms::option::def::SequenceSizeFieldPrefix, @ref comms::option::def::SequenceFixedSize,
    ///     @ref comms::option::def::SequenceSizeForcingEnabled,
    ///     @ref comms::option::def::SequenceLengthForcingEnabled) is used.
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] len Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto es = BaseImpl::read(iter, len);
        using TagTmp = typename std::conditional<
            ParsedOptions::HasSequenceFixedSize,
            AdjustmentNeededTag,
            NoAdjustmentTag
        >::type;

        adjustValue(TagTmp());
        return es;
    }

    /// @brief Read field value from input data sequence without error check and status report.
    /// @details Similar to @ref read(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to read the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        BaseImpl::readNoStatus(iter);
        using TagTmp = typename std::conditional<
            ParsedOptions::HasSequenceFixedSize,
            AdjustmentNeededTag,
            NoAdjustmentTag
        >::type;

        adjustValue(TagTmp());
    }

    /// @brief Get access to the value storage.
    ValueType& value()
    {
        return BaseImpl::value();
    }

    /// @brief Get access to the value storage.
    const ValueType& value() const
    {
        return BaseImpl::value();
    }

    /// @brief Get length of serialised data
    std::size_t length() const
    {
        return BaseImpl::length();
    }

    /// @brief Check validity of the field value.
    bool valid() const
    {
        return BaseImpl::valid();
    }

    /// @brief Refresh the field's value
    /// @return @b true if the value has been updated, @b false otherwise
    bool refresh()
    {
        return BaseImpl::refresh();
    }

    /// @brief Write current field value to output data sequence
    /// @details By default, the write operation will write all the
    ///     characters the field contains. If @ref comms::option::def::SequenceFixedSize option
    ///     is used, the number of characters, that is going to be written, is
    ///     exactly as the option specifies. If underlying string storage
    ///     doesn't contain enough data, the '\0' characters will
    ///     be appended to the written sequence until the required amount of
    ///     elements is reached.
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] len Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return BaseImpl::write(iter, len);
    }

    /// @brief Write current field value to output data sequence  without error check and status report.
    /// @details Similar to @ref write(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to write the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        BaseImpl::writeNoStatus(iter);
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    static constexpr std::size_t minLength()
    {
        return BaseImpl::minLength();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    static constexpr std::size_t maxLength()
    {
        return BaseImpl::maxLength();
    }

    /// @brief Force number of characters that must be read in the next read()
    ///     invocation.
    /// @details Exists only if @ref comms::option::def::SequenceSizeForcingEnabled option has been
    ///     used.
    /// @param[in] count Number of elements to read during following read operation.
    void forceReadElemCount(std::size_t count)
    {
        BaseImpl::forceReadElemCount(count);
    }

    /// @brief Clear forcing of the number of characters that must be read in
    ///     the next read() invocation.
    /// @details Exists only if @ref comms::option::def::SequenceSizeForcingEnabled option has been
    ///     used.
    void clearReadElemCount()
    {
        BaseImpl::clearReadElemCount();
    }

    /// @brief Force available length for the next read() invocation.
    /// @details Exists only if @ref comms::option::def::SequenceLengthForcingEnabled option has been
    ///     used.
    /// @param[in] count Number of elements to read during following read operation.
    void forceReadLength(std::size_t count)
    {
        return BaseImpl::forceReadLength(count);
    }

    /// @brief Clear forcing of the available length in the next read()
    ///     invocation.
    /// @details Exists only if @ref comms::option::def::SequenceLengthForcingEnabled option has been
    ///     used.
    void clearReadLengthForcing()
    {
        return BaseImpl::clearReadLengthForcing();
    }

    /// @brief Compile time check if this class is version dependent
    static constexpr bool isVersionDependent()
    {
        return ParsedOptions::HasCustomVersionUpdate || BaseImpl::isVersionDependent();
    }

    /// @brief Compile time check if this class has non-default refresh functionality
    static constexpr bool hasNonDefaultRefresh()
    {
        return BaseImpl::hasNonDefaultRefresh();
    }

    /// @brief Get version of the field.
    /// @details Exists only if @ref comms::option::def::VersionStorage option has been provided.
    VersionType getVersion() const
    {
        return BaseImpl::getVersion();
    }

    /// @brief Default implementation of version update.
    /// @return @b true in case the field contents have changed, @b false otherwise
    bool setVersion(VersionType version)
    {
        return BaseImpl::setVersion(version);
    }

protected:
    using BaseImpl::readData;
    using BaseImpl::writeData;

private:
    struct NoAdjustmentTag {};
    struct AdjustmentNeededTag {};
    struct HasResizeTag {};
    struct HasRemoveSuffixTag {};

    void adjustValue(NoAdjustmentTag)
    {
    }

    void adjustValue(AdjustmentNeededTag)
    {
        std::size_t count = 0;
        for (auto iter = BaseImpl::value().begin(); iter != BaseImpl::value().end(); ++iter) {
            if (*iter == 0) {
                break;
            }
            ++count;
        }

        doResize(count);
    }

    void doResize(std::size_t count)
    {
        using TagTmp =
            typename std::conditional<
                comms::details::hasResizeFunc<ValueType>(),
                HasResizeTag,
                typename std::conditional<
                    comms::details::hasRemoveSuffixFunc<ValueType>(),
                    HasRemoveSuffixTag,
                    void
                >::type
            >::type;

        static_assert(!std::is_void<Tag>::value,
            "The string storage value type must have either resize() or remove_suffix() "
            "member functions");
        doResize(count, TagTmp());
    }

    void doResize(std::size_t count, HasResizeTag)
    {
        BaseImpl::value().resize(count);
    }

    void doResize(std::size_t count, HasRemoveSuffixTag)
    {
        BaseImpl::value().remove_suffix(BaseImpl::value().size() - count);
    }

    static_assert(!ParsedOptions::HasSerOffset,
        "comms::option::def::NumValueSerOffset option is not applicable to String field");
    static_assert(!ParsedOptions::HasFixedLengthLimit,
        "comms::option::def::FixedLength option is not applicable to String field");
    static_assert(!ParsedOptions::HasFixedBitLengthLimit,
        "comms::option::def::FixedBitLength option is not applicable to String field");
    static_assert(!ParsedOptions::HasVarLengthLimits,
        "comms::option::def::VarLength option is not applicable to String field");
    static_assert(!ParsedOptions::HasScalingRatio,
        "comms::option::def::ScalingRatio option is not applicable to String field");
    static_assert(!ParsedOptions::HasUnits,
        "comms::option::def::Units option is not applicable to String field");
    static_assert(!ParsedOptions::HasMultiRangeValidation,
        "comms::option::def::ValidNumValueRange (or similar) option is not applicable to String field");
    static_assert(!ParsedOptions::HasSequenceElemSerLengthFieldPrefix,
        "comms::option::def::SequenceElemSerLengthFieldPrefix option is not applicable to String field");
    static_assert(!ParsedOptions::HasSequenceElemFixedSerLengthFieldPrefix,
        "comms::option::def::SequenceElemSerLengthFixedFieldPrefix option is not applicable to String field");
    static_assert(!ParsedOptions::HasVersionsRange,
        "comms::option::def::ExistsBetweenVersions (or similar) option is not applicable to String field");
};

/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are equal, false otherwise.
/// @related String
template <typename TFieldBase, typename... TOptions>
bool operator==(
    const String<TFieldBase, TOptions...>& field1,
    const String<TFieldBase, TOptions...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are NOT equal, false otherwise.
/// @related String
template <typename TFieldBase, typename... TOptions>
bool operator!=(
    const String<TFieldBase, TOptions...>& field1,
    const String<TFieldBase, TOptions...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @details Performs lexicographical compare of two string values.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case first field is less than second field.
/// @related String
template <typename TFieldBase, typename... TOptions>
bool operator<(
    const String<TFieldBase, TOptions...>& field1,
    const String<TFieldBase, TOptions...>& field2)
{
    return field1.value() < field2.value();
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::String.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref String
/// @related comms::field::String
template <typename T>
constexpr bool isString()
{
    return std::is_same<typename T::Tag, tag::String>::value;
}

/// @brief Upcast type of the field definition to its parent comms::field::String type
///     in order to have access to its internal types.
/// @related comms::field::String
template <typename TFieldBase, typename... TOptions>
inline
String<TFieldBase, TOptions...>&
toFieldBase(String<TFieldBase, TOptions...>& field)
{
    return field;
}

/// @brief Upcast type of the field definition to its parent comms::field::String type
///     in order to have access to its internal types.
/// @related comms::field::String
template <typename TFieldBase, typename... TOptions>
inline
const String<TFieldBase, TOptions...>&
toFieldBase(const String<TFieldBase, TOptions...>& field)
{
    return field;
}

}  // namespace field

}  // namespace comms


