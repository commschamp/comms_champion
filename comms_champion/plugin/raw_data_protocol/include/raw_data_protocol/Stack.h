//
// Copyright 2016 - 2021 (C). Alex Robenko. All rights reserved.
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
/// @brief Contains definition of transport layers protocol stack of raw_data_protocol
///     binary protocol.

#pragma once

#include "comms/comms.h"

#include "Message.h"

namespace comms_champion
{

namespace plugin
{

namespace raw_data_protocol
{

template <typename TField, typename... TOptions>
using IdField = comms::field::NoValue<TField>;

template <typename... TOptions>
using DataField = typename comms::protocol::MsgDataLayer<TOptions...>::Field;

template <
    typename TMsgBase,
    typename TDataMessage,
    typename TMsgAllocOptions = comms::option::EmptyOption,
    typename TDataFieldStorageOptions = comms::option::EmptyOption>
using Stack =
        comms::protocol::MsgIdLayer<
            IdField<typename TMsgBase::Field>,
            TMsgBase,
            std::tuple<TDataMessage>,
            comms::protocol::MsgDataLayer<
                TDataFieldStorageOptions
            >,
            TMsgAllocOptions
        >;

}  // namespace raw_data_protocol

}  // namespace plugin

}  // namespace comms_champion



