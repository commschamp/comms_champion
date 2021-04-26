//
// Copyright 2016 - 2021 (C). Alex Robenko. All rights reserved.
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

#include "raw_data_protocol/Stack.h"
#include "Message.h"
#include "DataMessage.h"

namespace comms_champion
{

namespace plugin
{

namespace raw_data_protocol
{

namespace cc_plugin
{

typedef raw_data_protocol::Stack<
    cc_plugin::Message,
    cc_plugin::DataMessage
> Stack;

}  // namespace cc_plugin

}  // namespace raw_data_protocol

}  // namespace plugin

}  // namespace comms_champion

