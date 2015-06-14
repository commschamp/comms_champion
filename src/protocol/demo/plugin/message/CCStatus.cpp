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

QVariantMap createStatusProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Execution Status");
    props.insert(cc::Property::indexedName(demo::message::ExecutionStatus_Idle), "Idle");
    props.insert(cc::Property::indexedName(demo::message::ExecutionStatus_Running), "Running");
    props.insert(cc::Property::indexedName(demo::message::ExecutionStatus_Complete), "Complete");
    props.insert(cc::Property::indexedName(demo::message::ExecutionStatus_Error), "Error");
    return props;
}

QVariantMap FeaturesProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Features");
    props.insert(cc::Property::indexedName(0), "Feature 1");
    props.insert(cc::Property::indexedName(1), "Feature 2");
    props.insert(cc::Property::indexedName(2), "Feature 3");
    props.insert(cc::Property::indexedName(3), "Feature 4");
    return props;
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(QVariant::fromValue(createStatusProperties()));
    props.append(QVariant::fromValue(FeaturesProperties()));

    assert(props.size() == CCStatus::FieldId_NumOfFields);
    return props;
}

}  // namespace

const char* CCStatus::nameImpl() const
{
    return Name;
}

const QVariantList& CCStatus::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
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




