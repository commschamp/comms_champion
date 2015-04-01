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

#include <tuple>
#include "message/CCHeartbeat.h"
#include "message/CCStatus.h"
#include "message/CCSerialInfo.h"

#include "protocol/DemoMessage.h"

namespace demo
{

namespace plugin
{

using AllMessages =
    std::tuple<
        message::CCHeartbeat,
        message::CCStatus,
        message::CCSerialInfo
    >;

static_assert(
    std::tuple_size<AllMessages>::value == demo::message::MsgId_NumOfMessages,
    "Incorrect number of messages.");

}  // namespace plugin

}  // namespace demo


