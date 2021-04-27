//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file 
/// Contains definition of @ref comms::protocol::checksum::BasicSum

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



