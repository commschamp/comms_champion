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

#include <cstdint>
#include "comms/comms.h"

namespace demo
{

namespace message
{

enum MsgId
{
    MsgId_Heartbeat,
    MsgId_Status,
    MsgId_NumOfMessages
};

struct DemoDefaultTraits
{
    typedef MsgId MsgIdType;
    typedef comms::traits::endian::Big Endianness;
    typedef const std::uint8_t* ReadIterator;
    typedef std::uint8_t* WriteIterator;
};

template <typename TTraits = DemoDefaultTraits>
using DemoMessageT = comms::Message<TTraits>;

using DemoMessage = DemoMessageT<DemoDefaultTraits>;

}  // namespace message

}  // namespace demo


