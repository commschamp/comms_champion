//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <tuple>
#include <cstdint>

#include "protocol/DemoMessage.h"

namespace demo
{

namespace message
{

template <typename TFieldBase>
using SerialInfoFields =
    std::tuple<
        comms::field::StaticString<
            TFieldBase,
            comms::field::option::SetValidStringMaxSize<64> >
    >;

template <typename TMsgBase = DemoMessage>
class SerialInfo : public
    comms::MessageBase<
        TMsgBase,
        comms::option::NumIdImpl<MsgId_SerialInfo>,
        comms::option::FieldsImpl<SerialInfoFields<typename TMsgBase::Field> >,
        comms::option::DispatchImpl<SerialInfo<TMsgBase> >
    >
{
public:
    enum FieldId {
        FieldId_Device,
        FieldId_NumOfFields
    };
};

}  // namespace message

}  // namespace demo





