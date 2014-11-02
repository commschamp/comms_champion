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


#include "Protocol.h"

#include "message/CCHeartbeat.h"

namespace demo
{

namespace plugin
{

namespace cc = comms_champion;

Protocol::~Protocol() = default;

cc::ErrorStatus Protocol::read(
        MsgPtr& msg,
        ReadIterType& iter,
        std::size_t size,
        std::size_t* missingSize)
{
    msg.reset(new message::CCHeartbeat());
    return cc::ErrorStatus::Success;
}

}  // namespace plugin

}  // namespace demo

