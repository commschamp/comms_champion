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

#include <cassert>

#include "Lists.h"

namespace cc = comms_champion;

namespace comms_champion
{

namespace demo
{

namespace cc_plugin
{

namespace message
{

namespace
{

typedef demo::message::ListsFields<Lists::Field> ListsFields;

QVariantMap createField2Properties()
{
    typedef ListsFields::field2 Field2;
    static const auto ElemCount =
        Field2::ParsedOptions::SequenceFixedSize;

    cc::property::field::ForField<ListsFields::field2> props;
    props.name("field2");

    for (auto idx = 0U; idx < ElemCount; ++idx) {
        props.add(
            cc::property::field::IntValue()
                .name(QString("element %1").arg(idx))
                .serialisedHidden()
                .asMap());
    }
    return props.asMap();
}

QVariantMap createField3Properties()
{
    return
        cc::property::field::ForField<ListsFields::field3>()
            .name("field3")
            .add(
                cc::property::field::ForField<ListsFields::field3::ValueType::value_type>()
                    .name("element")
                    .add(cc::property::field::IntValue().name("memeber1").asMap())
                    .add(cc::property::field::IntValue().name("memeber2").asMap())
                    .serialisedHidden()
                    .asMap())
            .serialisedHidden()
            .asMap();
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(cc::property::field::ForField<ListsFields::field1>().name("field1").asMap());
    props.append(createField2Properties());
    props.append(createField3Properties());

    assert(props.size() == Lists::FieldIdx_numOfValues);
    return props;
}

}  // namespace

Lists::Lists() = default;
Lists::~Lists() = default;

Lists& Lists::operator=(const Lists&) = default;
Lists& Lists::operator=(Lists&&) = default;

const char* Lists::nameImpl() const
{
    static const char* Str = "Lists";
    return Str;
}

const QVariantList& Lists::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
