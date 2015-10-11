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


#include "CCSerialInfo.h"

#include <type_traits>
#include <cassert>

#include <QtCore/QVariant>
#include <QtCore/QVariantMap>

#include "comms_champion/Property.h"

namespace demo
{

namespace plugin
{

namespace message
{

namespace cc = comms_champion;

namespace
{

const char* SerialInfoName = "Serial Info";

QVariantMap getParityMemberData()
{
    QVariantList valuesData;
    cc::Property::appendEnumValue(valuesData, "None", (int)demo::message::Parity::None);
    cc::Property::appendEnumValue(valuesData, "Odd", (int)demo::message::Parity::Odd);
    cc::Property::appendEnumValue(valuesData, "Even", (int)demo::message::Parity::Even);
    GASSERT(valuesData.size() == (int)demo::message::Parity::NumOfValues);

    auto props = cc::Property::createPropertiesMap("Parity", std::move(valuesData));
    cc::Property::setSerialisedHidden(props, true);
    return props;
}

QVariantMap getStopBitsMemberData()
{
    QVariantList valuesData;
    cc::Property::appendEnumValue(valuesData, "None", (int)demo::message::StopBit::None);
    cc::Property::appendEnumValue(valuesData, "One", (int)demo::message::StopBit::One);
    cc::Property::appendEnumValue(valuesData, "One and a Half", (int)demo::message::StopBit::OneAndHalf);
    cc::Property::appendEnumValue(valuesData, "Two", (int)demo::message::StopBit::Two);
    GASSERT(valuesData.size() == (int)demo::message::StopBit::NumOfValues);

    auto props = cc::Property::createPropertiesMap("Stop Bits", std::move(valuesData));
    cc::Property::setSerialisedHidden(props, true);
    return props;
}

QVariantMap getFlagsMemberData()
{
    QVariantList bitNames;
    bitNames.append(QVariant());
    bitNames.append("HW_FLOW_CTRL");
    auto props = cc::Property::createPropertiesMap("Flags", std::move(bitNames));
    cc::Property::setSerialisedHidden(props, true);
    return props;
}

QVariantMap getQosMemberData()
{
    auto props = cc::Property::createPropertiesMap("QoS");
    cc::Property::setSerialisedHidden(props, true);
    return props;
}

const QVariantMap& getMemberData(std::size_t idx)
{
    static const QVariantMap Map[] = {
        getParityMemberData(),
        getStopBitsMemberData(),
        getQosMemberData(),
        getFlagsMemberData()
    };

    static const auto DataCount = std::extent<decltype(Map)>::value;

    typedef typename
        std::decay<
            decltype(
                std::get<CCSerialInfo::FieldId_Flags>(std::declval<CCSerialInfo::AllFields>()).value())
        >::type FlagsMembers;
    static const auto NumOfFlagsMembers = std::tuple_size<FlagsMembers>::value;

    static_assert(
        DataCount == NumOfFlagsMembers,
        "Data for some member fields is missing.");

    if (DataCount <= idx) {
        static const QVariantMap EmptyMap;
        return EmptyMap;
    }

    return Map[idx];
}

QVariantMap getDeviceProperties()
{
    return cc::Property::createPropertiesMap("Device");
}

QVariantMap getBaudProperties()
{
    return cc::Property::createPropertiesMap("Baud");
}

QVariantMap getFlagsProperties()
{
    QVariantList membersData;
    membersData.append(getParityMemberData());
    membersData.append(getStopBitsMemberData());
    membersData.append(getQosMemberData());
    membersData.append(getFlagsMemberData());

    typedef CCSerialInfo::AllFields AllFields;
    typedef std::decay<decltype(std::get<CCSerialInfo::FieldId_Flags>(std::declval<AllFields>()))>::type FlagsType;
    typedef std::decay<decltype(std::declval<FlagsType>().value())>::type MembersType;
    static const std::size_t NumOfMembers = std::tuple_size<MembersType>::value;

    GASSERT(membersData.size() == (int)NumOfMembers);
    return cc::Property::createPropertiesMap("Flags", std::move(membersData));
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(QVariant::fromValue(getDeviceProperties()));
    props.append(QVariant::fromValue(getBaudProperties()));
    props.append(QVariant::fromValue(getFlagsProperties()));

    assert(props.size() == CCSerialInfo::FieldId_NumOfFields);
    return props;
}

}  // namespace

const char* CCSerialInfo::nameImpl() const
{
    return SerialInfoName;
}

const QVariantList& CCSerialInfo::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

void CCSerialInfo::resetImpl()
{
    fields() = Base::AllFields();
}

bool CCSerialInfo::assignImpl(const comms_champion::Message& other)
{
    assert(other.idAsString() == idAsString());
    auto* castedOther = dynamic_cast<const CCSerialInfo*>(&other);
    assert(castedOther != nullptr);
    fields() = castedOther->fields();
    return true;
}

}  // namespace message

}  // namespace plugin


}  // namespace demo




