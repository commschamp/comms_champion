//
// Copyright 2014-2015 (C). Alex Robenko. All rights reserved.
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

#include "comms/ErrorStatus.h"
#include "comms/options.h"
#include "basic/ArrayList.h"
#include "details/AdaptBasicField.h"

namespace comms
{

namespace field
{

template <typename TFieldBase, typename TElement, typename... TOptions>
class ArrayList : public TFieldBase
{
    using Base = TFieldBase;
    // TODO: storage type
    typedef basic::ArrayList<TFieldBase, TElement> BasicField;
    typedef details::AdaptBasicFieldT<BasicField, TOptions...> ThisField;
    typedef typename ThisField::ValueType StorageType;

public:
    /// @brief Value Type
    typedef typename ThisField::ElementType ElementType;

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
    ArrayList() = default;

    /// @brief Copy constructor is default
    ArrayList(const ArrayList&) = default;

    ArrayList(ArrayList&&) = default;

    /// @brief Destructor is default
    ~ArrayList() = default;

    /// @brief Copy assignment is default
    ArrayList& operator=(const ArrayList&) = default;

    ArrayList& operator=(ArrayList&&) = default;

//    Iterator begin()
//    {
//        return data_.begin();
//    }
//
//    ConstIterator begin() const
//    {
//        return cbegin();
//    }
//
//    ConstIterator cbegin() const
//    {
//        return data_.cbegin();
//    }
//
//    Iterator end()
//    {
//        return data_.end();
//    }
//
//    ConstIterator end() const
//    {
//        return cend();
//    }
//
//    ConstIterator cend() const
//    {
//        return data_.cend();
//    }
//
//    constexpr std::size_t size() const
//    {
//        return data_.size();
//    }

    /// @brief Get length of serialised data
    constexpr std::size_t length() const
    {
        return field_.length();
    }

//    template <typename U>
//    void pushBack(U&& value)
//    {
//        data_.push_back(std::forward<U>(value));
//    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        return field_.read(iter, len);
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
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return field_.write(iter, len);
    }

    bool valid() const
    {
        return field_.valid();
    }

private:
    ThisField field_;
};

/// @brief Equivalence comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator<(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return std::lexicographical_compare(
                field1.begin(), field1.end(), field2.begin(), field2.end());
}

/// @brief Non-equality comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator!=(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return (field1 < field2) || (field2 < field1);
}

/// @brief Equality comparison operator.
/// @related ComplexIntValue
template <typename... TArgs>
bool operator==(
    const ArrayList<TArgs...>& field1,
    const ArrayList<TArgs...>& field2)
{
    return !(field1 != field2);
}




}  // namespace field

}  // namespace comms

