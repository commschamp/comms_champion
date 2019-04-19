//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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
/// @brief Contains comms::util::ArrayView class.


#pragma once

#include <algorithm>
#include <iterator>

#include "comms/Assert.h"

namespace comms
{

namespace util
{

/// @brief Describes an object that can refer to a constant contiguous
///     sequence of other objects.
/// @details Provides "view" on the original data.
/// @headerfile "comms/util/ArrayView.h"
template <typename T>
class ArrayView
{
public:
    /// @brief Type of the single element
    using value_type = T;

    /// @brief Same as @ref value_type
    using ValueType = value_type;

    /// @brief Pointer to the single element (@b T*)
    using pointer = T*;

    /// @brief Same as @ref pointer
    using Pointer = pointer;

    /// @brief Pointer to the constant element (<b>const T*</b>)
    using const_pointer = const T*;

    /// @brief Same as @ref const_pointer
    using ConstPointer = const_pointer;

    /// @brief Reference to an element (@b T&)
    using reference = T&;

    /// @brief Same as @ref reference
    using Reference = reference;

    /// @brief Reference to a const element (<b>const T&</b>)
    using const_reference = const T&;

    /// @brief Same as @ref const_reference
    using ConstReference = const_reference;

    /// @brief Equal to @b std::size_t
    using size_type = std::size_t;

    /// @brief Same as @ref size_type;
    using SizeType = size_type;

    /// @brief Implementation defined constant RandomAccessIterator and
    ///     ContiguousIterator whose value_type is @b T.
    using const_iterator = const_pointer;

    /// @brief Same as @ref const_iterator
    using ConstIterator = const_iterator;

    /// @brief Same as @ref const_iterator
    using iterator = const_iterator;

    /// @brief Same as @ref iterator
    using Iterator = iterator;

    /// @brief Same as std::reverse_iterator<const_iterator>
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /// @brief Same as @ref const_reverse_iterator
    using ConstReverseIterator = const_reverse_iterator;

    /// @brief Same as @ref const_reverse_iterator
    using reverse_iterator = const_reverse_iterator;

    /// @brief Same as @ref reverse_iterator
    using ReverseIterator = reverse_iterator;

    /// @brief Default constructor
    ArrayView() noexcept = default;

    /// @brief Copy constructor
    ArrayView(const ArrayView&) noexcept = default;

    /// @brief Constructor
    ArrayView(const_pointer data, size_type len) noexcept
      : data_(data),
        len_(len)
    {
    }

    /// @brief Construct out of array of elements with known size
    template <std::size_t TN>
    ArrayView(const T (&data)[TN]) noexcept
      : data_(data),
        len_(TN)
    {
    }

    /// @brief Construct out of array of elements with known size
    template <std::size_t TN>
    ArrayView(T (&data)[TN]) noexcept
      : data_(data),
        len_(TN)
    {
    }

    /// @brief Destructor
    ~ArrayView() noexcept = default;

    /// @brief Copy assign
    ArrayView& operator=(const ArrayView&) = default;

    /// @brief Assign array of elements with known size
    template <std::size_t TN>
    ArrayView& operator=(const T (&data)[TN])
    {
        data_ = data;
        len_ = TN;
        return *this;
    }

    /// @brief Assign array of elements with known size
    template <std::size_t TN>
    ArrayView& operator=(T (&data)[TN])
    {
        data_ = data;
        len_ = TN;
        return *this;
    }

    /// @brief Iterator to begining of the sequence.
    constexpr const_iterator begin() const noexcept
    {
        return data_;
    }

    /// @brief Iterator to begining of the sequence.
    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    /// @brief Iterator to the end of the sequence
    constexpr const_iterator end() const noexcept
    {
        return begin() + len_;
    }

    /// @brief Iterator to the end of the sequence
    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    /// @brief Reverse iterator to the end of the sequence.
    const_reverse_iterator rbegin() const noexcept
    {
        return std::reverse_iterator<const_iterator>(end());
    }

    /// @brief Reverse iterator to the end of the sequence.
    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    /// @brief Reverse iterator to the beginning of the sequence.
    reverse_iterator rend() const noexcept
    {
        return std::reverse_iterator<const_iterator>(begin());
    }

    /// @brief Reverse iterator to the beginning of the sequence.
    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    /// @brief Element access operator
    constexpr const_reference operator[](size_type pos) const
    {
        return data_[pos];
    }

    /// @brief Element access with range check
    /// @details Checks the range with @ref COMMS_ASSERT() macro without throwing exception.
    const_reference at(size_type pos) const
    {
        COMMS_ASSERT(pos < len_);
        return data_[pos];
    }

    /// @brief Access the first element
    /// @pre The view is not empty
    constexpr const_reference front() const
    {
        return data_[0];
    }

    /// @brief Access the last element
    /// @pre The view is not empty
    constexpr const_reference back() const
    {
        return data_[len_ - 1U];
    }

    /// @brief Get number of element in the view.
    constexpr size_type size() const noexcept
    {
        return len_;
    }

    /// @brief Same as ref size()
    constexpr size_type length() const noexcept
    {
        return size();
    }

    /// @brief Check the view is empty
    /// @return @b true if and only if call to @ref size() returns @b 0.
    constexpr bool empty() const noexcept
    {
        return size() == 0U;
    }

    /// @brief Narrow the view by skipping number of elements at the beginning.
    /// @pre @b n is less or equal to value returned by @ref size().
    void remove_prefix(size_type n)
    {
        std::advance(data_, n);
        len_ -= n;
    }

    /// @brief Narrow the view by dropping number of elements at the end.
    /// @pre @b n is less or equal to value returned by @ref size().
    void remove_suffix(size_type n)
    {
        len_ -= n;
    }

    /// @brief Swap contents of two views
    void swap(ArrayView& other) noexcept
    {
        std::swap(data_, other.data_);
        std::swap(len_, other.len_);
    }


private:
    const_pointer data_ = nullptr;
    size_type len_ = 0;
};

/// @brief Lexicographical compare between the views.
/// @related ArrayView
template<typename T>
bool operator<(const ArrayView<T>& view1, const ArrayView<T>& view2)
{
    return std::lexicographical_compare(view1.begin(), view1.end(), view2.begin(), view2.end());
}

/// @brief Lexicographical compare between the views.
/// @related ArrayView
template<typename T>
bool operator<=(const ArrayView<T>& view1, const ArrayView<T>& view2)
{
    return !(view2 < view1);
}

/// @brief Lexicographical compare between the views.
/// @related ArrayView
template<typename T>
bool operator>(const ArrayView<T>& view1, const ArrayView<T>& view2)
{
    return (view2 < view1);
}

/// @brief Lexicographical compare between the views.
/// @related ArrayView
template<typename T>
bool operator>=(const ArrayView<T>& view1, const ArrayView<T>& view2)
{
    return !(view1 < view2);
}

/// @brief Equality compare between the views.
/// @related ArrayView
template<typename T>
bool operator==(const ArrayView<T>& view1, const ArrayView<T>& view2)
{
    return
        (view1.size() == view2.size()) &&
        std::equal(view1.begin(), view1.end(), view2.begin());
}

/// @brief Inequality compare between the views.
/// @related ArrayView
template<typename T>
bool operator!=(const ArrayView<T>& view1, const ArrayView<T>& view2)
{
    return !(view1 == view2);
}


} // namespace util

} // namespace comms

namespace std
{

/// @brief Specializes the std::swap algorithm.
/// @related comms::util::ArrayView
template <typename T>
void swap(comms::util::ArrayView<T>& view1, comms::util::ArrayView<T>& view2)
{
    view1.swap(view2);
}

}  // namespace std
