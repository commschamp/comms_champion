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
    explicit BasicIntValueFieldWidget(
        field_wrapper::BasicIntValueWrapperPtr&& wrapper,
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
    void readPropertiesAndUpdateUi();

    Ui::BasicIntValeFieldWidget m_ui;
    field_wrapper::BasicIntValueWrapperPtr m_wrapper;
    QString m_defaultStyleSheet;
};


}  // namespace comms_champion


