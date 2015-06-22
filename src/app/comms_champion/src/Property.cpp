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

#include "comms_champion/Property.h"

#include <QtCore/QVariant>

namespace comms_champion
{

const char* Property::name()
{
    static const char* Str = "cc.name";
    return Str;
}

void Property::setNameVal(QObject& obj, const QString& val)
{
    obj.setProperty(name(), QVariant::fromValue(val));
}

const QString& Property::indexedNamePrefix()
{
    static const QString Str("cc.name_");
    return Str;
}

QString Property::indexedName(unsigned idx)
{
    return indexedNamePrefix() + QString("%1").arg(idx);
}

void Property::setIndexedNameVal(QObject& obj, unsigned idx, const QString& val)
{
    obj.setProperty(indexedName(idx).toUtf8().data(), QVariant::fromValue(val));
}

const QString& Property::indexedDataPrefix()
{
    static const QString Str("cc.data_");
    return Str;
}

QString Property::indexedData(unsigned idx)
{
    return indexedDataPrefix() + QString("%1").arg(idx);
}

void Property::setIndexedDataVal(QObject& obj, unsigned idx, const QVariantMap& val)
{
    obj.setProperty(indexedData(idx).toUtf8().data(), QVariant::fromValue(val));
}

QVariant Property::getIndexedDataVal(QObject& obj, unsigned idx)
{
    return obj.property(indexedData(idx).toUtf8().data());
}

const char* Property::data()
{
    static const char* Str = "cc.data";
    return Str;
}

void Property::setDataVal(QObject& obj, const QVariantMap& val)
{
    obj.setProperty(data(), QVariant::fromValue(val));
}

QVariant Property::getDataVal(QObject& obj)
{
    return obj.property(data());
}

const char* Property::serialisedHidden()
{
    static const char* Str = "cc.ser_hidden";
    return Str;
}

void Property::setSerialisedHiddenVal(QObject& obj, bool val)
{
    obj.setProperty(serialisedHidden(), QVariant::fromValue(val));
}

QVariant Property::getSerialisedHiddenVal(QObject& obj)
{
    return obj.property(serialisedHidden());
}

const char* Property::fieldHidden()
{
    static const char* Str = "cc.field_hidden";
    return Str;
}

void Property::setFieldHiddenVal(QObject& obj, bool val)
{
    obj.setProperty(fieldHidden(), QVariant::fromValue(val));
}

QVariant Property::getFieldHiddenVal(QObject& obj)
{
    return obj.property(fieldHidden());
}

}  // namespace comms_champion


