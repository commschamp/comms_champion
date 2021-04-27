//
// Copyright 2014 - 2021 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/field_wrapper/FloatValueWrapper.h"
#include "FieldWidget.h"

#include "ui_FloatValueFieldWidget.h"

namespace comms_champion
{

class SpecialValueWidget;
class FloatValueFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::FloatValueWrapperPtr;

    explicit FloatValueFieldWidget(
        WrapperPtr wrapper,
        QWidget* parentObj = nullptr);

    ~FloatValueFieldWidget() noexcept;

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void serialisedValueUpdated(const QString& value);
    void valueUpdated(double value);
    void typeUpdated(int index);
    void specialSelected(double value);

private:
    using SpecialInfo = QPair<QString, double>;
    using SpecialsList = QList<SpecialInfo>;
    using WrapperType = WrapperPtr::element_type;
    using UnderlyingType = WrapperType::UnderlyingType;

    void updateSpinBoxValueRange();
    void updateType();
    int getTypeIndex();
    bool createSpecialsWidget(const SpecialsList& specials);

    Ui::FloatValueFieldWidget m_ui;
    WrapperPtr m_wrapper;
    double m_oldValue = 0.0;
    SpecialValueWidget* m_specialsWidget = nullptr;
};


}  // namespace comms_champion


