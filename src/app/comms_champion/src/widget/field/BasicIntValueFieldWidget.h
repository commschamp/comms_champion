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


#pragma once

#include "comms_champion/FieldWidget.h"
#include "comms_champion/field_wrapper/BasicIntValueWrapper.h"

#include "ui_BasicIntValueFieldWidget.h"

namespace comms_champion
{

class BasicIntValueFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::BasicIntValueWrapperPtr;

    explicit BasicIntValueFieldWidget(
        WrapperPtr&& wrapper,
        QWidget* parent = nullptr);

    ~BasicIntValueFieldWidget();

protected:
    virtual void refreshImpl() override;
    virtual void setEditEnabledImpl(bool enabled) override;
    virtual void propertiesUpdatedImpl() override;

private slots:
    void serialisedValueUpdated(const QString& value);
    void valueUpdated(int value);

private:
    using WrapperType = typename WrapperPtr::element_type;
    using UnderlyingType = typename WrapperType::UnderlyingType;

    void readPropertiesAndUpdateUi();

    Ui::BasicIntValueFieldWidget m_ui;
    WrapperPtr m_wrapper;
};


}  // namespace comms_champion


