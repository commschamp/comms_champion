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

QVariantMap createSyncProperties()
{
    return cc::Property::createPropertiesMap("Sync");
}

QVariantMap createSizeProperties()
{
    return cc::Property::createPropertiesMap("Size");
}

QVariantMap createIdProperties()
{
    auto props = cc::Property::createPropertiesMap("ID");

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
    QVariantList idProps;
    for (auto idx = 0U; idx < NumOfMsgNames; ++idx) {
        cc::Property::appendEnumValue(idProps, MsgNames[idx], idx);
    }

    cc::Property::setData(props, std::move(idProps));
    return props;
}

QVariantMap createDataProperties()
{
    return cc::Property::createPropertiesMap("Data");
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

const QVariantList& CCTransportMessage::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace plugin

}  // namespace demo

