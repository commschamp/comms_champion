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

#include "Optionals.h"

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

typedef demo::message::OptionalsFields<Optionals::Field> OptionalsFields;

QVariantMap createField1Properties()
{
    QVariantList bitNames;
    bitNames.append("enable field2");
    bitNames.append("enable field3");
    assert(bitNames.size() == OptionalsFields::field1_numOfBits);
    return cc::Property::createPropertiesMap("field1", std::move(bitNames));
}

QVariantMap createField2Properties()
{
    static const char* Name = "field2";
    auto fieldProps = cc::Property::createPropertiesMap(Name);
    auto props = cc::Property::createPropertiesMap(Name, std::move(fieldProps));
    cc::Property::setUncheckable(props);
    return props;
}

QVariantMap createField3Properties()
{
    static const char* Name = "field3";
    auto fieldProps = cc::Property::createPropertiesMap(Name);
    auto props = cc::Property::createPropertiesMap(Name, std::move(fieldProps));
    cc::Property::setUncheckable(props);
    return props;
}

QVariantList createFieldsProperties()
{
    QVariantList props;
    props.append(createField1Properties());
    props.append(createField2Properties());
    props.append(createField3Properties());

    assert(props.size() == Optionals::FieldIdx_numOfValues);
    return props;
}

}  // namespace

Optionals::Optionals() = default;
Optionals::~Optionals() = default;

Optionals& Optionals::operator=(const Optionals&) = default;
Optionals& Optionals::operator=(Optionals&&) = default;

const char* Optionals::nameImpl() const
{
    static const char* Str = "Optionals";
    return Str;
}

const QVariantList& Optionals::fieldsPropertiesImpl() const
{
    static const auto Props = createFieldsProperties();
    return Props;
}

}  // namespace message

}  // namespace cc_plugin

}  // namespace demo

}  // namespace comms_champion
