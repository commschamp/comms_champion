//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms/field/category.h"
#include "CommonBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

namespace details
{

template <typename TNext>
class CollectionFieldAdapterBase : public CommonBase<TNext>
{
    using Base = CommonBase<TNext>;
public:
    using Category = typename Base::Category;
    using Next = typename Base::Next;
    using ValueType = typename Base::ValueType;
    using ElementType = typename Next::ElementType;

    static_assert(
        std::is_base_of<comms::field::category::CollectionField, Category>::value,
        "This adapter base class is expected to wrap the collection field.");

    CollectionFieldAdapterBase(const CollectionFieldAdapterBase&) = default;
    CollectionFieldAdapterBase(CollectionFieldAdapterBase&&) = default;
    CollectionFieldAdapterBase& operator=(const CollectionFieldAdapterBase&) = default;
    CollectionFieldAdapterBase& operator=(CollectionFieldAdapterBase&&) = default;


    template <typename U>
    void pushBack(U&& val)
    {
        Base::next().pushBack(std::forward<U>(val));
    }

    void clear()
    {
        Base::next().clear();
    }

    static constexpr std::size_t minElementLength()
    {
        return Next::minElementLength();
    }

    static constexpr std::size_t maxElementLength()
    {
        return Next::maxElementLength();
    }

    static constexpr std::size_t elementLength(const ElementType& elem)
    {
        return Next::elementLength(elem);
    }

    template <typename TIter>
    static ErrorStatus readElement(ElementType& elem, TIter& iter, std::size_t& len)
    {
        return Next::readElement(elem, iter, len);
    }

    template <typename TIter>
    static ErrorStatus writeElement(const ElementType& elem, TIter& iter, std::size_t& len)
    {
        return Next::writeElement(elem, iter, len);
    }

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len)
    {
        return Base::next().readN(count, iter, len);
    }

    template <typename TIter>
    ErrorStatus writeN(std::size_t count, TIter& iter, std::size_t& len) const
    {
        return Base::next().writeN(count, iter, len);
    }

    void forceReadElemCount(std::size_t count)
    {
        Base::next().forceReadElemCount(count);
    }

    void clearReadElemCount()
    {
        Base::next().clearReadElemCount();
    }

protected:
    CollectionFieldAdapterBase() = default;
    explicit CollectionFieldAdapterBase(const ValueType& val)
      : Base(val)
    {
    }

    explicit CollectionFieldAdapterBase(ValueType&& val)
      : Base(std::move(val))
    {
    }
};

}  // namespace details

}  // namespace adapter

}  // namespace field

}  // namespace comms




