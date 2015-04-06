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
#include <cstdint>

#include "protocol/DemoMessage.h"

namespace demo
{

namespace message
{

enum ExecutionStatus
{
    ExecutionStatus_Idle,
    ExecutionStatus_Running,
    ExecutionStatus_Complete,
    ExecutionStatus_Error,
    ExecutionStatus_NumOfStatuses
};

template <typename TFieldBase>
using StatusFields =
    std::tuple<
        comms::field::BasicEnumValue<
            TFieldBase,
            ExecutionStatus,
            comms::field::option::LengthLimitImpl<1>,
            comms::field::option::ValidRangeImpl<ExecutionStatus_Idle, ExecutionStatus_NumOfStatuses - 1> >,
        comms::field::BitmaskValue<
            TFieldBase,
            comms::field::option::LengthLimitImpl<2>,
            comms::field::option::BitmaskReservedBitsImpl<0xfff0, false>,
            comms::field::option::BitmaskBitZeroIsLsbImpl
        >
    >;

template <typename TMsgBase = DemoMessage>
class Status : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_Status>,
        comms::option::FieldsImpl<StatusFields<typename TMsgBase::Field> >,
        comms::option::DispatchImpl<Status<TMsgBase> >
    >
{
public:
    enum FieldId {
        FieldId_ExecutionStatus,
        FieldId_Features,
        FieldId_NumOfFields
    };
};

}  // namespace message

}  // namespace demo


