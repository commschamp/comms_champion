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


#pragma once

#include <cstdint>

#include "comms/comms.h"

namespace comms_champion
{

namespace plugin
{

namespace raw_data_protocol
{

template <typename... TOptions>
class MessageT : public
    comms::Message<
        TOptions...,
        comms::option::BigEndian,
        comms::option::MsgIdType<unsigned>
    >
{
public:

    MessageT() = default;
    MessageT(const MessageT&) = default;
    MessageT(MessageT&&) = default;
    virtual ~MessageT() = default;
    MessageT& operator=(const MessageT&) = default;
    MessageT& operator=(MessageT&&) = default;
};

}  // namespace raw_data_protocol

}  // namespace plugin

} // namespace comms_champion


