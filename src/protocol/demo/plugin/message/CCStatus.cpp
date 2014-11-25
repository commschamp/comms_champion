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

namespace demo
{

namespace plugin
{

namespace message
{

namespace
{

const char* Name = "Status";

const char* FieldNames[] = {
    "Execution Status",
    "Features"
};

static_assert(std::extent<decltype(FieldNames)>::value == CCStatus::FieldId_NumOfFields,
    "CCStatus::FieldId enum has changed");

}  // namespace

const char* CCStatus::nameImpl() const
{
    return Name;
}

void CCStatus::updateFieldPropertiesImpl(QWidget& fieldWidget, uint idx) const
{
    if (FieldId_NumOfFields <= idx) {
        assert(idx < FieldId_NumOfFields);
        return;
    }

    fieldWidget.setProperty("name", FieldNames[idx]);
}

}  // namespace message

}  // namespace plugin


}  // namespace demo




