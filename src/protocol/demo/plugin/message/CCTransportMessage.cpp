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


#include "CCTransportMessage.h"

#include <cassert>
#include <type_traits>

#include "protocol/DemoMessage.h"
#include "ProtocolStack.h"

namespace demo
{

namespace plugin
{

namespace message
{

namespace
{

enum FieldIdx
{
    FieldIdx_Sync,
    FieldIdx_Size,
    FieldIdx_MsgId,
    FieldIdx_Data,
    FieldIdx_NumOfFields
};

static_assert(
    demo::plugin::ProtocolStack::NumOfLayers == FieldIdx_NumOfFields,
    "Incorrect assumption about protocols layers");

const char* FieldNames[] = {
    "Sync",
    "Size",
    "ID",
    "Data"
};

static_assert(
    std::extent<decltype(FieldNames)>::value == FieldIdx_NumOfFields,
    "FieldNames array must be updated.");

}  // namespace

const char* CCTransportMessage::nameImpl() const
{
    static const char* Str = "Demo Protocol Transport Message";
    return Str;
}

void CCTransportMessage::updateFieldPropertiesImpl(
    QWidget& fieldWidget,
    uint idx) const
{
    if (FieldIdx_NumOfFields <= idx) {
        assert(idx < FieldIdx_NumOfFields);
        return;
    }

    setNameProperty(fieldWidget, FieldNames[idx]);

    if (idx == FieldIdx_MsgId) {
        setIndexedNameProperty(fieldWidget, demo::message::MsgId_Heartbeat, "Heartbeat");
        setIndexedNameProperty(fieldWidget, demo::message::MsgId_Status, "Status");
    }
}

}  // namespace message

}  // namespace plugin

}  // namespace demo

