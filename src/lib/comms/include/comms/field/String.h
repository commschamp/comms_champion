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

#include <vector>

#include "comms/ErrorStatus.h"
#include "comms/options.h"
#include "comms/util/StaticString.h"
#include "basic/ArrayList.h"
#include "details/AdaptBasicField.h"
#include "details/OptionsParser.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TOptions, bool THasFixedStorage>
struct StringStorageType;

template <typename TOptions>
struct StringStorageType<TOptions, true>
{
    typedef comms::util::StaticString<TOptions::FixedSizeStorage> Type;
};

template <typename TOptions>
struct StringStorageType<TOptions, false>
{
    typedef std::string Type;
};

template <typename TOptions>
using StringStorageTypeT =
    typename StringStorageType<TOptions, TOptions::HasFixedSizeStorage>::Type;

} // namespace details

/// @brief Field that represents a string.
/// @details By default uses
///     <a href="http://en.cppreference.com/w/cpp/string/basic_string">std::string</a>,
///     for internal storage, unless comms::option::FixedSizeStorage option is used,
///     which forces usage of comms::util::StaticString instead.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field.@n
///     Supported options are:
///     @li comms::option::FixedSizeStorage
///     @li comms::option::SequenceSizeFieldPrefix
///     @li comms::option::SequenceSizeForcingEnabled
///     @li comms::option::SequenceFixedSize
///     @li comms::option::SequenceTerminationFieldSuffix
///     @li comms::option::SequenceTrailingFieldSuffix
///     @li comms::option::DefaultValueInitialiser
///     @li comms::option::ContentsValidator
///     @li comms::option::FailOnInvalid
///     @li comms::option::IgnoreInvalid
template <typename TFieldBase, typename... TOptions>
class String : public TFieldBase
{
    typedef TFieldBase Base;

    typedef details::OptionsParser<TOptions...> ParsedOptionsInternal;
    using StorageTypeInternal =
        details::StringStorageTypeT<ParsedOptionsInternal>;
    typedef basic::ArrayList<TFieldBase, StorageTypeInternal> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;

public:

    /// @brief All the options provided to this class bundled into struct.
    typedef ParsedOptionsInternal ParsedOptions;

    /// @brief Type of underlying value.
    /// @details If comms::option::FixedSizeStorage option is NOT used, the
    ///     ValueType is std::string, otherwise it becomes
    ///     comms::util::StaticString<TSize>, where TSize is a size
    ///     provided to comms::option::FixedSizeStorage option.
    typedef StorageTypeInternal ValueType;

    /// @brief Default constructor
    String() = default;

    /// @brief Constructor
    explicit String(const ValueType& val)
      : str_(val)
    {
    }

    /// @brief Constructor
    explicit String(ValueType&& val)
      : str_(std::move(val))
    {
    }

    /// @brief Constructor
    explicit String(const char* str)
    {
        str_.value() = str;
    }

    /// @brief Copy constructor
    String(const String&) = default;

    /// @brief Move constructor
    String(String&&) = default;

    /// @brief Destructor
    ~String() = default;

    /// @brief Copy assignment
    String& operator=(const String&) = default;

    /// @brief Move assignment
    String& operator=(String&&) = default;

    /// @brief Get access to the value storage.
    ValueType& value()
    {
        return str_.value();
    }

    /// @brief Get access to the value storage.
    const ValueType& value() const
    {
        return str_.value();
    }

    /// @brief Get length of serialised data
    constexpr std::size_t length() const
    {
        return str_.length();
    }

    /// @brief Check validity of the field value.
    constexpr bool valid() const
    {
        return str_.valid();
    }

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
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto es = str_.read(iter, len);
        adjustValue(AdjustmentTag());
        return es;
    }

    /// @brief Write current field value to output data sequence
    /// @details By default, the write operation will write all the
    ///     characters the field contains. If comms::option::SequenceFixedSize option
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
        return str_.write(iter, len);
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

    /// @brief Force number of characters that must be read in the next read()
    ///     invocation.
    /// @details If comms::option::SequenceSizeForcingEnabled option hasn't been
    ///     used this function has no effect.
    void forceReadElemCount(std::size_t count)
    {
        str_.forceReadElemCount(count);
    }

    /// @brief Clear forcing of the number of characters that must be read in
    ///     the next read() invocation.
    /// @details If comms::option::SequenceSizeForcingEnabled option hasn't been
    ///     used this function has no effect.
    void clearReadElemCount()
    {
        str_.clearReadElemCount();
    }

private:
    struct NoAdjustment {};
    struct AdjustmentNeeded {};
    typedef typename std::conditional<
        ParsedOptions::HasSequenceFixedSize,
        AdjustmentNeeded,
        NoAdjustment
    >::type AdjustmentTag;

    void adjustValue(NoAdjustment)
    {
    }

    void adjustValue(AdjustmentNeeded)
    {
        std::size_t count = 0;
        for (auto iter = value().begin(); iter != value().end(); ++iter) {
            if (*iter == 0) {
                break;
            }
            ++count;
        }

        value().resize(count);
    }

    ThisField str_;
};

/// @brief Equality comparison operator.
/// @related String
template <typename... TArgs>
bool operator==(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @related String
template <typename... TArgs>
bool operator!=(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @related String
template <typename... TArgs>
bool operator<(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.value() < field2.value();
}

namespace details
{

template <typename T>
struct IsString
{
    static const bool Value = false;
};

template <typename TFieldBase, typename... TOptions>
struct IsString<comms::field::String<TFieldBase, TOptions...> >
{
    static const bool Value = true;
};

}  // namespace details

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::String.
/// @tparam T Any type.
/// @related comms::field::String
template <typename T>
constexpr bool isString()
{
    return details::IsString<T>::Value;
}


}  // namespace field

}  // namespace comms


