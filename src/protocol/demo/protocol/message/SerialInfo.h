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

enum class Parity : std::uint8_t
{
    None,
    Odd,
    Even,
    NumOfValues
};

enum class StopBit : std::uint8_t
{
    None,
    One,
    OneAndHalf,
    Two,
    NumOfValues
};

template <typename TFieldBase>
using SerialInfoParityField =
    comms::field::BasicEnumValue<
        TFieldBase,
        Parity,
        comms::option::FixedLength<1>,
        comms::option::ValidNumValueRange<(int)Parity::None, (int)Parity::NumOfValues - 1>
    >;

template <typename TFieldBase>
using SerialInfoStopBitField =
    comms::field::BasicEnumValue<
        TFieldBase,
        StopBit,
        comms::option::FixedLength<1>,
        comms::option::ValidNumValueRange<(int)StopBit::None, (int)StopBit::NumOfValues - 1>
    >;

template <typename TFieldBase>
using SerialInfoQosField =
    comms::field::BasicIntValue<
        TFieldBase,
        std::uint8_t,
        comms::option::FixedLength<1>,
        comms::option::ValidNumValueRange<0, 3>
    >;

template <typename TFieldBase>
using SerialInfoFlagsField =
    comms::field::BitmaskValue<
        TFieldBase,
        comms::option::FixedLength<1>,
        comms::option::BitmaskReservedBits<0xfd, 0x0>
    >;

template <typename TFieldBase>
using SerialInfoFields =
    std::tuple<
        comms::field::String<
            TFieldBase,
            comms::field::BasicIntValue<
                TFieldBase,
                std::uint8_t,
                comms::option::ValidNumValueRange<0, 32>
            >
        >,
        comms::field::BasicIntValue<
            TFieldBase,
            std::uint16_t,
            comms::option::VarLength<1, 2>
        >,
        comms::field::Bitfield<
            TFieldBase,
            std::tuple<
                comms::field::BitfieldMember<SerialInfoParityField<TFieldBase>, 2>,
                comms::field::BitfieldMember<SerialInfoStopBitField<TFieldBase>, 2>,
                comms::field::BitfieldMember<SerialInfoQosField<TFieldBase>, 2>,
                comms::field::BitfieldMember<SerialInfoFlagsField<TFieldBase>, 2>
            >,
            comms::option::BitIndexingStartsFromMsb
        >
    >;

template <typename TMsgBase = DemoMessage>
class SerialInfo : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_SerialInfo>,
        comms::option::FieldsImpl<SerialInfoFields<typename TMsgBase::Field> >,
        comms::option::DispatchImpl<SerialInfo<TMsgBase> >
    >
{
public:
    enum FieldId {
        FieldId_Device,
        FieldId_Baud,
        FieldId_Flags,
        FieldId_NumOfFields
    };
};

}  // namespace message

}  // namespace demo





