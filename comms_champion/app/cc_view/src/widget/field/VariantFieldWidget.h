//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
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

#include <functional>

#include "comms_champion/field_wrapper/VariantWrapper.h"
#include "FieldWidget.h"

#include "ui_VariantFieldWidget.h"

namespace comms_champion
{

class VariantFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using WrapperPtr = field_wrapper::VariantWrapperPtr;
    using CreateMemberFieldWidgetFunc =
        std::function<FieldWidgetPtr (field_wrapper::FieldWrapper&)>;

    explicit VariantFieldWidget(
        WrapperPtr&& wrapper,
        CreateMemberFieldWidgetFunc&& func,
        QWidget* parentObj = nullptr);

    ~VariantFieldWidget() noexcept;

    void setMemberField(FieldWidget* memberFieldWidget);

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void memberFieldUpdated();
    void indexUpdated(int value);
    void memberComboUpdated(int value);
    void destroyMemberWidget();
    void createMemberWidget();

private:
    using WrapperType = WrapperPtr::element_type;

    void refreshInternal();
    void refreshMember();
    void updateMemberProps();
    void updateIndexDisplay();
    void updateIndexValue();
    void updateMemberCombo();

    Ui::VariantFieldWidget m_ui;
    WrapperPtr m_wrapper;
    FieldWidget* m_member = nullptr;
    QList<QVariantMap> m_membersProps;
    CreateMemberFieldWidgetFunc m_createFunc;
    bool m_indexHidden = false;
};


}  // namespace comms_champion


