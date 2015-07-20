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


#include "CCHeartbeat.h"

#include <type_traits>
#include <cassert>

#include <QtCore/QVariantMap>

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

QVariantList createFieldsProperties()
{
    QVariantList list;
    list.append(cc::Property::createPropertiesMap("Counter"));
    GASSERT(list.size() == CCHeartbeat::FieldId_NumOfFields);
    return list;
}

}  // namespace

const char* CCHeartbeat::nameImpl() const
{
    static const char* HeartbeatName = "Heartbeat";
    return HeartbeatName;
}

const QVariantList& CCHeartbeat::fieldsPropertiesImpl() const
{
    static const QVariantList Props = createFieldsProperties();
    return Props;
}

void CCHeartbeat::resetImpl()
{
    fields() = Base::AllFields();
}

void CCHeartbeat::assignImpl(const comms_champion::Message& other)
{
    assert(other.idAsString() == idAsString());
    auto* castedOther = dynamic_cast<const CCHeartbeat*>(&other);
    assert(castedOther != nullptr);
    fields() = castedOther->fields();
}

}  // namespace message

}  // namespace plugin


}  // namespace demo




