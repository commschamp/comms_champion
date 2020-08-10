//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>

namespace comms
{

namespace util
{

/// @cond SKIP_DOC
template <std::size_t TSize>
struct BitSizeToByteSize
{
    static_assert(0 < TSize, "The number of bits must be greater than 0");
    static_assert(TSize < 64, "The number of bits is too high.");
    static const std::size_t Value = BitSizeToByteSize<TSize + 1>::Value;
};

template <>
struct BitSizeToByteSize<8>
{
    static const std::size_t Value = sizeof(std::uint8_t);
};

template <>
struct BitSizeToByteSize<16>
{
    static const std::size_t Value = sizeof(std::uint16_t);
};

template <>
struct BitSizeToByteSize<32>
{
    static const std::size_t Value = sizeof(std::uint32_t);
};

template <>
struct BitSizeToByteSize<64>
{
    static const std::size_t Value = sizeof(std::uint64_t);
};

/// @endcond

}  // namespace util

}  // namespace comms


