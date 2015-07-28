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

#include "comms/traits.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace option
{

template <long long int TId>
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

struct NoFieldsImpl {};

struct NoIdImpl {};

template <typename TEndian>
struct Endian
{
    typedef TEndian Type;
};

using BigEndian = Endian<comms::traits::endian::Big>;

using LittleEndian = Endian<comms::traits::endian::Little>;

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


template<long long int TOffset>
struct NumValueSerOffset
{
    static const auto Value = TOffset;
};

template <std::size_t TSize>
struct FixedSizeStorage
{
    static const std::size_t Value = TSize;
};

struct InPlaceAllocation {};

template <typename TField>
struct SequenceSizeFieldPrefix
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

template<long long int TVal>
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

template<long long int TMinValue, long long int TMaxValue>
struct NumValueRangeValidator
{
    static_assert(
        TMinValue <= TMaxValue,
        "Min value must be not greater than Max value");

    template <typename TField>
    constexpr bool operator()(TField&& field) const
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
    static constexpr bool aboveMin(TValue&& value, CompareTag)
    {
        return (MinValue <= static_cast<decltype(MinValue)>(value));
    }

    template <typename TValue>
    static constexpr bool aboveMin(TValue&&, ReturnTrueTag)
    {
        return true;
    }

    template <typename TValue>
    static constexpr bool belowMax(TValue&& value, CompareTag)
    {
        return (static_cast<decltype(MaxValue)>(value) <= MaxValue);
    }

    template <typename TValue>
    static constexpr bool belowMax(TValue&&, ReturnTrueTag)
    {
        return true;
    }


    static const auto MinValue = TMinValue;
    static const auto MaxValue = TMaxValue;
};

template<long long int TMask, long long int TValue>
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

template<long long int TVal>
using DefaultNumValue = DefaultValueInitialiser<details::DefaultNumValueInitialiser<TVal> >;

template<long long int TMinValue, long long int TMaxValue>
using ValidNumValueRange = ContentsValidator<details::NumValueRangeValidator<TMinValue, TMaxValue> >;

template<long long unsigned TMask, long long unsigned TValue>
using BitmaskReservedBits = ContentsValidator<details::BitmaskReservedBitsValidator<TMask, TValue> >;


}  // namespace option

}  // namespace comms


