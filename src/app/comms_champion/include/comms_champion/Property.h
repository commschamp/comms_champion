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
    static const char* name();
    static void setNameVal(QObject& obj, const QString& val);
    static const QString& indexedNamePrefix();
    static QString indexedName(unsigned idx);
    static void setIndexedNameVal(QObject& obj, unsigned idx, const QString& val);
    static const QString& indexedDataPrefix();
    static QString indexedData(unsigned idx);
    static void setIndexedDataVal(QObject& obj, unsigned idx, const QVariantMap& val);
    static QVariant getIndexedDataVal(QObject& obj, unsigned idx);
    static const char* serialisedHidden();
    static void setSerialisedHiddenVal(QObject& obj, bool val);
    static QVariant getSerialisedHiddenVal(QObject& obj);
};

}  // namespace comms_champion


