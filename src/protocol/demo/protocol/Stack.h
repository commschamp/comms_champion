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

#include "comms/comms.h"
#include "protocol/DemoMessage.h"

namespace demo
{

namespace protocol
{

const std::uint16_t SyncPrefixValue = 0x689f;

template <typename TMsgBase, typename TAllMessages>
using Stack =
    comms::protocol::SyncPrefixLayer<
        comms::field::ComplexIntValue<
            typename TMsgBase::Field,
            std::uint16_t,
            comms::option::DefaultNumValue<SyncPrefixValue> >,
        comms::protocol::MsgSizeLayer<
            comms::field::IntValue<typename TMsgBase::Field, std::uint16_t>,
            comms::protocol::MsgIdLayer<
                comms::field::BasicEnumValue<
                    typename TMsgBase::Field,
                    demo::message::MsgId,
                    comms::option::FixedLength<1>,
                    comms::option::ValidNumValueRange<0, demo::message::MsgId_NumOfMessages - 1>>,
                TAllMessages,
                comms::protocol::MsgDataLayer<TMsgBase>
            >
        >
    >;

}  // namespace protocol

}  // namespace demo


