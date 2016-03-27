//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

namespace comms_champion
{

namespace demo
{

namespace cc_plugin
{

namespace
{

QVariantMap createMsgIdProperties()
{
    QVariantList enumValues;
    cc::Property::appendEnumValue(enumValues, "IntValues");
    assert(enumValues.size() == demo::MsgId_NumOfValues);

    return cc::Property::createPropertiesMap("ID", std::move(enumValues));
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(cc::Property::createPropertiesMap("SYNC"));
    props.append(cc::Property::createPropertiesMap("LENGTH"));
    props.append(createMsgIdProperties());
    props.append(cc::Property::createPropertiesMap("PAYLOAD"));
    props.append(cc::Property::createPropertiesMap("CHECKSUM"));
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
        sizeof(demo::ChecksumField<cc_plugin::Stack::Message::Field>::ValueType);

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

}  // namespace comms_champion
