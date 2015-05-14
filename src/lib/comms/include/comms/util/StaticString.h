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

#include <algorithm>
#include <iterator>

#include "comms/Assert.h"
#include "StaticQueue.h"

namespace comms
{

namespace util
{

template <typename std::size_t TSize>
class StaticString
{
    typedef StaticQueue<char, TSize> StorageType;

public:
    typedef typename StorageType::value_type value_type;
    typedef std::size_t size_type;
    typedef typename StorageType::reference reference;
    typedef typename StorageType::const_reference const_reference;
    typedef typename StorageType::pointer pointer;
    typedef typename StorageType::const_pointer const_pointer;
    typedef typename StorageType::LinearisedIterator iterator;
    typedef typename StorageType::ConstLinearisedIterator const_iterator;

    StaticString()
    {
        endString();
    }

    StaticString(const char* str)
    {
        copyString(str);
    }

    StaticString(size_type count, char ch)
    {
        assign(count, ch);
    }

    StaticString(const StaticString& other, size_type pos, size_type count)
    {
        assign(other, pos, count);
    }

    StaticString(const char* other, size_type count)
    {
        assign(other, count);
    }

    template <typename TIter>
    StaticString(TIter first, TIter last)
    {
        assign(first, last);
    }

    StaticString(const StaticString&) = default;
    StaticString(StaticString&&) = default;
    ~StaticString() = default;

    StaticString& operator=(const StaticString&) = default;
    StaticString& operator=(StaticString&&) = default;

    StaticString& operator=(const char* str)
    {
        str_.clear();
        copyString(str);
        return *this;
    }

    StaticString& operator=(char ch)
    {
        *this = StaticString(1U, ch);
        return *this;
    }

    StaticString& assign(size_type count, char ch)
    {
        clear();
        while (0 < count) {
            str_.push_back(ch);
            --count;
        }
        endString();
        return *this;
    }

    StaticString& assign(const StaticString& other)
    {
        return operator=(other);
    }

    StaticString& assign(StaticString&& other)
    {
        return operator=(std::move(other));
    }

    StaticString& assign(const StaticString& other, size_type pos, size_type count)
    {
        clear();
        while (0 < count) {
            if (other.size() <= pos) {
                break;
            }

            str_.push_back(other[pos]);
            ++pos;
            --count;
        }
        endString();
        return *this;
    }

    StaticString& assign(const char* other, size_type count)
    {
        clear();
        for (size_type idx = 0; idx < count; ++idx) {
            str_.push_back(other[idx]);
        }
        endString();
        return *this;
    }

    StaticString& assign(const char* other)
    {
        return operator=(other);
    }

    template <typename TIter>
    StaticString& assign(TIter from, TIter to)
    {
        clear();
        std::copy(from, to, std::back_inserter(str_));
        endString();
        return *this;
    }

    reference operator[](size_type pos)
    {
        return str_[pos];
    }

    const_reference operator[](size_type pos) const
    {
        return str_[pos];
    }

    const char& front() const
    {
        return str_[0];
    }

    const char& back() const
    {
        return str_[size() - 1];
    }

    const char* data() const
    {
        return &str_[0];
    }

    const char* c_str() const
    {
        return &str_[0];
    }

    iterator begin()
    {
        return str_.lbegin();
    }

    const_iterator begin() const
    {
        return str_.lbegin();
    }

    const_iterator cbegin() const
    {
        return str_.clbegin();
    }

    iterator end()
    {
        return str_.lend() - 1;
    }

    const_iterator end() const
    {
        return str_.lend() - 1;
    }

    const_iterator cend() const
    {
        return str_.clend() - 1;
    }

    bool empty() const
    {
        return size() == 0;
    }

    size_type size() const
    {
        return str_.size() - 1;
    }

    size_type length() const
    {
        return size();
    }

    size_type max_size() const
    {
        return str_.capacity() - 1;
    }

    void reserve(size_type)
    {
    }

    size_type capacity() const
    {
        return max_size();
    }

    void shrink_to_fit()
    {
    }

    void clear()
    {
        str_.clear();
        endString();
    }

    void push_back(char ch)
    {
        str_.back() = ch;
        endString();
        GASSERT(str_.isLinearised());
    }

    void pop_back()
    {
        str_.popBack();
        str_.popBack();
        endString();
    }

private:
    void copyString(const char* str)
    {
        while ((*str != '\0') && (str_.size() < max_size())) {
            str_.pushBack(*str);
            ++str;
        }
        endString();
    }

    void endString()
    {
        str_.pushBack('\0');
    }

    StorageType str_;
};

template <std::size_t TSize1, std::size_t TSize2>
bool operator<(const StaticString<TSize1>& str1, const StaticString<TSize2>& str2)
{
    return std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end());
}

template <std::size_t TSize1, std::size_t TSize2>
bool operator<=(const StaticString<TSize1>& str1, const StaticString<TSize2>& str2)
{
    return !(str2 < str1);
}

template <std::size_t TSize1, std::size_t TSize2>
bool operator>(const StaticString<TSize1>& str1, const StaticString<TSize2>& str2)
{
    return (str2 < str1);
}

template <std::size_t TSize1, std::size_t TSize2>
bool operator>=(const StaticString<TSize1>& str1, const StaticString<TSize2>& str2)
{
    return !(str1 < str2);
}

template <std::size_t TSize1, std::size_t TSize2>
bool operator==(const StaticString<TSize1>& str1, const StaticString<TSize2>& str2)
{
    return
        (str1.size() == str2.size()) &&
        std::equal(str1.begin(), str1.end(), str2.begin());
}

}  // namespace util

}  // namespace comms


