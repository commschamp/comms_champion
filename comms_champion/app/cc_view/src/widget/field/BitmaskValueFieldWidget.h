//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include <vector>

#include "comms_champion/field_wrapper/BitmaskValueWrapper.h"
#include "FieldWidget.h"

#include "ui_BitmaskValueFieldWidget.h"

class QCheckBox;
namespace comms_champion
{

class BitmaskValueFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::BitmaskValueWrapperPtr;

    explicit BitmaskValueFieldWidget(
        WrapperPtr&& wrapper,
        QWidget* parentObj = nullptr);

    ~BitmaskValueFieldWidget();

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void serialisedValueUpdated(const QString& value);
    void checkBoxUpdated(int value);

private:
    using WrapperType = WrapperPtr::element_type;
    using UnderlyingType = WrapperType::UnderlyingType;

    Ui::BitmaskValueFieldWidget m_ui;
    WrapperPtr m_wrapper;
    std::vector<QCheckBox*> m_checkboxes;
};


}  // namespace comms_champion


