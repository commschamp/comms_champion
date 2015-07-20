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


#pragma once

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariantMap>

namespace comms_champion
{

class Property
{
public:
    static QVariantMap createPropertiesMap(const QString& name);
    static QVariantMap createPropertiesMap(const char* name);
    static QVariantMap createPropertiesMap(const QString& name, QVariant&& data);

    static QVariant getName(const QVariantMap& props);
    static void setName(QVariantMap& props, const QString& value);

    static QVariant getData(const QVariantMap& props);
    static void setData(QVariantMap& props, const QVariantMap& data);
    static void setData(QVariantMap& props, QVariantMap&& data);
    static void setData(QVariantMap& props, const QVariantList& data);
    static void setData(QVariantMap& props, QVariantList&& data);

    static bool getSerialisedHidden(const QVariantMap& props);
    static void setSerialisedHidden(QVariantMap& props, bool value = true);

    static bool getFieldHidden(const QVariantMap& props);
    static void setFieldHidden(QVariantMap& props, bool value = true);

    static bool getReadOnly(const QVariantMap& props);
    static void setReadOnly(QVariantMap& props, bool value = true);

    static QVariant getFloatDecimals(const QVariantMap& props);
    static void setFloatDecimals(QVariantMap& props, int value);

    static void appendEnumValue(
        QVariantList& elemsList,
        const QString& elemName,
        long long int elemValue);

    static void appendEnumValue(
        QVariantList& elemsList,
        const QString& elemName);

};

}  // namespace comms_champion


