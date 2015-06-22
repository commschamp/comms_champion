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
    QVariantMap map;
    map.insert(cc::Property::name(), QVariant::fromValue(QString("Parity")));
    map.insert(cc::Property::serialisedHidden(), QVariant::fromValue(true));

    static const QString ValueMap[] = {
        "None",
        "Odd",
        "Even"
    };

    static const std::size_t NumOfValues = std::extent<decltype(ValueMap)>::value;
    static_assert(
        NumOfValues == (std::size_t)demo::message::Parity::NumOfValues,
        "Incorrect value mapping");

    for (auto idx = 0U; idx < NumOfValues; ++idx) {
        map.insert(cc::Property::indexedName(idx), QVariant::fromValue(ValueMap[idx]));
    }
    return map;
}

QVariantMap getStopBitsMemberData()
{
    QVariantMap map;
    map.insert(cc::Property::name(), QVariant::fromValue(QString("Stop Bits")));
    map.insert(cc::Property::serialisedHidden(), QVariant::fromValue(true));

    static const QString ValueMap[] = {
        "None",
        "One",
        "One and a Half",
        "Two"
    };

    static const std::size_t NumOfValues = std::extent<decltype(ValueMap)>::value;
    static_assert(
        NumOfValues == (std::size_t)demo::message::StopBit::NumOfValues,
        "Incorrect value mapping");

    for (auto idx = 0U; idx < NumOfValues; ++idx) {
        map.insert(cc::Property::indexedName(idx), QVariant::fromValue(ValueMap[idx]));
    }
    return map;
}

QVariantMap getFlagsMemberData()
{
    QVariantMap map;
    map.insert(cc::Property::name(), QVariant::fromValue(QString("Flags")));
    map.insert(cc::Property::serialisedHidden(), QVariant::fromValue(true));

    static const QString ValueMap[] = {
        QString(),
        "HW FLOW CTRL"
    };

    static const std::size_t NumOfValues = std::extent<decltype(ValueMap)>::value;
    for (auto idx = 0U; idx < NumOfValues; ++idx) {
        auto& value = ValueMap[idx];
        if (value.isEmpty()) {
            continue;
        }

        map.insert(cc::Property::indexedName(idx), QVariant::fromValue(value));
    }
    return map;
}

QVariantMap getQosMemberData()
{
    QVariantMap map;
    map.insert(cc::Property::name(), QVariant::fromValue(QString("QoS")));
    map.insert(cc::Property::serialisedHidden(), QVariant::fromValue(true));
    return map;
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
    QVariantMap props;
    props.insert(cc::Property::name(), "Device");
    return props;
}

QVariantMap getBaudProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Baud");
    return props;
}

QVariantMap getFlagsProperties()
{
    QVariantMap props;

    typedef CCSerialInfo::AllFields AllFields;
    typedef std::decay<decltype(std::get<CCSerialInfo::FieldId_Flags>(std::declval<AllFields>()))>::type FlagsType;
    typedef std::decay<decltype(std::declval<FlagsType>().value())>::type MembersType;
    static const std::size_t NumOfMembers = std::tuple_size<MembersType>::value;

    for (std::size_t idx = 0U; idx < NumOfMembers; ++idx) {
        props.insert(cc::Property::indexedData(idx), getMemberData(idx));
    }

    props.insert(cc::Property::name(), "Flags");
    return props;
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

void CCSerialInfo::assignImpl(const comms_champion::Message& other)
{
    assert(other.idAsString() == idAsString());
    auto* castedOther = dynamic_cast<const CCSerialInfo*>(&other);
    assert(castedOther != nullptr);
    fields() = castedOther->fields();
}

}  // namespace message

}  // namespace plugin


}  // namespace demo




