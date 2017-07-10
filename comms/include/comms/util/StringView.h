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

/// @file
/// @brief Contains comms::util::StringView class.


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

    template <std::size_t TN>
    StringViewBase(const TChar (&str)[TN]) noexcept
      : str_(str),
        len_(TN)
    {
        if (str_[len_ - 1] == '\0') {
            --len_;
        }
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

    template <std::size_t TN>
    StringViewBase& operator=(const TChar (&str)[TN])
    {
        str_ = str;
        len_ = TN;
        if (str_[len_ - 1] == '\0') {
            --len_;
        }
        return *this;
    }


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

    ReverseIterator rbegin() const noexcept
    {
        return std::reverse_iterator<Iterator>(end());
    }

    ConstReverseIterator crbegin() const noexcept
    {
        return rbegin();
    }

    ReverseIterator rend() const noexcept
    {
        return std::reverse_iterator<Iterator>(begin());
    }

    ConstReverseIterator crend() const noexcept
    {
        return rend();
    }

    constexpr const_reference operator[](SizeType pos) const
    {
        return str_[pos];
    }

    const_reference at(SizeType pos) const
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
        return compare(pos, count, StringViewBase(s));
    }

    int compare(SizeType pos1, SizeType count1, const TChar* s, SizeType count2) const
    {
        return compare(pos1, count1, StringViewBase(s, count2));
    }


    SizeType find(const StringViewBase& str, SizeType pos = 0) const
    {
        if (size() <= pos) {
            return npos;
        }

        GASSERT(pos <= size());
        auto remCount = size() - pos;
        if (remCount < str.size()) {
            return npos;
        }

        auto maxPos = size() - str.size();
        for (auto idx = pos; idx <= maxPos; ++idx) {
            auto* thisStrBeg = &str_[idx];
            auto* thisStrEnd = thisStrBeg + str.size();
            if (std::equal(thisStrBeg, thisStrEnd, str.begin())) {
                return idx;
            }
        }
        return npos;
    }

    SizeType find(TChar c, SizeType pos = 0) const
    {
        return find(StringViewBase(&c, 1), pos);
    }

    SizeType find(const TChar* str, SizeType pos, SizeType count) const
    {
        return find(StringViewBase(str, count), pos);
    }

    SizeType find(const TChar* str, SizeType pos = 0) const
    {
        return find(StringViewBase(str), pos);
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

    SizeType find_last_of(const StringViewBase& other, SizeType pos = npos)
    {
        if (empty()) {
            return npos;
        }

        pos = std::min(pos, size() - 1);
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

    SizeType find_last_of(TChar c, SizeType pos = npos)
    {
        return find_last_of(StringViewBase(&c, 1), pos);
    }

    SizeType find_last_of(const TChar* str, SizeType pos, SizeType count)
    {
        return find_last_of(StringViewBase(str, count), pos);
    }

    SizeType find_last_of(const TChar* str, SizeType pos = npos)
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

    SizeType find_last_not_of(const StringViewBase& other, SizeType pos = npos)
    {
        if (empty()) {
            return npos;
        }

        pos = std::min(pos, size() - 1);
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

    SizeType find_last_not_of(TChar c, SizeType pos = npos)
    {
        return find_last_not_of(StringViewBase(&c, 1), pos);
    }

    SizeType find_last_not_of(const TChar* str, SizeType pos, SizeType count)
    {
        return find_last_not_of(StringViewBase(str, count), pos);
    }

    SizeType find_last_not_of(const TChar* str, SizeType pos = npos)
    {
        return find_last_not_of(StringViewBase(str), pos);
    }

private:
    const TChar* str_ = nullptr;
    std::size_t len_ = 0;
};

} // namespace details

/// @brief Describes an object that can refer to a constant contiguous
///     sequence of char-like objects with the first element of the
///     sequence at position zero.
/// @details Similar to <a href="http://en.cppreference.com/w/cpp/string/basic_string_view">std::string_view</a>
///     introduced in C++17.
/// @headerfile "comms/util/StringView.h"
class StringView : public details::StringViewBase<>
{
    using Base = details::StringViewBase<>;
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
    static const auto npos = Base::npos;

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
    StringView(const char* str, SizeType len) noexcept : Base(str, len) {}

    /// @brief Constructor
    /// @details See <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view">std::string_view constructor</a>
    ///     for details.
    StringView(const char* str) noexcept : Base(str) {}

    /// @brief Construct out of array of characters with known size
    /// @details Omits the last '\0' character if such exists
    template <std::size_t TN>
    StringView(const char (&str)[TN]) noexcept : Base(str) {}

    /// @brief Destructor
    ~StringView() = default;

    /// @brief Copy assign
    StringView& operator=(const StringView&) = default;

    /// @brief Assign array of characters with known size
    /// @details Omits the last '\0' character if such exists
    template <std::size_t TN>
    StringView& operator=(const char (&str)[TN])
    {
        Base::operator=(str);
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
    /// @details Checks the range with @ref GASSERT() macro without throwing exception.
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
        return Base::data();
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
        return Base::copy(dest, count, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(const StringView& other) const
    {
        return Base::compare(other);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(size_type pos, size_type count, const StringView& other) const
    {
        return Base::compare(pos, count, other);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(
        size_type pos1,
        size_type count1,
        const StringView& other,
        size_type pos2,
        size_type count2) const
    {
        return Base::compare(pos1, count1, other, pos2, count2);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(const char* s) const
    {
        return Base::compare(s);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(size_type pos, size_type count, const char* s) const
    {
        return Base::compare(pos, count, s);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/compare">std::string_view::compare()</a>.
    int compare(size_type pos1, size_type count1, const char* s, size_type count2) const
    {
        return Base::compare(pos1, count1, s, count2);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(const StringView& str, size_type pos = 0) const
    {
        return Base::find(str, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(char c, size_type pos = 0) const
    {
        return Base::find(c, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(const char* str, size_type pos, size_type count) const
    {
        return Base::find(str, pos, count);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find">std::string_view::find()</a>.
    size_type find(const char* str, size_type pos = 0) const
    {
        return Base::find(str, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(const StringView& other, size_type pos = 0)
    {
        return Base::find_first_of(other, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(char c, size_type pos = 0)
    {
        return Base::find_first_of(c, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(const char* str, size_type pos, size_type count)
    {
        return Base::find_first_of(str, pos, count);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_of>std::string_view::find_first_of</a>.
    size_type find_first_of(const char* str, size_type pos = 0)
    {
        return Base::find_first_of(str, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(const StringView& other, size_type pos = npos)
    {
        return Base::find_last_of(other, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(char c, size_type pos = npos)
    {
        return Base::find_last_of(c, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(const char* str, size_type pos, size_type count)
    {
        return Base::find_last_of(str, pos, count);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_of>std::string_view::find_last_of</a>.
    size_type find_last_of(const char* str, size_type pos = npos)
    {
        return Base::find_last_of(str, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(const StringView& other, size_type pos = 0)
    {
        return Base::find_first_not_of(other, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(char c, size_type pos = 0)
    {
        return Base::find_first_not_of(c, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(const char* str, size_type pos, size_type count)
    {
        return Base::find_first_not_of(str, pos, count);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_first_not_of>std::string_view::find_first_not_of</a>.
    size_type find_first_not_of(const char* str, size_type pos = 0)
    {
        return Base::find_first_not_of(str, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(const StringView& other, size_type pos = npos)
    {
        return Base::find_last_not_of(other, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(char c, size_type pos = 0)
    {
        return Base::find_last_not_of(c, pos);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(const char* str, size_type pos, size_type count)
    {
        return Base::find_last_not_of(str, pos, count);
    }

    /// @brief Same as <a href="http://en.cppreference.com/w/cpp/string/basic_string_view/find_last_not_of>std::string_view::find_last_not_of</a>.
    size_type find_last_not_of(const char* str, size_type pos = npos)
    {
        return Base::find_last_not_of(str, pos);
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
