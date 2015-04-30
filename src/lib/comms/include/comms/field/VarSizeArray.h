//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include "details/VarSizeArrayBase.h"

namespace comms
{

namespace field
{

template <typename TField, typename TElement, typename... TOptions>
class VarSizeArray : public details::VarSizeArrayBase<TField, TElement, TOptions...>
{
    using Base = details::VarSizeArrayBase<TField, TElement, TOptions...>;
    typedef typename Base::StorageType StorageType;
public:
    /// @brief Value Type
    typedef typename Base::ElementType ElementType;

    typedef ElementType& Reference;

    typedef Reference reference;

    typedef const ElementType& ConstReference;

    typedef ConstReference const_reference;

    typedef typename StorageType::iterator Iterator;

    typedef Iterator iterator;

    typedef typename StorageType::const_iterator ConstIterator;

    typedef ConstIterator const_iterator;

    /// @brief Default constructor
    /// @details Sets default value to be 0.
    VarSizeArray() = default;

    /// @brief Copy constructor is default
    VarSizeArray(const VarSizeArray&) = default;

    /// @brief Destructor is default
    ~VarSizeArray() = default;

    /// @brief Copy assignment is default
    VarSizeArray& operator=(const VarSizeArray&) = default;

    Iterator begin()
    {
        return data_.begin();
    }

    ConstIterator begin() const
    {
        return cbegin();
    }

    ConstIterator cbegin() const
    {
        return data_.cbegin();
    }

    Iterator end()
    {
        return data_.end();
    }

    ConstIterator end() const
    {
        return cend();
    }

    ConstIterator cend() const
    {
        return data_.cend();
    }

    constexpr std::size_t size() const
    {
        return data_.size();
    }

    /// @brief Get length of serialised data
    constexpr std::size_t length() const
    {
        return std::accumulate(begin(), end(), std::size_t(0),
            [](std::size_t sum, ConstReference e) -> std::size_t
            {
                return sum + e.length();
            });
    }

    template <typename U>
    void pushBack(U&& value)
    {
        data_.push_back(std::forward<U>(value));
    }

    /// @brief Read the serialised field value from the some data structure.
    /// @tparam TIter Type of input iterator
    /// @param[in, out] iter Input iterator.
    /// @param[in] size Size of the data in iterated data structure.
    /// @return Status of the read operation.
    /// @pre Value of provided "size" must be less than or equal to
    ///      available data in the used data structure/stream
    /// @post The iterator will be incremented.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        data_.clear();
        while (0 < size) {
            ElementType field;
            auto es = field.read(iter, size);
            if (es != ErrorStatus::Success) {
                return es;
            }

            GASSERT(field.length() <= size);
            size -= field.length();
            data_.push_back(std::move(field));
        }

        return ErrorStatus::Success;
    }

    /// @brief Write the serialised field value to some data structure.
    /// @tparam TIter Type of output iterator
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Size of the buffer, field data must fit it.
    /// @return Status of the write operation.
    /// @pre Value of provided "size" must be less than or equal to
    ///      available space in the data structure.
    /// @post The iterator will be incremented.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        if (size < length()) {
            return ErrorStatus::BufferOverflow;
        }

        auto es = ErrorStatus::Success;
        auto remainingSize = size;
        for (auto fieldIter = begin(); fieldIter != end(); ++fieldIter) {
            es = fieldIter->write(iter, remainingSize);
            if (es != ErrorStatus::Success) {
                break;
            }
            remainingSize -= fieldIter->length();
        }

        return es;
    }

    bool valid() const {
        return std::all_of(
            begin(), end(),
            [](ConstReference e) -> bool
            {
                return e.valid();
            });
    }

private:
    StorageType data_;
};

/// @brief Equivalence comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator<(
    const VarSizeArray<TArgs...>& field1,
    const VarSizeArray<TArgs...>& field2)
{
    return std::lexicographical_compare(
                field1.begin(), field1.end(), field2.begin(), field2.end());
}

/// @brief Non-equality comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator!=(
    const VarSizeArray<TArgs...>& field1,
    const VarSizeArray<TArgs...>& field2)
{
    return (field1 < field2) || (field2 < field1);
}

/// @brief Equality comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator==(
    const VarSizeArray<TArgs...>& field1,
    const VarSizeArray<TArgs...>& field2)
{
    return !(field1 != field2);
}




}  // namespace field

}  // namespace comms

