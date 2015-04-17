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

namespace demo
{

namespace plugin
{

namespace message
{

namespace
{

const char* SerialInfoName = "Serial Info";

const char* FieldNames[] = {
    "Device",
    "Baud"
};

static_assert(std::extent<decltype(FieldNames)>::value == CCSerialInfo::FieldId_NumOfFields,
    "CCSerialInfo::FieldId enum has changed");

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




