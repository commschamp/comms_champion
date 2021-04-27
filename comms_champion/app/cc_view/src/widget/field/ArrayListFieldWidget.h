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

#include <functional>
#include <vector>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariantMap>

#include "ui_ArrayListElementWidget.h"
#include "ui_ArrayListFieldWidget.h"
CC_ENABLE_WARNINGS()

#include "comms_champion/field_wrapper/ArrayListWrapper.h"
#include "FieldWidget.h"

namespace comms_champion
{

class ArrayListElementWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget Base;
public:

    ArrayListElementWidget(
        FieldWidget* fieldWidget,
        QWidget* parentObj = nullptr);

    void refresh();
    void setEditEnabled(bool enabled);
    void setDeletable(bool deletable);
    void updateProperties(const QVariantMap& props);
    void setNameSuffix(const QString& value);

signals:
    void sigFieldUpdated();
    void sigRemoveRequested();

private:

    void updateUi();
    Ui::ArrayListElementWidget m_ui;
    FieldWidget* m_fieldWidget = nullptr;
    bool m_editEnabled = true;
    bool m_deletable = true;
};

class ArrayListFieldWidget : public FieldWidget
{
    Q_OBJECT
    typedef FieldWidget Base;
public:
    using Wrapper = field_wrapper::ArrayListWrapper;
    using WrapperPtr = Wrapper::Ptr;
    typedef std::function<std::vector<FieldWidgetPtr> (Wrapper&)> CreateMissingDataFieldsFunc;

    explicit ArrayListFieldWidget(
        WrapperPtr wrapper,
        CreateMissingDataFieldsFunc&& updateFunc,
        QWidget* parentObj = nullptr);

    ~ArrayListFieldWidget() noexcept;

    template <typename TFunc>
    void setCreateMissingDataFieldsCallback(TFunc&& func)
    {
        m_createMissingDataFieldsCallback = std::forward<TFunc>(func);
        addMissingFields();
    }

protected:
    virtual void refreshImpl() override;
    virtual void editEnabledUpdatedImpl() override;
    virtual void updatePropertiesImpl(const QVariantMap& props) override;

private slots:
    void dataFieldUpdated();
    void addNewField();
    void removeField();

private:

    void addDataField(FieldWidget* dataFieldWidget);
    void refreshInternal();
    void updateUi();
    void addMissingFields();
    void updatePrefixField();

    Ui::ArrayListFieldWidget m_ui;
    WrapperPtr m_wrapper;
    std::vector<ArrayListElementWidget*> m_elements;
    CreateMissingDataFieldsFunc m_createMissingDataFieldsCallback;
    std::vector<QVariantMap> m_elemProperties;
    bool m_prefixVisible = false;
    bool m_appendIndexToElementName = false;
};

}  // namespace comms_champion


