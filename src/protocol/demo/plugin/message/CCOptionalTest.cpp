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

const char* FieldNames[] = {
    "Flags",
    "Opt. Enum",
    "Opt. Int"
};

static_assert(std::extent<decltype(FieldNames)>::value == CCOptionalTest::FieldId_NumOfFields,
    "CCOptionalTest::FieldId enum has changed");

}  // namespace

const char* CCOptionalTest::nameImpl() const
{
    return OptionalTestName;
}

void CCOptionalTest::updateFieldPropertiesImpl(QWidget& fieldWidget, uint idx) const
{
    if (FieldId_NumOfFields <= idx) {
        assert(idx < FieldId_NumOfFields);
        return;
    }

    cc::Property::setNameVal(fieldWidget, FieldNames[idx]);
    if (idx == FieldId_Flags) {
        cc::Property::setIndexedNameVal(fieldWidget, 0, "Enable opt. enum");
        cc::Property::setIndexedNameVal(fieldWidget, 1, "Enable opt. int");
    }
    else if (idx == FieldId_OptEnum) {
        QVariantMap map;
        map.insert(cc::Property::name(), QVariant::fromValue(QString("Opt enum")));

        static const QString NameMap[] = {
            "Val1",
            "Val2",
            "Val3"
        };

        static const std::size_t NumOfValues = std::extent<decltype(NameMap)>::value;
        for (auto idx = 0U; idx < NumOfValues; ++idx) {
            map.insert(cc::Property::indexedName(idx), QVariant::fromValue(NameMap[idx]));
        }
        cc::Property::setDataVal(fieldWidget, map);
    }
    else if (idx == FieldId_OptInt) {
        QVariantMap map;
        map.insert(cc::Property::name(), QVariant::fromValue(QString("Opt int")));
        cc::Property::setDataVal(fieldWidget, map);
    }
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




