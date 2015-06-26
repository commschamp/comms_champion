//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

template <typename TResult = std::uint8_t>
class BasicSum
{
public:
    template <typename TIter>
    TResult operator()(TIter& iter, std::size_t len) const
    {
        typedef typename std::make_unsigned<
            typename std::decay<decltype(*iter)>::type
        >::type ByteType;

        auto checksum = TResult(0);
        for (auto idx = 0U; idx < len; ++idx) {
            checksum += static_cast<TResult>(static_cast<ByteType>(*iter));
            ++iter;
        }
        return checksum;
    }
};

}  // namespace checksum

}  // namespace protocol

}  // namespace comms



