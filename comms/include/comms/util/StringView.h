//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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
#include <limits>

#include "comms/Assert.h"

namespace comms
{

namespace util
{

namespace details
{

template <typename TChar = char>
class StringViewBase
{
public:
    using ValueType = TChar;
    using value_type = ValueType;
    using Pointer = TChar*;
    using pointer = Pointer;
    using ConstPointer = const TChar*;
    using const_pointer = ConstPointer;
    using Reference = const TChar&;
    using reference = Reference;
    using ConstReference = const TChar&;
    using const_reference = ConstReference;
    using SizeType = std::size_t;
    using size_type = SizeType;

    using ConstIterator = const TChar*;
    using const_iterator = ConstIterator;
    using Iterator = ConstIterator;
    using iterator = Iterator;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
    using const_reverse_iterator = ConstReverseIterator;
    using ReverseIterator = ConstReverseIterator;
    using reverse_iterator = ReverseIterator;

    static const auto npos = static_cast<SizeType>(-1);

    StringViewBase() noexcept = default;
    StringViewBase(const StringViewBase&) noexcept = default;
    StringViewBase(const TChar* str, SizeType len)
      : str_(str),
        len_(len)
    {
    }

    StringViewBase(const TChar* str) noexcept
      : str_(str)
    {
        static const auto MaxLen = std::numeric_limits<SizeType>::max();
        while ((len_ < MaxLen) && (str[len_] != '\0')) {
            ++len_;
        }
    }

    ~StringViewBase() = default;

    StringViewBase& operator=(const StringViewBase&) = default;

    constexpr Iterator begin() const noexcept
    {
        return str_;
    }

    constexpr ConstIterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr Iterator end() const noexcept
    {
        return str_ + len_;
    }

    constexpr ConstIterator cend() const noexcept
    {
        return end();
    }

    constexpr ReverseIterator rbegin() const noexcept
    {
        return std::reverse_iterator<Iterator>(end());
    }

    constexpr ConstReverseIterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr ReverseIterator rend() const noexcept
    {
        return std::reverse_iterator<Iterator>(begin());
    }

    constexpr ConstReverseIterator crend() const noexcept
    {
        return rend();
    }

    constexpr const_reference operator[](SizeType pos) const
    {
        return str_[pos];
    }

    constexpr const_reference at(SizeType pos) const
    {
        GASSERT(pos < len_);
        return str_[pos];
    }

    constexpr const_reference front() const
    {
        return str_[0];
    }

    constexpr const_reference back() const
    {
        return str_[len_ - 1U];
    }

    constexpr const_pointer data() const noexcept
    {
        return str_;
    }

    constexpr size_type size() const noexcept
    {
        return len_;
    }

    constexpr size_type length() const noexcept
    {
        return size();
    }

    constexpr bool empty() const noexcept
    {
        return size() == 0U;
    }

    void remove_prefix(size_type n)
    {
        std::advance(str_, n);
        len_ -= n;
    }

    void remove_suffix(size_type n)
    {
        len_ -= n;
    }

    void swap(StringViewBase& other) noexcept
    {
        std::swap(str_, other.str_);
        std::swap(len_, other.len_);
    }

    SizeType copy(TChar* dest, SizeType count, SizeType pos = 0) const
    {
        if (len_ <= pos) {
            return 0U;
        }

        auto toCopy = std::min(count, len_ - pos);
        std::copy_n(str_ + pos, toCopy, dest);
        return toCopy;
    }

    int compare(const StringViewBase& other) const
    {
        return compare(0, len_, other);
    }

    int compare(SizeType pos, SizeType count, const StringViewBase& other) const
    {
        return compare(pos, count, other, 0, other.len_);
    }

    int compare(
        SizeType pos1,
        SizeType count1,
        const StringViewBase& other,
        SizeType pos2,
        SizeType count2) const
    {
        GASSERT(pos1 <= size());
        GASSERT(pos2 <= other.size());
        count1 = std::min(count1, size() - pos1);
        count2 = std::min(count2, other.size() - pos2);
        auto minCount = std::min(count1, count2);
        for (auto idx = 0U; idx < minCount; ++idx) {
            auto thisCh = (*this)[pos1 + idx];
            auto otherCh = other[pos2 + idx];
            auto diff = static_cast<int>(thisCh) - static_cast<int>(otherCh);
            if (diff != 0) {
                return diff;
            }
        }

        return static_cast<int>(count1) - static_cast<int>(count2);
    }

    int compare(const TChar* s) const
    {
        return compare(0U, len_, s);
    }

    int compare(SizeType pos, SizeType count, const TChar* s) const
    {
        return compare(pos, count, s, std::numeric_limits<SizeType>::max());
    }

    int compare(SizeType pos1, SizeType count1, const TChar* s, SizeType count2) const
    {
        return compare(pos1, count1, StringViewBase(s, count2));
    }

    SizeType find_first_of(const StringViewBase& other, SizeType pos = 0)
    {
        if (empty() || (size() <= pos)) {
            return npos;
        }

        pos = std::min(pos, size() - 1);
        for (auto iter = cbegin() + pos; iter != cend(); ++iter) {
            auto foundIter = std::find(other.cbegin(), other.cend(), *iter);
            if (foundIter != other.cend()) {
                return static_cast<SizeType>(std::distance(cbegin(), iter));
            }
        }

        return npos;
    }

    SizeType find_first_of(TChar c, SizeType pos = 0)
    {
        return find_first_of(StringViewBase(&c, 1), pos);
    }

    SizeType find_first_of(const TChar* str, SizeType pos, SizeType count)
    {
        return find_first_of(StringViewBase(str, count), pos);
    }

    SizeType find_first_of(const TChar* str, SizeType pos = 0)
    {
        return find_first_of(StringViewBase(str), pos);
    }

    SizeType find_last_of(const StringViewBase& other, SizeType pos = 0)
    {
        if (empty() || (size() <= pos)) {
            return npos;
        }

        auto begIter = std::reverse_iterator<const TChar*>(cbegin() + pos + 1);
        auto endIter = std::reverse_iterator<const TChar*>(cbegin());
        for (auto iter = begIter; iter != endIter; ++iter) {
            auto foundIter = std::find(other.cbegin(), other.cend(), *iter);
            if (foundIter != other.cend()) {
                return static_cast<std::size_t>(std::distance(iter, endIter)) - 1U;
            }
        }

        return npos;
    }

    SizeType find_last_of(TChar c, SizeType pos = 0)
    {
        return find_last_of(StringViewBase(&c, 1), pos);
    }

    SizeType find_last_of(const TChar* str, SizeType pos, SizeType count)
    {
        return find_last_of(StringViewBase(str, count), pos);
    }

    SizeType find_last_of(const TChar* str, SizeType pos = 0)
    {
        return find_last_of(StringViewBase(str), pos);
    }

    SizeType find_first_not_of(const StringViewBase& other, SizeType pos = 0)
    {
        if (empty() || (size() <= pos)) {
            return npos;
        }

        pos = std::min(pos, size() - 1);
        for (auto iter = cbegin() + pos; iter != cend(); ++iter) {
            auto foundIter = std::find(other.cbegin(), other.cend(), *iter);
            if (foundIter == other.cend()) {
                return static_cast<SizeType>(std::distance(cbegin(), iter));
            }
        }

        return npos;
    }

    SizeType find_first_not_of(TChar c, SizeType pos = 0)
    {
        return find_first_not_of(StringViewBase(&c, 1), pos);
    }

    SizeType find_first_not_of(const TChar* str, SizeType pos, SizeType count)
    {
        return find_first_not_of(StringViewBase(str, count), pos);
    }

    SizeType find_first_not_of(const TChar* str, SizeType pos = 0)
    {
        return find_first_not_of(StringViewBase(str), pos);
    }

    SizeType find_last_not_of(const StringViewBase& other, SizeType pos = 0)
    {
        if (empty() || (size() <= pos)) {
            return npos;
        }

        auto begIter = std::reverse_iterator<const TChar*>(cbegin() + pos + 1);
        auto endIter = std::reverse_iterator<const TChar*>(cbegin());
        for (auto iter = begIter; iter != endIter; ++iter) {
            auto foundIter = std::find(other.cbegin(), other.cend(), *iter);
            if (foundIter == other.cend()) {
                return static_cast<std::size_t>(std::distance(iter, endIter)) - 1U;
            }
        }

        return npos;
    }

    SizeType find_last_not_of(TChar c, SizeType pos = 0)
    {
        return find_last_not_of(StringViewBase(&c, 1), pos);
    }

    SizeType find_last_not_of(const TChar* str, SizeType pos, SizeType count)
    {
        return find_last_not_of(StringViewBase(str, count), pos);
    }

    SizeType find_last_not_of(const TChar* str, SizeType pos = 0)
    {
        return find_last_not_of(StringViewBase(str), pos);
    }

private:
    const TChar* str_ = nullptr;
    std::size_t len_ = 0;
};

} // namespace details

class StringView : public details::StringViewBase<>
{
    using Base = details::StringViewBase<>;
public:
    using size_type = Base::size_type;

    StringView() noexcept = default;
    StringView(const StringView&) noexcept = default;
    StringView(const char* str, SizeType len) noexcept : Base(str, len) {}
    StringView(const char* str) noexcept : Base(str) {}

    ~StringView() = default;
    StringView& operator=(const StringView&) = default;

    void remove_suffix(size_type len)
    {
        Base::remove_suffix(len);
    }
};

/// @brief Lexicographical compare between the strings.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator<(const StringView& str1, const StringView& str2)
{
    return std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end());
}

/// @brief Lexicographical compare between the strings.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator<=(const StringView& str1, const StringView& str2)
{
    return !(str2 < str1);
}

/// @brief Lexicographical compare between the strings.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator>(const StringView& str1, const StringView& str2)
{
    return (str2 < str1);
}

/// @brief Lexicographical compare between the strings.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator>=(const StringView& str1, const StringView& str2)
{
    return !(str1 < str2);
}

/// @brief Equality compare between the strings.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator==(const StringView& str1, const StringView& str2)
{
    return
        (str1.size() == str2.size()) &&
        std::equal(str1.begin(), str1.end(), str2.begin());
}

/// @brief Inequality compare between the strings.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator!=(const StringView& str1, const StringView& str2)
{
    return !(str1 == str2);
}


} // namespace util

} // namespace comms

namespace std
{

/// @brief Specializes the std::swap algorithm.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/swap2">Reference</a>
/// @related comms::util::StringView
inline
void swap(comms::util::StringView& str1, comms::util::StringView& str2)
{
    str1.swap(str2);
}

}  // namespace std
