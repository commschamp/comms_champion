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

#include "TransportMessage.h"

#include <cassert>

#include <QtCore/QVariantMap>

namespace cc = comms_champion;

namespace demo
{

namespace cc_plugin
{

namespace
{

typedef TransportMessage::Field FieldBase;
typedef demo::SyncField<FieldBase> SyncField;
typedef demo::ChecksumField<FieldBase> ChecksumField;
typedef demo::LengthField<FieldBase> LengthField;
typedef demo::MsgIdField<FieldBase> MsgIdField;
typedef demo::DataField<FieldBase> DataField;

QVariantMap createMsgIdProperties()
{
    static const char* Names[] = {
        "IntValues",
        "EnumValues",
        "BitmaskValues",
        "Bitfields",
        "Strings",
        "Lists",
        "Optionals",
        "FloatValues",
        "Variants"
    };

    static const auto NamesCount = std::extent<decltype(Names)>::value;
    static_assert(NamesCount == demo::MsgId_NumOfValues, "Not all messages are added");

    cc::property::field::ForField<MsgIdField> props;
    props.name("ID");

    QVariantList enumValues;
    for (auto name : Names) {
        props.add(name);
    }

    assert(props.values().size() == demo::MsgId_NumOfValues);
    return props.asMap();
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(cc::property::field::ForField<SyncField>().name("SYNC").asMap());
    props.append(
        cc::property::field::ForField<LengthField>()
            .name("LENGTH")
            .displayOffset(2)
            .asMap());
    props.append(createMsgIdProperties());
    props.append(cc::property::field::ForField<DataField>().name("PAYLOAD").asMap());
    props.append(cc::property::field::ForField<ChecksumField>().name("CHECKSUM").asMap());
    assert(props.size() == TransportMessage::FieldIdx_NumOfValues);
    return props;
}

}  // namespace

const QVariantList& TransportMessage::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

comms::ErrorStatus TransportMessage::readImpl(ReadIterator& iter, std::size_t size)
{
    static const auto ChecksumLen =
        sizeof(demo::ChecksumField<cc_plugin::Message::Field>::ValueType);

    size -= ChecksumLen;
    auto es = Base::readFieldsUntil<FieldIdx_Checksum>(iter, size);
    if (es == comms::ErrorStatus::Success) {
        size += ChecksumLen;
        es = readFieldsFrom<FieldIdx_Checksum>(iter, size);
    }
    return es;
}


}  // namespace cc_plugin

}  // namespace demo

