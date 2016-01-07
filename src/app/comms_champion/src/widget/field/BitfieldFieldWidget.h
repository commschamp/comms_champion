//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <vector>

#include "comms_champion/field_wrapper/BitfieldWrapper.h"
#include "FieldWidget.h"

#include "ui_BitfieldFieldWidget.h"

namespace comms_champion
{

class BitfieldFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::BitfieldWrapperPtr;

    explicit BitfieldFieldWidget(
        WrapperPtr&& wrapper,
        QWidget* parentObj = nullptr);

    ~BitfieldFieldWidget();

    void addMemberField(FieldWidget* memberFieldWidget);

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void serialisedValueUpdated(const QString& value);
    void memberFieldUpdated();

private:
    using WrapperType = typename WrapperPtr::element_type;
    using UnderlyingType = typename WrapperType::UnderlyingType;

    void refreshInternal();
    void refreshMembers();

    Ui::BitfieldFieldWidget m_ui;
    WrapperPtr m_wrapper;
    std::vector<FieldWidget*> m_members;
};


}  // namespace comms_champion


