//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/field_wrapper/EnumValueWrapper.h"
#include "FieldWidget.h"

#include "ui_EnumValueFieldWidget.h"

namespace comms_champion
{

class EnumValueFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::EnumValueWrapperPtr;

    explicit EnumValueFieldWidget(
        WrapperPtr&& wrapper,
        QWidget* parentObj = nullptr);

    ~EnumValueFieldWidget() noexcept;

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void serialisedValueUpdated(const QString& value);
    void valueUpdated(int idx);

private:
    using WrapperType = WrapperPtr::element_type;
    using UnderlyingType = WrapperType::UnderlyingType;

    Ui::EnumValueFieldWidget m_ui;
    WrapperPtr m_wrapper;
    bool m_signalsConnected = false;
    int m_idxOffset = 0;
};


}  // namespace comms_champion


