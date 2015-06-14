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
    QVariantMap props;
    props.insert(cc::Property::name(), QVariant::fromValue(QString("Flags")));
    props.insert(cc::Property::indexedName(0), QVariant::fromValue(QString("Enable opt. enum")));
    props.insert(cc::Property::indexedName(1), QVariant::fromValue(QString("Enable opt. int")));
    return props;
}

QVariantMap createOptEnumProperties()
{
    static const QString Name("Opt enum");
    QVariantMap enumProps;
    enumProps.insert(cc::Property::name(), QVariant::fromValue(Name));

    static const QString NameMap[] = {
        "Val1",
        "Val2",
        "Val3"
    };

    static const std::size_t NumOfValues = std::extent<decltype(NameMap)>::value;
    for (auto idx = 0U; idx < NumOfValues; ++idx) {
        enumProps.insert(cc::Property::indexedName(idx), QVariant::fromValue(NameMap[idx]));
    }

    QVariantMap props;
    props.insert(cc::Property::name(), QVariant::fromValue(Name));
    props.insert(cc::Property::data(), QVariant::fromValue(enumProps));
    return props;
}

QVariantMap createOptIntProperties()
{
    static const QString Name("Opt int");
    QVariantMap intProps;
    intProps.insert(cc::Property::name(), QVariant::fromValue(Name));

    QVariantMap props;
    props.insert(cc::Property::name(), QVariant::fromValue(Name));
    props.insert(cc::Property::data(), QVariant::fromValue(intProps));
    return props;
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
    Base::getFields() = Base::AllFields();
}

void CCOptionalTest::assignImpl(const comms_champion::Message& other)
{
    assert(other.idAsString() == idAsString());
    auto* castedOther = dynamic_cast<const CCOptionalTest*>(&other);
    assert(castedOther != nullptr);
    getFields() = castedOther->getFields();
}

}  // namespace message

}  // namespace plugin


}  // namespace demo




