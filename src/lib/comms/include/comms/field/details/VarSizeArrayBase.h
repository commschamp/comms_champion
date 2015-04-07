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
    typedef std::array<ElementType, TSize> StorageType;

    typedef typename
        std::aligned_storage<
        sizeof(ElementType),
        alignof(ElementType)
    >::type ActualElementType;

    typedef std::array<ActualElementType, TSize> ActualStorageType;
public:
    typedef typename StorageType::iterator iterator;
    typedef typename StorageType::const_iterator const_iterator;

    iterator begin()
    {
        return reinterpret_cast<StorageType*>(&data_)->begin();
    }

    const_iterator cbegin() const
    {
        return reinterpret_cast<const StorageType*>(&data_)->cbegin();
    }

    iterator end()
    {
        return begin() + size_;
    }

    const_iterator cend() const
    {
        return cbegin() + size_;
    }

    std::size_t size() const
    {
        return size_;
    }

    static constexpr std::size_t max_size()
    {
        return TSize;
    }

    template <typename U>
    void push_back(U&& value)
    {
        GASSERT(size_ < TSize);
        auto* place = &data_[size_];
        new (place) ElementType(std::forward<U>(value));
        ++size_;
    }

    void clear()
    {
        for (auto idx = 0U; idx < size_; ++idx) {
            auto* elem = reinterpret_cast<ElementType*>(&data_[idx]);
            elem->~ElementType();
        }
        size_ = 0;
    }


private:
    ActualStorageType data_;
    std::size_t size_ = 0;
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


