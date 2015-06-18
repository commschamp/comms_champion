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

enum OptionalEnum
{
    OptionalEnum_Val1,
    OptionalEnum_Val2,
    OptionalEnum_Val3,
    OptionalEnum_NumOfValues
};

template <typename TFieldBase>
using OptionalTestFields =
    std::tuple<
        comms::field::BitmaskValue<
            TFieldBase,
            comms::option::FixedLength<1>,
            comms::option::BitmaskReservedBits<0xfffc, 0x0>
        >,
        comms::field::Optional<
            comms::field::EnumValue<
                TFieldBase,
                OptionalEnum,
                comms::option::FixedLength<1>,
                comms::option::ValidNumValueRange<OptionalEnum_Val1, OptionalEnum_NumOfValues - 1>
            >
        >,
        comms::field::Optional<
            comms::field::IntValue<
                TFieldBase,
                std::uint16_t
            >
        >
    >;

template <typename TMsgBase = DemoMessage>
class OptionalTest : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_OptionalTest>,
        comms::option::FieldsImpl<OptionalTestFields<typename TMsgBase::Field> >,
        comms::option::DispatchImpl<OptionalTest<TMsgBase> >
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgId_OptionalTest>,
        comms::option::FieldsImpl<OptionalTestFields<typename TMsgBase::Field> >,
        comms::option::DispatchImpl<OptionalTest<TMsgBase> > >Base;
public:
    enum FieldId {
        FieldId_Flags,
        FieldId_OptEnum,
        FieldId_OptInt,
        FieldId_NumOfFields
    };

    OptionalTest()
    {
        auto& fields = Base::getFields();
        auto& optEnumField = std::get<FieldId_OptEnum>(fields);
        auto& optIntField = std::get<FieldId_OptInt>(fields);

        optEnumField.setMode(comms::field::OptionalMode::Missing);
        optIntField.setMode(comms::field::OptionalMode::Missing);
    }

protected:
    virtual bool validImpl() const override
    {
        if (!Base::validImpl()) {
            return false;
        }

        auto& fields = Base::getFields();
        auto& flagsField = std::get<FieldId_Flags>(fields);
        auto& optEnumField = std::get<FieldId_OptEnum>(fields);
        auto& optIntField = std::get<FieldId_OptInt>(fields);

        auto mask = flagsField.value();
        bool enumExists = ((mask & 0x1) != 0);
        if ((enumExists) &&
            (optEnumField.getMode() != comms::field::OptionalMode::Exists)) {
            return false;
        }

        if ((!enumExists) &&
            (optEnumField.getMode() != comms::field::OptionalMode::Missing)) {
            return false;
        }

        bool intExists = ((mask & 0x2) != 0);
        if ((intExists) &&
            (optIntField.getMode() != comms::field::OptionalMode::Exists)) {
            return false;
        }

        if ((!intExists) &&
            (optIntField.getMode() != comms::field::OptionalMode::Missing)) {
            return false;
        }

        return true;
    }
};

}  // namespace message

}  // namespace demo


