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

#include "comms/options.h"
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
    comms::option::ValidNumValueRange<TMinValue, TMaxValue>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::ValidNumValueRange<TMinValue, TMaxValue> Option;

protected:
    using Base::BasicIntValueBase;

    static const auto MinValidValue =
        static_cast<decltype(Base::MinValidValue)>(Option::MinValue);
    static const auto MaxValidValue =
        static_cast<decltype(Base::MaxValidValue)>(Option::MaxValue);
};

namespace details
{

template <long long int TVal1, long long int TVal2, bool TLess>
struct MinValueHelper;

template <long long int TVal1, long long int TVal2>
struct MinValueHelper<TVal1, TVal2, true>
{
    static const long long int Value = TVal1;
};

template <long long int TVal1, long long int TVal2>
struct MinValueHelper<TVal1, TVal2, false>
{
    static const long long int Value = TVal2;
};

template <long long int TVal1, long long int TVal2>
struct MinValue
{
    static const long long int Value =
        MinValueHelper<TVal1, TVal2, (TVal1 < TVal2)>::Value;
};

template <long long int TVal1, long long int TVal2, bool TLess>
struct MaxValueHelper;

template <long long int TVal1, long long int TVal2>
struct MaxValueHelper<TVal1, TVal2, true>
{
    static const long long int Value = TVal2;
};

template <long long int TVal1, long long int TVal2>
struct MaxValueHelper<TVal1, TVal2, false>
{
    static const long long int Value = TVal1;
};

template <long long int TVal1, long long int TVal2>
struct MaxValue
{
    static const long long int Value =
        MaxValueHelper<TVal1, TVal2, (TVal1 < TVal2)>::Value;
};

}  // namespace details

template <typename TField, typename T, std::size_t TLen, typename... TOptions>
class BasicIntValueBase<
    TField,
    T,
    comms::option::FixedLength<TLen>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::FixedLength<TLen> Option;

protected:
    using Base::BasicIntValueBase;

    typedef typename util::SizeToType<TLen, std::is_signed<T>::value>::Type SerialisedType;

    static const auto MinValidValue =
        static_cast<decltype(Base::MinValidValue)>(
            details::MaxValue<
                static_cast<long long int>(std::numeric_limits<SerialisedType>::min()) - Base::Offset,
                static_cast<long long int>(Base::MinValidValue)
            >::Value);

    static const auto MaxValidValue =
        static_cast<decltype(Base::MaxValidValue)>(
            details::MinValue<
                static_cast<long long int>(std::numeric_limits<SerialisedType>::max()) - Base::Offset,
                static_cast<long long int>(Base::MaxValidValue)
            >::Value);

    static const std::size_t SerialisedLen = Option::Value;
};

template <typename TField, typename T, long long int TOff, typename... TOptions>
class BasicIntValueBase<
    TField,
    T,
    comms::option::NumValueSerOffset<TOff>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::NumValueSerOffset<TOff> Option;
protected:
    using Base::BasicIntValueBase;

    static const auto Offset = static_cast<decltype(Base::Offset)>(Option::Value);
};

template <typename TField, typename T, long long int TValue, typename... TOptions>
class BasicIntValueBase<
    TField,
    T,
    comms::option::DefaultNumValue<TValue>,
    TOptions...> : public BasicIntValueBase<TField, T, TOptions...>
{
    static_assert(std::is_integral<T>::value, "T must be integral.");

    typedef BasicIntValueBase<TField, T, TOptions...> Base;
    typedef comms::option::DefaultNumValue<TValue> Option;

protected:
    using Base::BasicIntValueBase;

    static const auto DefaultValue = static_cast<decltype(Base::DefaultValue)>(Option::Value);
};

}  // namespace details

}  // namespace field

}  // namespace comms


