//
// Copyright 2013 - 2019 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

#include <array>
#include <cstdint>

namespace comms
{

namespace util
{

namespace details
{

template <std::size_t TSize>
struct SizeToTypeHelper {
    using Type = std::array<std::uint8_t, TSize>;
};

template <>
struct SizeToTypeHelper<1>
{
    using Type = std::uint8_t;
};

template <>
struct SizeToTypeHelper<2>
{
    using Type = std::uint16_t;
};

template <>
struct SizeToTypeHelper<4>
{
    using Type = std::uint32_t;
};

template <>
struct SizeToTypeHelper<8>
{
    using Type = std::uint64_t;
};

template <>
struct SizeToTypeHelper<3>
{
    using Type = SizeToTypeHelper<4>::Type;
};

template <>
struct SizeToTypeHelper<5>
{
    using Type = SizeToTypeHelper<8>::Type;
};

template <>
struct SizeToTypeHelper<6>
{
    using Type = SizeToTypeHelper<8>::Type;
};

template <>
struct SizeToTypeHelper<7>
{
    using Type = SizeToTypeHelper<8>::Type;
};


}  // namespace details

/// @cond SKIP_DOC

template <std::size_t TSize, bool TSigned = false>
class SizeToType
{
    using ByteType = typename SizeToType<1, TSigned>::Type;

public:
    using Type = std::array<ByteType, TSize>;
};

template <std::size_t TSize>
struct SizeToType<TSize, false>
{
    using Type = typename details::SizeToTypeHelper<TSize>::Type;
};

template <std::size_t TSize>
struct SizeToType<TSize, true>
{
    using Type = typename
        std::make_signed<
            typename SizeToType<TSize, false>::Type
        >::type;
};

/// @endcond

}  // namespace util

}  // namespace comms
