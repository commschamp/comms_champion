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


#include "CCSerialInfo.h"

#include <type_traits>
#include <cassert>

#include <QtCore/QVariant>
#include <QtCore/QVariantMap>

namespace demo
{

namespace plugin
{

namespace message
{

namespace
{

const char* SerialInfoName = "Serial Info";
const QString NamePropertyName("cc.name");

const char* FieldNames[] = {
    "Device",
    "Baud",
    "Flags"
};

static_assert(std::extent<decltype(FieldNames)>::value == CCSerialInfo::FieldId_NumOfFields,
    "CCSerialInfo::FieldId enum has changed");

QVariantMap getParityMemberData()
{
    QVariantMap map;
    map.insert(NamePropertyName, QVariant::fromValue(QString("Parity")));
    return map;
}

QVariantMap getStopBitsMemberData()
{
    QVariantMap map;
    map.insert(NamePropertyName, QVariant::fromValue(QString("Stop Bits")));
    return map;
}

QVariantMap getFlagsMemberData()
{
    QVariantMap map;
    map.insert(NamePropertyName, QVariant::fromValue(QString("Flags")));
    return map;
}

const QVariantMap& getMemberData(std::size_t idx)
{
    static const QVariantMap Map[] = {
        getParityMemberData(),
        getStopBitsMemberData(),
        getFlagsMemberData()
    };

    static const auto DataCount = std::extent<decltype(Map)>::value;
    static_assert(
        DataCount == CCSerialInfo::FieldId_NumOfFields,
        "Data for some member fields is missing.");

    if (DataCount <= idx) {
        static const QVariantMap EmptyMap;
        return EmptyMap;
    }

    return Map[idx];
}

}  // namespace

const char* CCSerialInfo::nameImpl() const
{
    return SerialInfoName;
}

void CCSerialInfo::updateFieldPropertiesImpl(QWidget& fieldWidget, uint idx) const
{
    if (FieldId_NumOfFields <= idx) {
        assert(idx < FieldId_NumOfFields);
        return;
    }

    setNameProperty(fieldWidget, FieldNames[idx]);
    if (idx == FieldId_Flags) {
        auto& flagsField = std::get<FieldId_Flags>(getFields());
        auto& flagsMemberFields = flagsField.fields();
        typedef typename std::decay<decltype(flagsMemberFields)>::type FlagsBitfieldMembers;
        static const std::size_t NumOfMembers = std::tuple_size<FlagsBitfieldMembers>::value;
        for (std::size_t idx = 0U; idx < NumOfMembers; ++idx) {
            static const QString MemDataPropNamePrefix("cc.member_");
            QString memDataPropName = MemDataPropNamePrefix + QString("%1").arg(idx);
            fieldWidget.setProperty(
                memDataPropName.toUtf8().data(),
                QVariant::fromValue(getMemberData(idx)));
        }
    }
}

void CCSerialInfo::resetImpl()
{
    Base::getFields() = Base::AllFields();
}

void CCSerialInfo::assignImpl(const comms_champion::Message& other)
{
    assert(other.idAsString() == idAsString());
    auto* castedOther = dynamic_cast<const CCSerialInfo*>(&other);
    assert(castedOther != nullptr);
    getFields() = castedOther->getFields();
}

}  // namespace message

}  // namespace plugin


}  // namespace demo




