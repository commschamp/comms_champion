//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include "comms_champion/field_wrapper/IntValueWrapper.h"
#include "FieldWidget.h"

#include "ui_LongLongIntValueFieldWidget.h"

namespace comms_champion
{

class LongLongIntValueFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::IntValueWrapperPtr;

    explicit LongLongIntValueFieldWidget(
        WrapperPtr wrapper,
        QWidget* parentObj = nullptr);

    ~LongLongIntValueFieldWidget();

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void serialisedValueUpdated(const QString& value);
    void valueUpdated(const QString& value);

private:
    using WrapperType = WrapperPtr::element_type;
    using UnderlyingType = WrapperType::UnderlyingType;
    typedef long long int DisplayedType;
    UnderlyingType adjustDisplayedToReal(DisplayedType val);
    DisplayedType adjustRealToDisplayed(UnderlyingType val);
    static DisplayedType getDisplayedValue(const QString& value);

    Ui::LongLongIntValueFieldWidget m_ui;
    WrapperPtr m_wrapper;
    DisplayedType m_offset = 0;
};


}  // namespace comms_champion


