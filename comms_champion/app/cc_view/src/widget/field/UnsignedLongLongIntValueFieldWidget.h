//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/field_wrapper/UnsignedLongValueWrapper.h"
#include "FieldWidget.h"

#include "ui_UnsignedLongLongIntValueFieldWidget.h"

namespace comms_champion
{

class SpecialValueWidget;
class UnsignedLongLongIntValueFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::UnsignedLongValueWrapperPtr;

    explicit UnsignedLongLongIntValueFieldWidget(
        WrapperPtr wrapper,
        QWidget* parentObj = nullptr);

    ~UnsignedLongLongIntValueFieldWidget() noexcept;

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void serialisedValueUpdated(const QString& value);
    void valueUpdated(const QString& value);
    void specialSelected(long long value);

private:
    using SpecialInfo = QPair<QString, long long>;
    using SpecialsList = QList<SpecialInfo>;
    using WrapperType = WrapperPtr::element_type;
    using UnderlyingType = WrapperType::UnderlyingType;
    typedef double DisplayedType;
    UnderlyingType adjustDisplayedToReal(DisplayedType val);
    DisplayedType adjustRealToDisplayed(UnderlyingType val);
    static DisplayedType getDisplayedValue(const QString& value);
    bool createSpecialsWidget(const SpecialsList& specials);

    Ui::UnsignedLongLongIntValueFieldWidget m_ui;
    WrapperPtr m_wrapper;
    long long m_offset = 0;
    int m_decimals = 0;
    SpecialValueWidget* m_specialsWidget = nullptr;
};


}  // namespace comms_champion


