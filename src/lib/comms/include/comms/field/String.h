//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

    typedef ParsedOptionsInternal ParsedOptions;
    typedef StorageTypeInternal StorageType;

    String() = default;

    explicit String(const StorageType& fields)
      : str_(fields)
    {
    }

    explicit String(const char* str)
    {
        str_.getValue() = str;
    }

    String(const String&) = default;

    String(String&&) = default;

    ~String() = default;

    String& operator=(const String&) = default;

    String& operator=(String&&) = default;

    String& operator=(const char* str)
    {
        str_.getValue() = str;
        return *this;
    }

    StorageType& string()
    {
        return str_.getValue();
    }

    const StorageType& string() const
    {
        return str_.getValue();
    }

    const StorageType& getValue() const
    {
        return str_.getValue();
    }

    void setValue(const StorageType& value)
    {
        string() = value;
    }

    constexpr std::size_t length() const
    {
        return str_.length();
    }

    constexpr bool valid() const
    {
        return str_.valid();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        return str_.read(iter, len);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return str_.write(iter, len);
    }

private:

    ThisField str_;
};

/// @brief Equality comparison operator.
/// @related String
template <typename... TArgs>
bool operator==(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.string() == field2.string();
}

/// @brief Non-equality comparison operator.
/// @related String
template <typename... TArgs>
bool operator!=(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.string() != field2.string();
}

/// @brief Equivalence comparison operator.
/// @related String
template <typename... TArgs>
bool operator<(
    const String<TArgs...>& field1,
    const String<TArgs...>& field2)
{
    return field1.string() < field2.string();
}

namespace details
{

template <typename T>
struct IsString
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsString<comms::field::String<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isString()
{
    return details::IsString<T>::Value;
}


}  // namespace field

}  // namespace comms


