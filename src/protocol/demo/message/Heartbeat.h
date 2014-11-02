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

#include "DemoMessage.h"

namespace demo
{

namespace message
{

template <typename TFieldBase>
using HeatbeatFields =
    std::tuple<
        comms::field::BasicIntValue<
            TFieldBase,
            unsigned,
            comms::field::option::
            LengthLimitImpl<2> >
    >;

template <typename TMsgBase = DemoMessage>
class Heartbeat : public
    comms::MessageBase<
        TMsgBase,
        comms::option::NumIdImpl<MsgId_Heartbeat>,
        comms::option::FieldsImpl<HeatbeatFields<typename TMsgBase::Field> >,
        comms::option::DispatchImpl<Heartbeat<TMsgBase> >
    >
{
};

}  // namespace message

}  // namespace demo


