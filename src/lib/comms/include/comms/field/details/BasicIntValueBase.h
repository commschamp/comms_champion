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

#include <type_traits>
#include <limits>

#include "comms/field/options.h"
#include "comms/util/SizeToType.h"
#include "comms/util/IntegralPromotion.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TField, typename T, typename... TOptions>
class BasicIntValueBase;

template <typename TField, typename T>
class BasicIntValueBase<TField, T> : public TField
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef TField Base;

protected:

    typedef T ValueType;
    typedef typename comms::util::IntegralPromotion<ValueType>::Type OffsetType;
    typedef typename util::SizeToType<sizeof(T), std::is_signed<T>::value>::Type SerialisedType;

    using Base::Base;

    static const auto DefaultValue = static_cast<ValueType>(0);
    static const std::size_t SerialisedLen = sizeof(T);
    static const auto MinValidValue = std::numeric_limits<ValueType>::min();
    static const auto MaxValidValue = std::numeric_limits<ValueType>::max();
    static const auto Offset = static_cast<OffsetType>(0);
};

template <typename TField, typename T, long long int TMinValue, long long int TMaxValue, typename... TOptions>
class BasicIntValueBase<
    TField,
    T,
    comms::field::option::ValidRangeImpl<TMinValue, TMaxValue>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;

protected:
    using Base::BasicIntValueBase;

    static const auto MinValidValue =
        static_cast<decltype(Base::MinValidValue)>(TMinValue);
    static const auto MaxValidValue =
        static_cast<decltype(Base::MaxValidValue)>(TMaxValue);
};

template <typename TField, typename T, std::size_t TLen, typename... TOptions>
class BasicIntValueBase<
    TField,
    T,
    comms::field::option::LengthLimitImpl<TLen>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;

protected:
    using Base::BasicIntValueBase;

    typedef typename util::SizeToType<TLen, std::is_signed<T>::value>::Type SerialisedType;

    static const std::size_t SerialisedLen = TLen;
};

template <typename TField, typename T, long long int TOff, typename... TOptions>
class BasicIntValueBase<
    TField,
    T,
    comms::field::option::SerOffsetImpl<TOff>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;
protected:
    using Base::BasicIntValueBase;

    static const auto Offset = static_cast<decltype(Base::Offset)>(TOff);
};

template <typename TField, typename T, long long int TValue, typename... TOptions>
class BasicIntValueBase<
    TField,
    T,
    comms::field::option::DefaultValueImpl<TValue>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;

protected:
    using Base::BasicIntValueBase;

    static const auto DefaultValue = static_cast<decltype(Base::DefaultValue)>(TValue);
};

}  // namespace details

}  // namespace field

}  // namespace comms


