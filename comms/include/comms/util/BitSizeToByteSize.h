//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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


