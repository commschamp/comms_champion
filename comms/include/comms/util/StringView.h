//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
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

/// @file
/// @brief Contains comms::util::StringView class.


#pragma once

#include <algorithm>
#include <iterator>
#include <limits>
#include <string>

#include "comms/Assert.h"
#include "ArrayView.h"

namespace comms
{

namespace util
{

/// @brief Describes an object that can refer to a constant contiguous
///     sequence of char-like objects with the first element of the
///     sequence at position zero.
/// @details Similar to <a href="http://en.cppreference.com/w/cpp/string/basic_string_view">std::string_view</a>
///     introduced in C++17.
/// @headerfile "comms/util/StringView.h"
class StringView : public ArrayView<char>
{
    using Base = ArrayView<char>;
public:
    /// @brief Type of the character (@b char)
    using value_type = typename Base::value_type;

    /// @brief Same as @ref value_type
    using ValueType = value_type;

    /// @brief Pointer to the character (@b char*)
    using pointer = typename Base::pointer;

    /// @brief Same as @ref pointer
    using Pointer = pointer;

    /// @brief Pointer to the constant character (<b>const char*</b>)
    using const_pointer = typename Base::const_pointer;

    /// @brief Same as @ref const_pointer
    using ConstPointer = const_pointer;

    /// @brief Reference to a character (@b char&)
    using reference = typename Base::reference;

    /// @brief Same as @ref reference
    using Reference = reference;

    /// @brief Reference to a const character (<b>const char&</b>)
    using const_reference = typename Base::const_reference;

    /// @brief Same as @ref const_reference
    using ConstReference = const_reference;

    /// @brief Equal to @b std::size_t
    using size_type = typename Base::size_type;

    /// @brief Same as @ref size_type;
    using SizeType = size_type;

    /// @brief Implementation defined constant RandomAccessIterator and
    ///     ContiguousIterator whose value_type is @b char.
    using const_iterator = typename Base::const_iterator;

    /// @brief Same as @ref const_iterator
    using ConstIterator = const_iterator;

    /// @brief Same as @ref const_iterator
    using iterator = const_iterator;

    /// @brief Same as @ref iterator
    using Iterator = iterator;

    /// @brief Same as std::reverse_iterator<const_iterator>
    using const_reverse_iterator = typename Base::const_reverse_iterator;

    /// @brief Same as @ref const_reverse_iterator
    using ConstReverseIterator = const_reverse_iterator;

    /// @brief Same as @ref const_reverse_iterator
    using reverse_iterator = const_reverse_iterator;

    /// @brief Same as @ref reverse_iterator
    using ReverseIterator = reverse_iterator;

    /// @brief Special value, the meaning is the same as
    ///     <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/npos">std::string_view::npos</a>.
    static const auto npos = static_cast<SizeType>(-1);

    /// @brief Default constructor
    /// @details See <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view">std::string_view constructor</a>
    ///     for details.
    StringView() noexcept = default;

    /// @brief Copy constructor
    /// @details See <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view">std::string_view constructor</a>
    ///     for details.
    StringView(const StringView&) noexcept = default;

    /// @brief Constructor
    /// @details See <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view">std::string_view constructor</a>
    ///     for details.
    StringView(const char* str, size_type len) noexcept : Base(str, len) {}

    /// @brief Constructor
    /// @details See <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view">std::string_view constructor</a>
    ///     for details.
    StringView(const char* str) noexcept
    {
        static const auto MaxLen = std::numeric_limits<size_type>::max();
        size_type len = 0;
        while (len < MaxLen) {
            if (str[len] == '\0') {
                break;
            }
            ++len;
        }
        Base::operator=(Base(str, len));
    }

    /// @brief Constructor
    StringView(const std::string& str) noexcept
      : Base(str.c_str(), str.size())
    {
    }

    /// @brief Construct out of array of characters with known size
    /// @details Omits the last '\0' character if such exists
    template <std::size_t TN>
    StringView(const char (&str)[TN]) noexcept : Base(str, TN)
    {
        if ((0U <= TN) && (back() == '\0')) {
            remove_suffix(1);
        }
    }

    /// @brief Construct out of array of characters with known size
    /// @details Omits the last '\0' character if such exists
    template <std::size_t TN>
    StringView(char (&str)[TN]) noexcept : Base(str, TN)
    {
        if ((0U <= TN) && (back() == '\0')) {
            remove_suffix(1);
        }
    }

    /// @brief Destructor
    ~StringView() noexcept = default;

    /// @brief Copy assign
    StringView& operator=(const StringView&) = default;

    /// @brief Assign array of characters with known size
    /// @details Omits the last '\0' character if such exists
    template <std::size_t TN>
    StringView& operator=(const char (&str)[TN])
    {
        Base::operator=(str);
        if ((0U <= TN) && (back() == '\0')) {
            remove_suffix(1);
        }
        return *this;
    }

    /// @brief Assign array of characters with known size
    /// @details Omits the last '\0' character if such exists
    template <std::size_t TN>
    StringView& operator=(char (&str)[TN])
    {
        Base::operator=(str);
        if ((0U <= TN) && (back() == '\0')) {
            remove_suffix(1);
        }
        return *this;
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/begin">std::string_view::begin()</a>.
    constexpr iterator begin() const noexcept
    {
        return Base::begin();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/begin">std::string_view::cbegin()</a>.
    constexpr const_iterator cbegin() const noexcept
    {
        return Base::cbegin();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/end">std::string_view::end()</a>.
    constexpr iterator end() const noexcept
    {
        return Base::end();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/end">std::string_view::end()</a>.
    constexpr const_iterator cend() const noexcept
    {
        return Base::cend();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/rbegin">std::string_view::rbegin()</a>.
    const_reverse_iterator rbegin() const noexcept
    {
        return Base::rbegin();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/rbegin">std::string_view::crbegin()</a>.
    const_reverse_iterator crbegin() const noexcept
    {
        return Base::crbegin();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/rend">std::string_view::rend()</a>.
    reverse_iterator rend() const noexcept
    {
        return Base::rend();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/rend">std::string_view::crend()</a>.
    const_reverse_iterator crend() const noexcept
    {
        return Base::crend();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/operator_at">std::string_view::oprator[]()</a>
    constexpr const_reference operator[](size_type pos) const
    {
        return Base::operator[](pos);
    }

    /// @brief Similar to <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/at">std::string::at()</a>
    /// @details Checks the range with @ref COMMS_ASSERT() macro without throwing exception.
    const_reference at(size_type pos) const
    {
        return Base::at(pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/front">std::string_view::front()</a>
    constexpr const_reference front() const
    {
        return Base::front();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/back">std::string_view::back()</a>
    constexpr const_reference back() const
    {
        return Base::back();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/data">std::string_view::data()</a>
    constexpr const_pointer data() const noexcept
    {
        return &(*begin());
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/size">std::string_view::size()</a>
    constexpr size_type size() const noexcept
    {
        return Base::size();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/size">std::string_view::length()</a>
    constexpr size_type length() const noexcept
    {
        return Base::length();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/empty">std::string_view::empty()</a>
    constexpr bool empty() const noexcept
    {
        return Base::empty();
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/substr">std::string_view::substr()</a>
    std::string substr(size_type pos = 0, size_type count = npos) const
    {
        COMMS_ASSERT(pos <= size());
        return std::string(begin() + pos, begin() + pos + std::min(size() - pos, count));
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/remove_prefix">std::string_view::remove_prefix()</a>
    void remove_prefix(size_type n)
    {
        Base::remove_prefix(n);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/remove_suffix">std::string_view::remove_suffix()</a>
    void remove_suffix(size_type n)
    {
        Base::remove_suffix(n);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/swap>std::string_view::swap()</a>.
    void swap(StringView& other) noexcept
    {
        Base::swap(other);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/copy">std::string_view::copy()</a>.
    size_type copy(char* dest, size_type count, size_type pos = 0) const
    {
        if (size() <= pos) {
            return 0U;
        }

        auto toCopy = std::min(count, size() - pos);
        std::copy_n(cbegin() + pos, toCopy, dest);
        return toCopy;
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(const StringView& other) const
    {
        return compare(0, size(), other);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(size_type pos, size_type count, const StringView& other) const
    {
        return compare(pos, count, other, 0, other.size());
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(
        size_type pos1,
        size_type count1,
        const StringView& other,
        size_type pos2,
        size_type count2) const
    {
        COMMS_ASSERT(pos1 <= size());
        COMMS_ASSERT(pos2 <= other.size());
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

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(const char* s) const
    {
        return compare(0U, size(), s);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(size_type pos, size_type count, const char* s) const
    {
        return compare(pos, count, StringView(s));
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(size_type pos1, size_type count1, const char* s, size_type count2) const
    {
        return compare(pos1, count1, StringView(s, count2));
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(const StringView& str, size_type pos = 0) const
    {
        if (size() <= pos) {
            return npos;
        }

        COMMS_ASSERT(pos <= size());
        auto remCount = size() - pos;
        if (remCount < str.size()) {
            return npos;
        }

        auto maxPos = size() - str.size();
        for (auto idx = pos; idx <= maxPos; ++idx) {
            auto thisStrBeg = cbegin() + idx;
            auto thisStrEnd = thisStrBeg + str.size();
            if (std::equal(thisStrBeg, thisStrEnd, str.begin())) {
                return idx;
            }
        }
        return npos;
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(char c, size_type pos = 0) const
    {
        return find(StringView(&c, 1), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(const char* str, size_type pos, size_type count) const
    {
        return find(StringView(str, count), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(const char* str, size_type pos = 0) const
    {
        return find(StringView(str), pos);
    }
    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(const StringView& other, size_type pos = 0)
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

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(char c, size_type pos = 0)
    {
        return find_first_of(StringView(&c, 1), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(const char* str, size_type pos, size_type count)
    {
        return find_first_of(StringView(str, count), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(const char* str, size_type pos = 0)
    {
        return find_first_of(StringView(str), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(const StringView& other, size_type pos = npos)
    {
        if (empty()) {
            return npos;
        }

        pos = std::min(pos, size() - 1);
        auto begIter = std::reverse_iterator<const_iterator>(cbegin() + pos + 1);
        auto endIter = std::reverse_iterator<const_iterator>(cbegin());
        for (auto iter = begIter; iter != endIter; ++iter) {
            auto foundIter = std::find(other.cbegin(), other.cend(), *iter);
            if (foundIter != other.cend()) {
                return static_cast<std::size_t>(std::distance(iter, endIter)) - 1U;
            }
        }

        return npos;
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(char c, size_type pos = npos)
    {
        return find_last_of(StringView(&c, 1), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(const char* str, size_type pos, size_type count)
    {
        return find_last_of(StringView(str, count), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(const char* str, size_type pos = npos)
    {
        return find_last_of(StringView(str), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(const StringView& other, size_type pos = 0)
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

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(char c, size_type pos = 0)
    {
        return find_first_not_of(StringView(&c, 1), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(const char* str, size_type pos, size_type count)
    {
        return find_first_not_of(StringView(str, count), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(const char* str, size_type pos = 0)
    {
        return find_first_not_of(StringView(str), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(const StringView& other, size_type pos = npos)
    {
        if (empty()) {
            return npos;
        }

        pos = std::min(pos, size() - 1);
        auto begIter = std::reverse_iterator<const_iterator>(cbegin() + pos + 1);
        auto endIter = std::reverse_iterator<const_iterator>(cbegin());
        for (auto iter = begIter; iter != endIter; ++iter) {
            auto foundIter = std::find(other.cbegin(), other.cend(), *iter);
            if (foundIter == other.cend()) {
                return static_cast<std::size_t>(std::distance(iter, endIter)) - 1U;
            }
        }

        return npos;
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(char c, size_type pos = 0)
    {
        return find_last_not_of(StringView(&c, 1), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(const char* str, size_type pos, size_type count)
    {
        return find_last_not_of(StringView(str, count), pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(const char* str, size_type pos = npos)
    {
        return find_last_not_of(StringView(str), pos);
    }

};

/// @brief Lexicographical compare between the string views.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator<(const StringView& str1, const StringView& str2)
{
    return std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end());
}


/// @brief Lexicographical compare between the string views.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator<=(const StringView& str1, const StringView& str2)
{
    return !(str2 < str1);
}

/// @brief Lexicographical compare between the string views.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator>(const StringView& str1, const StringView& str2)
{
    return (str2 < str1);
}

/// @brief Lexicographical compare between the string views.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator>=(const StringView& str1, const StringView& str2)
{
    return !(str1 < str2);
}

/// @brief Equality compare between the string views.
/// @see <a href="http://en.cppreference.com/w/cpp/string/basic_string/operator_cmp">Reference</a>
/// @related StringView
inline
bool operator==(const StringView& str1, const StringView& str2)
{
    return
        (str1.size() == str2.size()) &&
        std::equal(str1.begin(), str1.end(), str2.begin());
}

/// @brief Inequality compare between the string views.
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
