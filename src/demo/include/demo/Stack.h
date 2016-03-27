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

namespace comms_champion
{

namespace demo
{

/// @brief Field representing synchronisation information in
///     message wrapping.
/// @details Expects <b>0xab 0xbc</b>sequence.
template <typename TField>
using SyncField =
    comms::field::IntValue<
        TField,
        std::uint16_t,
        comms::option::DefaultNumValue<0xabcd>,
        comms::option::ValidNumValueRange<0xabcd, 0xabcd>
    >;

/// @brief Field representing last two checksum bytes in message wrapping.
template <typename TField>
using ChecksumField =
    comms::field::IntValue<
        TField,
        std::uint16_t
    >;

/// @brief Field representing remaining length in message wrapping.
template <typename TField>
using LengthField =
    comms::field::IntValue<
        TField,
        std::uint16_t,
        comms::option::NumValueSerOffset<sizeof(std::uint16_t)>
    >;

/// @brief Field representing message ID in message wrapping.
template <typename TField>
using MsgIdField =
    comms::field::EnumValue<
        TField,
        MsgId,
        comms::option::ValidNumValueRange<0, MsgId_NumOfValues - 1>
    >;

/// @brief Field representing full message payload.
template <typename TField, typename... TOptions>
using DataField =
    comms::field::ArrayList<
        TField,
        std::uint8_t,
        TOptions...
    >;

/// @brief Definition of Demo binary protocol stack of layers.
/// @details It is used to process incoming binary stream of data and create
///     allocate message objects for received messages. It also responsible to
///     serialise outgoing messages and wrap their payload with appropriate transport
///     information. See <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/page_prot_stack_tutorial.html">Protocol Stack Tutorial</a>
///     page in @b COMMS library tutorial for more information.@n
///     The outermost layer is
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1protocol_1_1SyncPrefixLayer.html">comms::protocol::SyncPrefixLayer</a>.
///     Please see its documentation for public interface description.
/// @tparam TMsgBase Interface class for all the messages, expected to be some
///     variant of demo::MessageT class with options.
/// @tparam TMessages Types of all messages that this protocol stack must
///     identify during read and support creation of proper message object.
///     The types of the messages must be bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
/// @tparam TMsgAllocOptions The contents of this template parameter are passed
///     as options to
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1protocol_1_1MsgIdLayer.html">comms::protocol::MsgIdLayer</a>
///     protocol layer in @b COMMS library. They are used to specify whether
///     dynamic memory allocation is allowed or "in place" allocation for
///     message objects must be implemented. It is expected to be either
///     single @b COMMS library option or multiple options bundled in
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
/// @tparam TDataFieldStorageOptions The contents of this template parameters
///     are passed to the definition of storage field of
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1protocol_1_1MsgDataLayer.html">comms::protocol::MsgDataLayer</a>
///     layer. The field is a variant of
///     <a href="https://dl.dropboxusercontent.com/u/46999418/comms_champion/comms/html/classcomms_1_1field_1_1ArrayList.html">comms::field::ArrayList</a>
///     which uses <a href="http://en.cppreference.com/w/cpp/container/vector">std::vector</a>
///     as its internal storage by default. The option(s) specified in this
///     template parameter is/are forwarded to the definition of the storage
///     field (comms::field::ArrayList).
template <
    typename TMsgBase,
    typename TMessages,
    typename TMsgAllocOptions = std::tuple<>,
    typename TDataFieldStorageOptions = std::tuple<> >
using Stack =
    comms::protocol::SyncPrefixLayer<
        SyncField<typename TMsgBase::Field>,
        comms::protocol::ChecksumLayer<
            ChecksumField<typename TMsgBase::Field>,
            comms::protocol::checksum::BasicSum<std::uint16_t>,
            comms::protocol::MsgSizeLayer<
                LengthField<typename TMsgBase::Field>,
                comms::protocol::MsgIdLayer<
                    MsgIdField<typename TMsgBase::Field>,
                    TMessages,
                    comms::protocol::MsgDataLayer<
                        TMsgBase,
                        DataField<typename TMsgBase::Field, TDataFieldStorageOptions>
                    >,
                    TMsgAllocOptions
                >
            >
        >
    >;

}  // namespace demo

}  // namespace comms_champion



