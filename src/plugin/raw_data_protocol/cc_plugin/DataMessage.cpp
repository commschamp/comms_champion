//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "DataMessage.h"

namespace cc = comms_champion;

namespace comms_champion
{

namespace plugin
{

namespace raw_data_protocol
{

namespace cc_plugin
{

namespace
{

typedef raw_data_protocol::DataMessageFields<DataMessage::Field> DataMessageFields;

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(cc::property::field::ForField<DataMessageFields::data>().name("data").asMap());

    assert(props.size() == DataMessage::FieldIdx_numOfValues);
    return props;
}

}  // namespace

DataMessage::DataMessage() = default;
DataMessage::~DataMessage() = default;

DataMessage& DataMessage::operator=(const DataMessage&) = default;
DataMessage& DataMessage::operator=(DataMessage&&) = default;

const char* DataMessage::nameImpl() const
{
    static const char* Str = "Raw Data";
    return Str;
}

const QVariantList& DataMessage::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace cc_plugin

}  // namespace raw_data_protocol

}  // namespace plugin

}  // namespace comms_champion
