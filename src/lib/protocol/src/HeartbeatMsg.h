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

#include "comms_champion/protocol/Message.h"

namespace comms_champion
{

namespace protocol
{

class HeartbeatMsg : public Message
{
    typedef Message Base;
public:

    HeartbeatMsg() = default;
    HeartbeatMsg(const HeartbeatMsg&) = default;
    ~HeartbeatMsg() = default;

    HeartbeatMsg& operator=(const HeartbeatMsg&) = default;

protected:
    virtual const char* nameImpl() const override;

};

}  // namespace protocol

}  // namespace comms_champion


