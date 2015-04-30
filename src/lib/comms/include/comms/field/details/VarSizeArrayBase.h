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

#include <vector>
#include <array>
#include <type_traits>
#include "comms/options.h"
#include "comms/Assert.h"
#include "comms/util/StaticQueue.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TElementType, std::size_t TSize>
class VarSizeStdArrayWrapper
{
    typedef TElementType ElementType;

    typedef comms::util::StaticQueue<ElementType, TSize> Queue;
public:
    typedef typename Queue::LinearisedIterator iterator;
    typedef typename Queue::ConstLinearisedIterator const_iterator;
    typedef typename std::iterator_traits<iterator>::iterator_category iterator_category;
    typedef typename std::iterator_traits<iterator>::value_type value_type;
    typedef typename std::iterator_traits<iterator>::difference_type difference_type;
    typedef typename std::iterator_traits<iterator>::pointer pointer;


    iterator begin()
    {
        return data_.lbegin();
    }

    const_iterator begin() const
    {
        return data_.lbegin();
    }


    const_iterator cbegin() const
    {
        return data_.clbegin();
    }

    iterator end()
    {
        return data_.lend();
    }

    const_iterator end() const
    {
        return data_.lend();
    }

    const_iterator cend() const
    {
        return data_.clend();
    }

    std::size_t size() const
    {
        return data_.size();
    }

    static constexpr std::size_t max_size()
    {
        return Queue::capacity();
    }

    template <typename U>
    void push_back(U&& value)
    {
        GASSERT(size() < max_size());
        data_.pushBack(std::forward<U>(value));
    }

    void clear()
    {
        data_.clear();
    }


private:
    Queue data_;
};

template <typename TField, typename T, typename... TOptions>
class VarSizeArrayBase;

template <typename TField, typename T>
class VarSizeArrayBase<TField, T> : public TField
{
    typedef TField Base;

protected:

    typedef T ElementType;
    typedef std::vector<ElementType> StorageType;

    using Base::Base;
};

template <typename TField, typename T, std::size_t TSize, typename... TOptions>
class VarSizeArrayBase<TField, T, comms::option::FixedSizeStorage<TSize>, TOptions...> : public
    VarSizeArrayBase<TField, T, TOptions...>
{
    typedef VarSizeArrayBase<TField, T, TOptions...> Base;
    typedef comms::option::FixedSizeStorage<TSize> Option;

protected:

    typedef typename Base::ElementType ElementType;
    typedef VarSizeStdArrayWrapper<ElementType, Option::Value> StorageType;

    using Base::Base;
};



}  // namespace details

}  // namespace field

}  // namespace comms


