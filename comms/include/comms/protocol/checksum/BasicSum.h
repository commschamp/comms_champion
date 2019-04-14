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

namespace protocol
{

namespace checksum
{

/// @brief Summary of all bytes checksum calculator.
/// @details The checksum calculator class that sums all the bytes and
///     returns the result as a checksum value.
/// @tparam TResult Type of the checksum result value.
/// @headerfile comms/protocol/checksum/BasicSum.h
template <typename TResult = std::uint8_t>
class BasicSum
{
public:
    /// @brief Operator that is invoked to calculate the checksum value
    /// @param[in, out] iter Input iterator,
    /// @param[in] len Number of bytes to summarise.
    /// @return The checksum value.
    /// @post The iterator is advanced by number of bytes read (len).
    template <typename TIter>
    TResult operator()(TIter& iter, std::size_t len) const
    {
        using ByteType = typename std::make_unsigned<
            typename std::decay<decltype(*iter)>::type
        >::type;

        TResult checksum = 0;
        for (auto idx = 0U; idx < len; ++idx) {
            checksum = static_cast<TResult>(checksum + static_cast<ByteType>(*iter));
            ++iter;
        }
        return checksum;
    }
};

}  // namespace checksum

}  // namespace protocol

}  // namespace comms



