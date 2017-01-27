//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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
#include "cc_plugin/Message.h"
#include "cc_plugin/Stack.h"

namespace comms_champion
{

namespace demo
{

namespace cc_plugin
{

namespace details
{

using TransportMessageFields =
    std::tuple<
        demo::SyncField<cc_plugin::Message::Field>,
        demo::LengthField<cc_plugin::Message::Field>,
        demo::MsgIdField<cc_plugin::Message::Field>,
        demo::DataField<cc_plugin::Message::Field>,
        demo::ChecksumField<cc_plugin::Message::Field>
    >;

}  // namespace details

class TransportMessage : public
    comms_champion::TransportMessageBase<
        cc_plugin::Message,
        details::TransportMessageFields>
{
    typedef comms_champion::TransportMessageBase<
        cc_plugin::Message,
        details::TransportMessageFields> Base;
public:
    enum FieldIdx
    {
        FieldIdx_Sync,
        FieldIdx_Len,
        FieldIdx_Id,
        FieldIdx_Payload,
        FieldIdx_Checksum,
        FieldIdx_NumOfValues
    };

    TransportMessage() = default;
    TransportMessage(const TransportMessage&) = default;
    TransportMessage(TransportMessage&&) = default;
    virtual ~TransportMessage() = default;

    TransportMessage& operator=(const TransportMessage&) = default;
    TransportMessage& operator=(TransportMessage&&) = default;

protected:
    virtual const QVariantList& fieldsPropertiesImpl() const override;
    virtual comms::ErrorStatus readImpl(ReadIterator& iter, std::size_t size) override;

};

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
