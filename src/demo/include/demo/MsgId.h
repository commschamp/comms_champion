//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

/// @file
/// @brief Contains ID enumerator for all the messages.

#pragma once

#include <cstdint>

namespace comms_champion
{

namespace demo
{

/// @brief Enumeration type of message ID.
enum MsgId : std::uint8_t
{
    MsgId_IntValues, ///< Integer values accumulating message
    MsgId_EnumValues, ///< Enum values accumulating message
    MsgId_BitmaskValues, ///< Bitmask values accumulating message
    MsgId_Bitfields, ///< Bitfields accumulating message
    MsgId_Strings, ///< Strings accumulating message
    MsgId_NumOfValues ///< Limit to valid message IDs, must be last
};

}  // namespace comms_champion

}  // namespace demo
