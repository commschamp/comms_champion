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

#include <memory>
#include <cstdint>
#include <cstddef>

#include "Message.h"
#include "ErrorStatus.h"

namespace comms_champion
{

class Protocol
{
public:
    typedef std::unique_ptr<Message> MsgPtr;
    typedef const std::uint8_t* ReadIterType;

    virtual ~Protocol() {}

    virtual ErrorStatus read(
        MsgPtr& msg,
        ReadIterType& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr) = 0;
};

}  // namespace comms_champion


