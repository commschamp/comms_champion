//
// Copyright 2016 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/comms_champion.h"
#include "raw_data_protocol/Message.h"

namespace comms_champion
{

namespace plugin
{

namespace raw_data_protocol
{

namespace cc_plugin
{

template <typename... TOptions>
class MessageT :
    public comms_champion::MessageBase<raw_data_protocol::MessageT, TOptions...>
{
    typedef comms_champion::MessageBase<raw_data_protocol::MessageT, TOptions...> Base;
public:
    virtual ~MessageT() {}

protected:

    virtual QString idAsStringImpl() const override
    {
        return "-";
    }

    virtual const QVariantList& fieldsPropertiesImpl() const override
    {
        static const QVariantList Props;
        return Props;
    }
};

typedef MessageT<> Message;

}  // namespace cc_plugin

}  // namespace raw_data_protocol

}  // namespace plugin

}  // namespace comms_champion
