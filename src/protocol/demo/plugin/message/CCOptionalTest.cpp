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


#include "CCOptionalTest.h"

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

const char* OptionalTestName = "Optional Test";

QVariantMap createFlagsProperties()
{
    QVariantList valuesData;
    valuesData.append("Enable opt. enum");
    valuesData.append("Enable opt. int");
    return cc::Property::createPropertiesMap("Flags", std::move(valuesData));
}

QVariantMap createOptEnumProperties()
{
    static const QString Name("Opt enum");
    QVariantList enumValues;
    cc::Property::appendEnumValue(enumValues, "Val1");
    cc::Property::appendEnumValue(enumValues, "Val2");
    cc::Property::appendEnumValue(enumValues, "Val3");

    auto enumProps = cc::Property::createPropertiesMap(Name, std::move(enumValues));
    return cc::Property::createPropertiesMap(Name, std::move(enumProps));
}

QVariantMap createOptIntProperties()
{
    static const QString Name("Opt int");
    auto valProps = cc::Property::createPropertiesMap(Name);
    return cc::Property::createPropertiesMap(Name, std::move(valProps));
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(QVariant::fromValue(createFlagsProperties()));
    props.append(QVariant::fromValue(createOptEnumProperties()));
    props.append(QVariant::fromValue(createOptIntProperties()));

    assert(props.size() == CCOptionalTest::FieldId_NumOfFields);
    return props;
}

}  // namespace

const char* CCOptionalTest::nameImpl() const
{
    return OptionalTestName;
}

const QVariantList& CCOptionalTest::fieldsPropertiesImpl() const
{
    static const QVariantList Props = createFieldsProperties();
    return Props;
}

void CCOptionalTest::resetImpl()
{
    fields() = Base::AllFields();
}

void CCOptionalTest::assignImpl(const comms_champion::Message& other)
{
    assert(other.idAsString() == idAsString());
    auto* castedOther = dynamic_cast<const CCOptionalTest*>(&other);
    assert(castedOther != nullptr);
    fields() = castedOther->fields();
}

}  // namespace message

}  // namespace plugin


}  // namespace demo




