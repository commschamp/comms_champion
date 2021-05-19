//
// Copyright 2013 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

template <std::size_t TSize, bool TSigned = false>
using SizeToTypeT = typename SizeToType<TSize, TSigned>::Type;

/// @endcond

}  // namespace util

}  // namespace comms
