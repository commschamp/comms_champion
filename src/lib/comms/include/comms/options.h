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

#include "comms/traits.h"

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

template<long long int TOffset>
struct NumValueSerOffset
{
    static const auto Value = TOffset;
};

// TODO: should be alias to content validator
template<long long int TMinValue, long long int TMaxValue>
struct ValidNumValueRange
{
    static const auto MinValue = TMinValue;
    static const auto MaxValue = TMaxValue;
};

// TODO: should be alias to default initialiser
template<long long int TVal>
struct DefaultNumValue
{
    static const auto Value = TVal;
};

// TODO: should be alias to content validator
template<long long unsigned TMask, bool TValue>
struct BitmaskReservedBits
{
    static const auto Mask = TMask;
    static const auto Value = TValue;
};

// TODO: rename to Msb
struct BitIndexingStartsFromLsb {};

template <std::size_t TSize>
struct FixedSizeStorage
{
    static const std::size_t Value = TSize;
};

template <typename T>
struct DefaultValueInitialiser
{
    typedef T Type;
};

// TODO: remove
template <std::size_t TSize>
struct ValidStringMaxSize
{
    static const std::size_t Value = TSize;
};

template <typename T>
struct StringContentValidator
{
    typedef T Type;
};

// TODO: remove
template<std::size_t TLen>
struct StringSizeLength
{
    static const std::size_t Value = TLen;
};

}  // namespace option


}  // namespace comms


