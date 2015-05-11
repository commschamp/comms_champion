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

namespace comms
{

namespace field
{

namespace adapter
{

namespace details
{

template <class T, class R = void>
struct EnableIfHasFieldType { typedef R Type; };

template <class T, class Enable = void>
struct FieldType
{
    typedef T Type;
};

template <class T>
struct FieldType<T, typename EnableIfHasFieldType<typename T::FieldType>::Type>
{
    typedef typename T::FieldType Type;
};

template <class T>
using FieldTypeT = typename FieldType<T>::Type;

template <typename TNext>
class CommonBase
{
    static_assert(comms::field::category::isCategorised<TNext>(),
        "Next layer must be categorised.");
public:

    typedef TNext Next;
    typedef typename Next::Category Category;

    typedef details::FieldTypeT<Next> FieldType;

    typedef typename Next::Endian Endian;

    typedef typename Next::ValueType ValueType;

    typedef typename Next::ParamValueType ParamValueType;

    CommonBase(const CommonBase&) = default;
    CommonBase(CommonBase&&) = default;
    CommonBase& operator=(const CommonBase&) = default;
    CommonBase& operator=(CommonBase&&) = default;

    Next& next()
    {
        return next_;
    }

    const Next& next() const
    {
        return next_;
    }

    ParamValueType getValue() const
    {
        return next().getValue();
    }

    void setValue(ParamValueType value)
    {
        next().setValue(value);
    }

    constexpr std::size_t length() const
    {
        return next().length();
    }

    static constexpr std::size_t minLength()
    {
        return Next::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return Next::maxLength();
    }

    constexpr bool valid() const
    {
        return next().valid();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return next().read(iter, size);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return next().write(iter, size);
    }

protected:
    CommonBase() = default;
    explicit CommonBase(ParamValueType value)
      : next_(value)
    {
    }

private:
    struct NextIsLeafTag {};
    struct NextIsAdapterTag {};

    typedef typename std::conditional<
        std::is_same<FieldType, Next>::value,
        NextIsLeafTag,
        NextIsAdapterTag
    >::type NextTypeTag;

    FieldType& fieldInternal(NextIsLeafTag)
    {
        return next_;
    }

    const FieldType& fieldInternal(NextIsLeafTag) const
    {
        return next_;
    }

    FieldType& fieldInternal(NextIsAdapterTag)
    {
        return next_.field();
    }

    const FieldType& fieldInternal(NextIsAdapterTag) const
    {
        return next_.field();
    }

    Next next_;
};

}  // namespace details

}  // namespace adapter

}  // namespace field

}  // namespace comms

