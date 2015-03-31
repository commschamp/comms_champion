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

#include <type_traits>
#include <array>
#include <algorithm>

#include "comms/ErrorStatus.h"
#include "comms/Assert.h"
#include "comms/util/access.h"

#include "details/StaticStringBase.h"

namespace comms
{

namespace field
{

template <typename TField, typename... TOptions>
class StaticString : public details::StaticStringBase<TField, TOptions...>
{
    typedef details::StaticStringBase<TField, TOptions...> Base;
public:
    typedef char CharType;
    typedef const CharType* Iterator;
    typedef CharType value_type;
    typedef Iterator iterator;

    typedef typename Base::DefaultInitialiser DefaultInitialiser;
    typedef typename Base::SizeValidator SizeValidator;
    typedef typename Base::ContentValidator ContentValidator;

    static const std::size_t SizeLength = Base::SizeLength;

    StaticString()
    {
        DefaultInitialiser initialiser;
        GASSERT(initialiser.size() < StorageSize);
        std::copy_n(initialiser.begin(), initialiser.size(), storage_.begin());
        size_ = initialiser.size();
        endString();
    }

    StaticString(const CharType* value)
    {
        setValue(value);
    }

    StaticString(const StaticString& other)
      : size_(other.size_)
    {
        GASSERT(other.storage_[other.size_] == '\0');
        std::copy_n(other.storage_.begin(), size_ + 1, storage_.begin());
    }

    StaticString& operator=(const StaticString& other)
    {
        if (&other == this) {
            return *this;
        }

        std::copy_n(other.storage_.begin(), other.size_, storage_.begin());
        size_ = other.size_;
        return *this;
    }

    const CharType* getValue() const
    {
        return &storage_[0];
    }

    void setValue(const CharType* value)
    {
        size_ = 0;
        while ((*value != '\0') && (size_ < capacity())) {
            storage_[size_] = *value;
            ++size_;
            ++value;
        }
        endString();
    }

    void clear()
    {
        size_ = 0;
        endString();
    }

    constexpr std::size_t length() const
    {
        return SizeLength + size_;
    }

    constexpr std::size_t size() const
    {
        return size_;
    }

    static constexpr std::size_t capacity()
    {
        return StorageSize - 1;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    constexpr bool valid() const
    {
        return SizeValidator()(size_) && ContentValidator()(begin(), end());
    }

    Iterator begin() const
    {
        return cbegin();
    }

    Iterator cbegin() const
    {
        return &storage_[0];
    }

    Iterator end() const
    {
        return cend();
    }

    Iterator cend() const
    {
        return cbegin() + size_;
    }

    template <typename U>
    void pushBack(U&& ch)
    {
        if (capacity() <= size_) {
            GASSERT(!"Storage capacity overflow");
            return;
        }

        storage_[size_] = std::forward<U>(ch);
        ++size_;
        endString();
    }

    template <typename U>
    void push_back(U&& ch)
    {
        pushBack(std::forward<U>(ch));
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        if (size < SizeLength) {
            return ErrorStatus::NotEnoughData;
        }

        auto len = Base::template readData<std::size_t, SizeLength>(iter);

        if (capacity() < len) {
            return ErrorStatus::InvalidMsgData;
        }

        auto remSize = size - SizeLength;
        if (remSize < len) {
            return ErrorStatus::NotEnoughData;
        }

        std::copy_n(iter, len, storage_.begin());
        std::advance(iter, len);
        size_ = len;
        endString();
        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        Base::template writeData<SizeLength>(size_, iter);
        std::copy_n(storage_.begin(), size_, iter);
        std::advance(iter, size_);

        return ErrorStatus::Success;
    }

private:
    void endString()
    {
        storage_[size_] = '\0';
    }

    static const std::size_t StorageSize = Base::StorageSize;
    typedef std::array<CharType, StorageSize> StorageType;

    StorageType storage_;
    std::size_t size_ = 0;
};

/// @brief Equality comparison operator.
/// @related BasicIntValue
template <typename... TArgs>
bool operator==(
    const StaticString<TArgs...>& field1,
    const StaticString<TArgs...>& field2)
{
    return
        field1.size() == field2.size() &&
        std::equal(field1.begin(), field1.end(), field2.begin());
}

/// @brief Non-equality comparison operator.
/// @related BasicIntValue
template <typename... TArgs>
bool operator!=(
    const StaticString<TArgs...>& field1,
    const StaticString<TArgs...>& field2)
{
    return !(field1 == field2);
}

/// @brief Equivalence comparison operator.
/// @related BasicIntValue
template <typename... TArgs>
bool operator<(
    const StaticString<TArgs...>& field1,
    const StaticString<TArgs...>& field2)
{
    return std::lexicographical_compare(field1.begin(), field1.end(), field2.begin(), field2.end());
}

namespace details
{

template <typename T>
struct IsStaticString
{
    static const bool Value = false;
};

template <typename... TArgs>
struct IsStaticString<comms::field::StaticString<TArgs...> >
{
    static const bool Value = true;
};

}  // namespace details

template <typename T>
constexpr bool isStaticString()
{
    return details::IsStaticString<T>::Value;
}


}  // namespace field

}  // namespace comms


