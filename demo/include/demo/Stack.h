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

/// @file
/// @brief Contains definition of transport layers protocol stack of demo
///     binary protocol.

#pragma once

#include "comms/comms.h"

#include "MsgId.h"
#include "Message.h"
#include "FieldBase.h"

namespace demo
{

/// @brief Field representing synchronisation information in
///     message wrapping.
/// @details Expects <b>0xab 0xbc</b>sequence.
using SyncField =
    comms::field::IntValue<
        FieldBase,
        std::uint16_t,
        comms::option::DefaultNumValue<0xabcd>,
        comms::option::ValidNumValueRange<0xabcd, 0xabcd>
    >;

/// @brief Field representing last two checksum bytes in message wrapping.
using ChecksumField =
    comms::field::IntValue<
        FieldBase,
        std::uint16_t
    >;

/// @brief Field representing remaining length in message wrapping.
using LengthField =
    comms::field::IntValue<
        FieldBase,
        std::uint16_t,
        comms::option::NumValueSerOffset<sizeof(std::uint16_t)>
    >;

/// @brief Field representing message ID in message wrapping.
using MsgIdField =
    comms::field::EnumValue<
        FieldBase,
        MsgId,
        comms::option::ValidNumValueRange<0, MsgId_NumOfValues - 1>
    >;

/// @brief Field representing full message payload.
template <typename... TOptions>
using DataField = typename comms::protocol::MsgDataLayer<TOptions...>::Field;

/// @brief Definition of Demo binary protocol stack of layers.
/// @details It is used to process incoming binary stream of data and create
///     allocate message objects for received messages. It also responsible to
///     serialise outgoing messages and wrap their payload with appropriate transport
///     information. See <b>Protocol Stack Tutorial</b>
///     page in @b COMMS library tutorial for more information.@n
///     The outermost layer is
///     @b comms::protocol::SyncPrefixLayer.
///     Please see its documentation for public interface description.
/// @tparam TMsgBase Interface class for all the messages, expected to be some
///     variant of demo::MessageT class with options.
/// @tparam TMessages Types of all messages that this protocol stack must
///     identify during read and support creation of proper message object.
///     The types of the messages must be bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
/// @tparam TMsgAllocOptions Template parameter(s) passed as options to
///     @b comms::protocol::MsgIdLayer
///     protocol layer in @b COMMS library. They are used to specify whether
///     dynamic memory allocation is allowed or "in place" allocation for
///     message objects must be implemented. It is expected to be either
///     single @b COMMS library option or multiple options bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
/// @tparam TDataFieldStorageOptions Extra parameter(s) that are passed
///     to @b comms::protocol::MsgDataLayer rotocol layer in @b COMMS library.
///     It may be used to choose storage
///     type of the payload field for "caching" read/write operations.
template <
    typename TMsgBase,
    typename TMessages,
    typename TMsgAllocOptions = comms::option::EmptyOption,
    typename TDataFieldStorageOptions = comms::option::EmptyOption >
struct Stack : public
    comms::protocol::SyncPrefixLayer<
        SyncField,
        comms::protocol::ChecksumLayer<
            ChecksumField,
            comms::protocol::checksum::BasicSum<std::uint16_t>,
            comms::protocol::MsgSizeLayer<
                LengthField,
                comms::protocol::MsgIdLayer<
                    MsgIdField,
                    TMsgBase,
                    TMessages,
                    comms::protocol::TransportValueLayer<
                        VersionField,
                        Message<>::TransportFieldIdx_version,
                        comms::protocol::MsgDataLayer<
                            TDataFieldStorageOptions
                        >
                    >,
                    TMsgAllocOptions
                >
            >
        >
    >
{
    COMMS_PROTOCOL_LAYERS_ACCESS(payload, version, id, size, checksum, sync);
};

}  // namespace demo



