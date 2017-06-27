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

#include "comms_champion/field_wrapper/UnknownValueWrapper.h"
#include "FieldWidget.h"

#include "ui_UnknownValueFieldWidget.h"

namespace comms_champion
{

class UnknownValueFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    explicit UnknownValueFieldWidget(
        field_wrapper::UnknownValueWrapperPtr&& wrapper,
        QWidget* parentObj = nullptr);

    ~UnknownValueFieldWidget();

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;

private slots:
    void serialisedValueUpdated(const QString& value);

private:
    void setFieldValid(bool valid);

    Ui::UnknownValueFieldWidget m_ui;
    field_wrapper::UnknownValueWrapperPtr m_wrapper;
};


}  // namespace comms_champion


