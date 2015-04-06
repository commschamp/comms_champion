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


}  // namespace option


}  // namespace comms


