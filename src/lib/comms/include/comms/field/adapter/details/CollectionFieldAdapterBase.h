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
    typedef CommonBase<TNext> Base;
public:
    typedef typename Base::Category Category;
    typedef typename Base::Next Next;
    typedef typename Base::ValueType ValueType;
    typedef typename Next::ElementType ElementType;

    static_assert(
        std::is_base_of<comms::field::category::CollectionField, Category>::value,
        "This adapter base class is expected to wrap the collection field.");

    CollectionFieldAdapterBase(const CollectionFieldAdapterBase&) = default;
    CollectionFieldAdapterBase(CollectionFieldAdapterBase&&) = default;
    CollectionFieldAdapterBase& operator=(const CollectionFieldAdapterBase&) = default;
    CollectionFieldAdapterBase& operator=(CollectionFieldAdapterBase&&) = default;


    template <typename U>
    void pushBack(U&& value)
    {
        Base::next().pushBack(std::forward<U>(value));
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
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t len)
    {
        return Base::next().readN(count, iter, len);
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
    explicit CollectionFieldAdapterBase(const ValueType& value)
      : Base(value)
    {
    }

    explicit CollectionFieldAdapterBase(ValueType&& value)
      : Base(std::move(value))
    {
    }
};

}  // namespace details

}  // namespace adapter

}  // namespace field

}  // namespace comms




