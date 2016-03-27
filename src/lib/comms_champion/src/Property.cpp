//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "comms_champion/Property.h"

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariant>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

namespace
{

const QString& nameKey()
{
    static const QString Str("cc.name");
    return Str;
}

const QString& dataKey()
{
    static const QString Str("cc.data");
    return Str;
}

const QString& serialisedHiddenKey()
{
    static const QString Str("cc.ser_hidden");
    return Str;
}

const QString& fieldHiddenKey()
{
    static const QString Str("cc.field_hidden");
    return Str;
}

const QString& readOnlyKey()
{
    static const QString Str("cc.read_only");
    return Str;
}

const QString& displayScaledKey()
{
    static const QString Str("cc.display_scaled");
    return Str;
}

const QString& floatDecimalsKey()
{
    static const QString Str("cc.float_dec");
    return Str;
}

const QString& uncheckableKey()
{
    static const QString Str("cc.uncheckable");
    return Str;
}

const QString& numValueDisplayOffsetKey()
{
    static const QString Str("cc.num_display_offset");
    return Str;
}


}  // namespace

QVariantMap Property::createPropertiesMap(const QString& name)
{
    QVariantMap props;
    props.insert(nameKey(), name);
    return props;
}

QVariantMap Property::createPropertiesMap(const char* name)
{
    QVariantMap props;
    props.insert(nameKey(), name);
    return props;
}

QVariantMap Property::createPropertiesMap(const QString& name, QVariant&& data)
{
    QVariantMap props;
    props.insert(nameKey(), name);
    props.insert(dataKey(), std::move(data));
    return props;
}

QVariant Property::getName(const QVariantMap& props)
{
    return props.value(nameKey());
}

void Property::setName(QVariantMap& props, const QString& value)
{
    props.insert(nameKey(), value);
}

QVariant Property::getData(const QVariantMap& props)
{
    return props.value(dataKey());
}

void Property::setData(QVariantMap& props, const QVariantMap& data)
{
    props.insert(dataKey(), data);
}

void Property::setData(QVariantMap& props, QVariantMap&& data)
{
    props.insert(dataKey(), std::move(data));
}

void Property::setData(QVariantMap& props, const QVariantList& data)
{
    props.insert(dataKey(), data);
}

void Property::setData(QVariantMap& props, QVariantList&& data)
{
    props.insert(dataKey(), std::move(data));
}

bool Property::getSerialisedHidden(const QVariantMap& props)
{
    auto serHiddenVar = props.value(serialisedHiddenKey());
    return
        (serHiddenVar.isValid()) &&
        (serHiddenVar.canConvert<bool>()) &&
        (serHiddenVar.value<bool>());
}

void Property::setSerialisedHidden(QVariantMap& props, bool value)
{
    props.insert(serialisedHiddenKey(), value);
}

bool Property::getFieldHidden(const QVariantMap& props)
{
    auto serHiddenVar = props.value(fieldHiddenKey());
    return
        (serHiddenVar.isValid()) &&
        (serHiddenVar.canConvert<bool>()) &&
        (serHiddenVar.value<bool>());
}

void Property::setFieldHidden(QVariantMap& props, bool value)
{
    props.insert(fieldHiddenKey(), value);
}

bool Property::getReadOnly(const QVariantMap& props)
{
    auto serHiddenVar = props.value(readOnlyKey());
    return
        (serHiddenVar.isValid()) &&
        (serHiddenVar.canConvert<bool>()) &&
        (serHiddenVar.value<bool>());
}

void Property::setReadOnly(QVariantMap& props, bool value)
{
    props.insert(readOnlyKey(), value);
}

bool Property::getDisplayScaled(const QVariantMap& props)
{
    auto var = props.value(displayScaledKey());
    return
        (var.isValid()) &&
        (var.canConvert<bool>()) &&
        (var.value<bool>());
}

void Property::setDisplayScaled(QVariantMap& props, bool value)
{
    props.insert(displayScaledKey(), value);
}

QVariant Property::getFloatDecimals(const QVariantMap& props)
{
    return props.value(floatDecimalsKey());
}

void Property::setFloatDecimals(QVariantMap& props, int value)
{
    props.insert(floatDecimalsKey(), value);
}

bool Property::getUncheckable(const QVariantMap& props)
{
    auto var = props.value(uncheckableKey());
    return
        (var.isValid()) &&
        (var.canConvert<bool>()) &&
        (var.value<bool>());
}

void Property::setUncheckable(QVariantMap& props, bool value)
{
    props.insert(uncheckableKey(), value);
}

void Property::appendEnumValue(
    QVariantList& elemsList,
    const QString& elemName,
    long long int elemValue)
{
    auto elemProps = createPropertiesMap(elemName);
    elemProps.insert(dataKey(), elemValue);
    elemsList.append(elemProps);
}

void Property::appendEnumValue(
    QVariantList& elemsList,
    const QString& elemName)
{
    auto elemProps = createPropertiesMap(elemName);
    elemProps.insert(dataKey(), elemsList.size());
    elemsList.append(elemProps);
}

long long int Property::getNumValueDisplayOffset(const QVariantMap& props)
{
    auto var = props.value(numValueDisplayOffsetKey());
    if ((!var.isValid()) ||
        (!var.canConvert<long long int>())) {
        return 0;
    }
    return var.value<long long int>();
}

void Property::setNumValueDisplayOffset(QVariantMap& props, long long int offset)
{
    props.insert(numValueDisplayOffsetKey(), offset);
}

}  // namespace comms_champion


