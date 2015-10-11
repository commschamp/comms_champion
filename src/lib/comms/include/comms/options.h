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

#include <tuple>
#include <type_traits>
#include <limits>
#include <ratio>

#include "comms/traits.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace option
{

// Message/Field common options

template <typename TEndian>
struct Endian
{
    typedef TEndian Type;
};

using BigEndian = Endian<comms::traits::endian::Big>;

using LittleEndian = Endian<comms::traits::endian::Little>;

struct EmptyOption {};

// Message interface options
template <typename T>
struct MsgIdType
{
    typedef T Type;
};

template <typename TIter>
struct ReadIterator
{
    typedef TIter Type;
};

template <typename TIter>
struct WriteIterator
{
    typedef TIter Type;
};

template <typename T>
struct Handler
{
    typedef T Type;
};

struct ValidCheckInterface {};


template <std::intmax_t TId>
struct StaticNumIdImpl
{
    static const auto Value = TId;
};

template <typename TActual>
struct DispatchImpl
{
    typedef TActual MsgType;
};

template <typename TFields>
struct FieldsImpl;

template <typename... TFields>
struct FieldsImpl<std::tuple<TFields...> >
{
    typedef std::tuple<TFields...> Fields;
};

using NoFieldsImpl = FieldsImpl<std::tuple<> >;

struct NoIdImpl {};

template<std::size_t TLen>
struct FixedLength
{
    static const std::size_t Value = TLen;
};

template<std::size_t TLen>
struct FixedBitLength
{
    static const std::size_t Value = TLen;
};

template<std::size_t TMin, std::size_t TMax>
struct VarLength
{
    static_assert(TMin <= TMax, "TMin must not be greater that TMax.");
    static const std::size_t MinValue = TMin;
    static const std::size_t MaxValue = TMax;
};


template<std::intmax_t TOffset>
struct NumValueSerOffset
{
    static const auto Value = TOffset;
};

template <std::size_t TSize>
struct FixedSizeStorage
{
    static const std::size_t Value = TSize;
};

template <std::intmax_t TNum, std::intmax_t TDenom>
struct ScalingRatio
{
    typedef std::ratio<TNum, TDenom> Type;
};

struct InPlaceAllocation {};

template <typename TField>
struct SequenceSizeFieldPrefix
{
    typedef TField Type;
};

template <typename TField>
struct SequenceTrailingFieldSuffix
{
    typedef TField Type;
};

struct SequenceSizeForcingEnabled
{
};

template <std::size_t TSize>
struct SequenceFixedSize
{
    static const std::size_t Value = TSize;
};

template <typename T>
struct DefaultValueInitialiser
{
    typedef T Type;
};

template <typename T>
struct ContentsValidator
{
    typedef T Type;
};

struct FailOnInvalid {};

struct IgnoreInvalid {};

namespace details
{

template<std::intmax_t TVal>
struct DefaultNumValueInitialiser
{
    template <typename TField>
    void operator()(TField&& field)
    {
        typedef typename std::decay<TField>::type FieldType;
        typedef typename FieldType::ValueType ValueType;
        field.value() = static_cast<ValueType>(TVal);
    }
};

template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
struct NumValueRangeValidator
{
    static_assert(
        TMinValue <= TMaxValue,
        "Min value must be not greater than Max value");

    template <typename TField>
    constexpr bool operator()(const TField& field) const
    {
        typedef typename std::conditional<
            (std::numeric_limits<decltype(MinValue)>::min() < MinValue),
            CompareTag,
            ReturnTrueTag
        >::type MinTag;

        typedef typename std::conditional<
            (MaxValue < std::numeric_limits<decltype(MaxValue)>::max()),
            CompareTag,
            ReturnTrueTag
        >::type MaxTag;

        return aboveMin(field.value(), MinTag()) && belowMax(field.value(), MaxTag());
    }

private:
    struct ReturnTrueTag {};
    struct CompareTag {};

    template <typename TValue>
    static constexpr bool aboveMin(const TValue& value, CompareTag)
    {
        typedef typename std::decay<decltype(value)>::type ValueType;
        return (static_cast<ValueType>(MinValue) <= value);
    }

    template <typename TValue>
    static constexpr bool aboveMin(const TValue&, ReturnTrueTag)
    {
        return true;
    }

    template <typename TValue>
    static constexpr bool belowMax(const TValue& value, CompareTag)
    {
        typedef typename std::decay<decltype(value)>::type ValueType;
        return (value <= static_cast<ValueType>(MaxValue));
    }

    template <typename TValue>
    static constexpr bool belowMax(const TValue&, ReturnTrueTag)
    {
        return true;
    }


    static const auto MinValue = TMinValue;
    static const auto MaxValue = TMaxValue;
};

template<std::uintmax_t TMask, std::uintmax_t TValue>
struct BitmaskReservedBitsValidator
{
    template <typename TField>
    constexpr bool operator()(TField&& field) const
    {
        typedef typename std::decay<TField>::type FieldType;
        typedef typename FieldType::ValueType ValueType;

        return (field.value() & static_cast<ValueType>(TMask)) == static_cast<ValueType>(TValue);
    }
};

}  // namespace details

template<std::intmax_t TVal>
using DefaultNumValue = DefaultValueInitialiser<details::DefaultNumValueInitialiser<TVal> >;

template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
using ValidNumValueRange = ContentsValidator<details::NumValueRangeValidator<TMinValue, TMaxValue> >;

template<std::uintmax_t TMask, std::uintmax_t TValue>
using BitmaskReservedBits = ContentsValidator<details::BitmaskReservedBitsValidator<TMask, TValue> >;


}  // namespace option

}  // namespace comms


