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


#include "comms_champion/FieldWidgetCreator.h"

#include <cassert>

#include "widget/field/IntValueFieldWidget.h"
#include "widget/field/LongIntValueFieldWidget.h"
#include "widget/field/BitmaskValueFieldWidget.h"
#include "widget/field/EnumValueFieldWidget.h"
#include "widget/field/StringFieldWidget.h"
#include "widget/field/BitfieldFieldWidget.h"
#include "widget/field/OptionalFieldWidget.h"
#include "widget/field/BundleFieldWidget.h"
#include "widget/field/ArrayListRawDataFieldWidget.h"
#include "widget/field/ArrayListFieldWidget.h"
#include "widget/field/FloatValueFieldWidget.h"
#include "widget/field/UnknownValueFieldWidget.h"

namespace comms_champion
{

void FieldWidgetCreator::bitfieldWidgetAddMember(
    FieldWidget& bitfieldWidget,
    FieldWidgetPtr memberWidget)
{
    auto* castedBitfieldWidget = qobject_cast<BitfieldFieldWidget*>(&bitfieldWidget);
    if (castedBitfieldWidget == nullptr) {
        assert(!"Wrong cast, expected bitfield widget");
        return;
    }

    castedBitfieldWidget->addMemberField(memberWidget.release());
}

void FieldWidgetCreator::optionalWidgetSetField(
    FieldWidget& optionalWidget,
    FieldWidgetPtr fieldWidget)
{
    auto* castedOptionalFieldWidget = qobject_cast<OptionalFieldWidget*>(&optionalWidget);
    if (castedOptionalFieldWidget == nullptr) {
        assert(!"Wrong cast, expected optional widget");
        return;
    }

    castedOptionalFieldWidget->setField(fieldWidget.release());
}

void FieldWidgetCreator::bundleWidgetAddMember(
    FieldWidget& bundleWidget,
    FieldWidgetPtr memberWidget)
{
    auto* castedBundleWidget = qobject_cast<BundleFieldWidget*>(&bundleWidget);
    if (castedBundleWidget == nullptr) {
        assert(!"Wrong cast, expected bundle widget");
        return;
    }

    castedBundleWidget->addMemberField(memberWidget.release());
}

FieldWidgetPtr
FieldWidgetCreator::createIntValueFieldWidget(
    field_wrapper::IntValueWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new IntValueFieldWidget(std::move(fieldWrapper)));
}

FieldWidgetPtr
FieldWidgetCreator::createLongIntValueFieldWidget(
    field_wrapper::LongIntValueWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new LongIntValueFieldWidget(std::move(fieldWrapper)));
}


FieldWidgetPtr
FieldWidgetCreator::createBitmaskValueFieldWidget(
    field_wrapper::BitmaskValueWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new BitmaskValueFieldWidget(std::move(fieldWrapper)));
}

FieldWidgetPtr
FieldWidgetCreator::createEnumValueFieldWidget(
    field_wrapper::EnumValueWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new EnumValueFieldWidget(std::move(fieldWrapper)));
}

FieldWidgetPtr
FieldWidgetCreator::createStringFieldWidget(
    field_wrapper::StringWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new StringFieldWidget(std::move(fieldWrapper)));
}

FieldWidgetPtr
FieldWidgetCreator::createBitfieldFieldWidget(
    field_wrapper::BitfieldWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new BitfieldFieldWidget(std::move(fieldWrapper)));
}

FieldWidgetPtr
FieldWidgetCreator::createOptionalFieldWidget(
    field_wrapper::OptionalWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new OptionalFieldWidget(std::move(fieldWrapper)));
}

FieldWidgetPtr FieldWidgetCreator::createBundleFieldWidget()
{
    return
        FieldWidgetPtr(new BundleFieldWidget());
}

FieldWidgetPtr FieldWidgetCreator::createArrayListRawDataFieldWidget(
    field_wrapper::ArrayListRawDataWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new ArrayListRawDataFieldWidget(std::move(fieldWrapper)));
}

FieldWidgetPtr FieldWidgetCreator::createArrayListFieldWidget(
    field_wrapper::ArrayListWrapperPtr fieldWrapper,
    std::function<std::vector<FieldWidgetPtr> (std::size_t)>&& updateFunc)
{
    return
        FieldWidgetPtr(
            new ArrayListFieldWidget(
                std::move(fieldWrapper),
                std::move(updateFunc)));
}

FieldWidgetPtr FieldWidgetCreator::createFloatValueFieldWidget(
    field_wrapper::FloatValueWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new FloatValueFieldWidget(
                std::move(fieldWrapper)));
}

FieldWidgetPtr
FieldWidgetCreator::createUnknownValueFieldWidget(
    field_wrapper::UnknownValueWrapperPtr fieldWrapper)
{
    return
        FieldWidgetPtr(
            new UnknownValueFieldWidget(std::move(fieldWrapper)));
}


}  // namespace comms_champion

