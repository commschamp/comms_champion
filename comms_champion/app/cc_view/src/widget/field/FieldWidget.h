//
// Copyright 2014 - 2017 (C). Alex Robenko. All rights reserved.
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

#include <cassert>
#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariantMap>
#include <QtWidgets/QWidget>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/field.h"
#include "comms_champion/field_wrapper/FieldWrapper.h"

class QLineEdit;
class QLabel;
class QPlainTextEdit;
class QSpinBox;

namespace comms_champion
{

class FieldWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget Base;
public:
    FieldWidget(QWidget* parentObj = nullptr);
    ~FieldWidget() noexcept = default;

public slots:
    void refresh();
    void setEditEnabled(bool enabled);
    void updateProperties(const QVariantMap& props);

signals:
    void sigFieldUpdated();

protected:
    void emitFieldUpdated();
    bool isEditEnabled() const;

    static void setValidityStyleSheet(QLabel& widget, bool valid);
    static void setValidityStyleSheet(QLineEdit& widget, bool valid);
    static void setValidityStyleSheet(QPlainTextEdit& widget, bool valid);
    static void setSerialisedInputMask(QLineEdit& line, int minWidth, int maxWidth);
    static void setSerialisedInputMask(QLineEdit& line, int width);
    static void updateValue(QLineEdit& line, const QString& value);
    static void updateSerValue(QPlainTextEdit& text, const field_wrapper::FieldWrapper& wrapper);

    template <typename TWrapper>
    void handleNumericSerialisedValueUpdate(
        const QString& value,
        TWrapper& wrapper)
    {
        assert(isEditEnabled());
        do {
            if ((value.size() & 0x1U) == 0) {
               wrapper.setSerialisedString(value);
               break;
            }

            QString valueCpy(value);
            valueCpy.append(QChar('0'));
            wrapper.setSerialisedString(valueCpy);
        } while (false);
        refresh();
        emitFieldUpdated();
    }

    void setNameLabelWidget(QLabel* widget)
    {
        m_nameLabel = widget;
    }

    void setValueWidget(QWidget* widget)
    {
        m_valueWidget = widget;
    }

    void setSeparatorWidget(QWidget* widget)
    {
        m_sepWidget = widget;
    }

    void setSerialisedValueWidget(QWidget* widget)
    {
        m_serValueWidget = widget;
    }

    virtual void refreshImpl() = 0;
    virtual void editEnabledUpdatedImpl();
    virtual void updatePropertiesImpl(const QVariantMap& props);

private:
    void performUiElementsVisibilityCheck(const property::field::Common& props);
    void performUiReadOnlyCheck(const property::field::Common& props);
    void performNameLabelUpdate(const property::field::Common& props);

    bool m_forcedReadOnly = false;
    bool m_editEnabled = true;
    QLabel* m_nameLabel = nullptr;
    QWidget* m_valueWidget = nullptr;
    QWidget* m_sepWidget = nullptr;
    QWidget* m_serValueWidget = nullptr;
};

typedef std::unique_ptr<FieldWidget> FieldWidgetPtr;

}  // namespace comms_champion




