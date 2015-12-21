//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <utility>
#include <type_traits>
#include "comms_champion/ErrorStatus.h"

namespace comms_champion
{

ErrorStatus transformErrorStatus(comms::ErrorStatus value)
{
    static const ErrorStatus Map[] = {
        /* Success */ ErrorStatus::Success,
        /* UpdateRequired */ ErrorStatus::NumOfErrorStatuses,
        /* NotEnoughData */ ErrorStatus::NotEnoughData,
        /* ProtocolError */ ErrorStatus::ProtocolError,
        /* BufferOverflow */ ErrorStatus::BufferOverflow,
        /* InvalidMsgId */ ErrorStatus::InvalidMsgId,
        /* InvalidMsgData */ ErrorStatus::InvalidMsgData,
        /* MsgAllocFailure */ ErrorStatus::MsgAllocFailure
    };

    using UnderlyingType = typename std::underlying_type<decltype(value)>::type;
    static const auto castedLimit =
        static_cast<UnderlyingType>(comms::ErrorStatus::NumOfErrorStatuses);
    static_assert(std::extent<decltype(Map)>::value == castedLimit,
        "comms::ErrorStatus has changed, update to transformation map is required");


    auto castedValue = static_cast<UnderlyingType>(value);

    if (castedLimit <= castedValue) {
        return ErrorStatus::NumOfErrorStatuses;
    }

    return Map[castedValue];
}

}  // namespace comms_champion


