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


#include "CCStatus.h"

#include <type_traits>
#include <cassert>

#include <QtCore/QVariant>

#include "comms_champion/Property.h"

namespace cc = comms_champion;

namespace demo
{

namespace plugin
{

namespace message
{

namespace
{

const char* Name = "Status";

const char* FieldNames[] = {
    "Execution Status",
    "Features"
};

static_assert(std::extent<decltype(FieldNames)>::value == CCStatus::FieldId_NumOfFields,
    "CCStatus::FieldId enum has changed");

}  // namespace

const char* CCStatus::nameImpl() const
{
    return Name;
}

void CCStatus::updateFieldPropertiesImpl(QWidget& fieldWidget, uint idx) const
{
    if (FieldId_NumOfFields <= idx) {
        assert(idx < FieldId_NumOfFields);
        return;
    }

    cc::Property::setNameVal(fieldWidget, FieldNames[idx]);

    if (idx == FieldId_ExecutionStatus) {
        cc::Property::setIndexedNameVal(fieldWidget, demo::message::ExecutionStatus_Idle, "Idle");
        cc::Property::setIndexedNameVal(fieldWidget, demo::message::ExecutionStatus_Running, "Running");
        cc::Property::setIndexedNameVal(fieldWidget, demo::message::ExecutionStatus_Complete, "Complete");
        cc::Property::setIndexedNameVal(fieldWidget, demo::message::ExecutionStatus_Error, "Error");
    }
    else if (idx == FieldId_Features) {
        cc::Property::setIndexedNameVal(fieldWidget, 0, "Feature1");
        cc::Property::setIndexedNameVal(fieldWidget, 1, "Feature2");
        cc::Property::setIndexedNameVal(fieldWidget, 2, "Feature3");
        cc::Property::setIndexedNameVal(fieldWidget, 3, "Feature4");
    }
}

void CCStatus::resetImpl()
{
    Base::getFields() = Base::AllFields();
}

void CCStatus::assignImpl(const comms_champion::Message& other)
{
    assert(other.idAsString() == idAsString());
    auto* castedOther = dynamic_cast<const CCStatus*>(&other);
    assert(castedOther != nullptr);
    getFields() = castedOther->getFields();
}


}  // namespace message

}  // namespace plugin


}  // namespace demo




