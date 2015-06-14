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

#include "CCRawDataMessage.h"

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

enum FieldIdx
{
    FieldIdx_Data,
    FieldIdx_NumOfFields
};

const char* FieldNames[] = {
    "Data"
};

static_assert(
    std::extent<decltype(FieldNames)>::value == FieldIdx_NumOfFields,
    "FieldNames array must be updated.");

QVariantMap createDataProperties()
{
    QVariantMap props;
    props.insert(cc::Property::name(), "Data");
    return props;
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(QVariant::fromValue(createDataProperties()));

    assert(props.size() == FieldIdx_NumOfFields);
    return props;
}

}  // namespace


const char* CCRawDataMessage::nameImpl() const
{
    static const char* Str = "Raw Data Message";
    return Str;
}

void CCRawDataMessage::updateFieldPropertiesImpl(
    QWidget& fieldWidget,
    uint idx) const
{
    if (FieldIdx_NumOfFields <= idx) {
        assert(idx < FieldIdx_NumOfFields);
        return;
    }

    cc::Property::setNameVal(fieldWidget, FieldNames[idx]);
}

const QVariantList& CCRawDataMessage::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace plugin

}  // namespace demo


