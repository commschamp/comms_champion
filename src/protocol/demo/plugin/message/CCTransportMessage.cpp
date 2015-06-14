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

#include "comms_champion/Property.h"

#include "protocol/DemoMessage.h"
#include "ProtocolStack.h"


namespace cc = comms_champion;

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

QVariantMap createSyncProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Sync");
    return props;
}

QVariantMap createSizeProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Size");
    return props;
}

QVariantMap createIdProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Size");

    static const QString MsgNames[] = {
        "Heartbeat",
        "Status",
        "Serial Info",
        "Optional Test"
    };

    static const auto NumOfMsgNames = std::extent<decltype(MsgNames)>::value;

    static_assert(
        NumOfMsgNames == demo::message::MsgId_NumOfMessages,
        "Message names mapping is incorrect.");
    for (auto idx = 0U; idx < NumOfMsgNames; ++idx) {
        props.insert(cc::Property::indexedName(idx), MsgNames[idx]);
    }

    return props;
}

QVariantMap createDataProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Data");
    return props;
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(QVariant::fromValue(createSyncProperties()));
    props.append(QVariant::fromValue(createSizeProperties()));
    props.append(QVariant::fromValue(createIdProperties()));
    props.append(QVariant::fromValue(createDataProperties()));

    assert(props.size() == FieldIdx_NumOfFields);
    return props;
}

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

    cc::Property::setNameVal(fieldWidget, FieldNames[idx]);

    if (idx == FieldIdx_MsgId) {
        static const QString MsgNames[] = {
            "Heartbeat",
            "Status",
            "Serial Info",
            "Optional Test"
        };

        static const auto NumOfMsgNames = std::extent<decltype(MsgNames)>::value;

        static_assert(
            NumOfMsgNames == demo::message::MsgId_NumOfMessages,
            "Message names mapping is incorrect.");
        for (auto idx = 0U; idx < NumOfMsgNames; ++idx) {
            cc::Property::setIndexedNameVal(fieldWidget, idx, MsgNames[idx]);
        }
    }
}

const QVariantList& CCTransportMessage::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace plugin

}  // namespace demo

