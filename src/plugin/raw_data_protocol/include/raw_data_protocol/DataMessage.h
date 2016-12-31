//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "Message.h"

namespace comms_champion
{

namespace plugin
{

namespace raw_data_protocol
{

template <typename TFieldBase>
struct DataMessageFields
{
    using data =
        comms::field::ArrayList<
            TFieldBase,
            std::uint8_t
        >;


    /// @brief All the fields bundled in std::tuple.
    using All = std::tuple<
        data
    >;
};

template <typename TMsgBase>
class DataMessage : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<0>,
        comms::option::FieldsImpl<typename DataMessageFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<DataMessage<TMsgBase> >,
        comms::option::DispatchImpl
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<0>,
        comms::option::FieldsImpl<typename DataMessageFields<typename TMsgBase::Field>::All>,
        comms::option::MsgType<DataMessage<TMsgBase> >,
        comms::option::DispatchImpl
    > Base;
public:

    enum FieldIdx
    {
        FieldIdx_data,
        FieldIdx_numOfValues
    };

    static_assert(std::tuple_size<typename Base::AllFields>::value == FieldIdx_numOfValues,
        "Number of fields is incorrect");

    DataMessage() = default;
    DataMessage(const DataMessage&) = default;
    DataMessage(DataMessage&& other) = default;
    virtual ~DataMessage() = default;
    DataMessage& operator=(const DataMessage&) = default;
    DataMessage& operator=(DataMessage&&) = default;
};

}  // namespace raw_data_protocol

}  // namespace plugin

}  // namespace comms_champion


