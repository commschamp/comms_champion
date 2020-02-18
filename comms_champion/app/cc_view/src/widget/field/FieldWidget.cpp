//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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


#include "FieldWidget.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <cassert>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpinBox>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

namespace
{

void updateValidityStyle(
    QWidget& widget,
    bool valid,
    const QString& invalidStylesheet)
{
    static const auto DefaultStylesheet = QWidget().styleSheet();
    auto* stylesheet = &DefaultStylesheet;
    if (!valid) {
        stylesheet = &invalidStylesheet;
    }

    widget.setStyleSheet(*stylesheet);
}

}  // namespace

FieldWidget::FieldWidget(QWidget* parentObj)
  : Base(parentObj)
{
}

void FieldWidget::refresh()
{
    refreshImpl();
}

void FieldWidget::setEditEnabled(bool enabled)
{
    m_editEnabled = enabled;
    if ((!m_editEnabled) && (m_hiddenWhenReadOnly) && (!isHidden())) {
        setHidden(true);
    }
    editEnabledUpdatedImpl();
}

void FieldWidget::updateProperties(const QVariantMap& props)
{
    property::field::Common commonProps(props);
    m_hiddenWhenReadOnly = commonProps.isHiddenWhenReadOnly();
    performNameLabelUpdate(commonProps);
    updatePropertiesImpl(props);
    performUiElementsVisibilityCheck(commonProps);
    performUiReadOnlyCheck(commonProps);
}

void FieldWidget::emitFieldUpdated()
{
    emit sigFieldUpdated();
}

bool FieldWidget::isEditEnabled() const
{
    return m_editEnabled && (!m_forcedReadOnly);
}

void FieldWidget::setValidityStyleSheet(QLabel& widget, bool valid)
{
    static const QString InvalidStylesheet("QLabel { color: red }");
    updateValidityStyle(widget, valid, InvalidStylesheet);
}

void FieldWidget::setValidityStyleSheet(QLineEdit& widget, bool valid)
{
    static const QString InvalidStylesheet("QLineEdit { color: red }");
    updateValidityStyle(widget, valid, InvalidStylesheet);
}

void FieldWidget::setValidityStyleSheet(QPlainTextEdit& widget, bool valid)
{
    static const QString InvalidStylesheet("QPlainTextEdit { color: red }");
    updateValidityStyle(widget, valid, InvalidStylesheet);
}

void FieldWidget::setSerialisedInputMask(
    QLineEdit& line,
    int minWidth,
    int maxWidth)
{
    auto maskMinWidth = static_cast<std::size_t>(minWidth);
    auto maskMaxWidth = static_cast<std::size_t>(maxWidth);
    assert(maskMinWidth <= maskMaxWidth);
    QString mask;
    mask.reserve(maskMaxWidth);
    std::fill_n(std::back_inserter(mask), maskMinWidth, 'H');
    std::fill_n(std::back_inserter(mask), maskMaxWidth - maskMinWidth, 'h');
    line.setInputMask(mask);
}

void FieldWidget::setSerialisedInputMask(
    QLineEdit& line,
    int width)
{
    setSerialisedInputMask(line, width, width);
}

void FieldWidget::updateValue(QLineEdit& line, const QString& value)
{
    if (line.text() == value) {
        return;
    }

    auto cursorPos = std::min(value.size(), line.cursorPosition());
    line.setText(value);
    line.setCursorPosition(cursorPos);
}

void FieldWidget::updateSerValue(
    QPlainTextEdit& text,
    const field_wrapper::FieldWrapper& wrapper)
{
    QString serValueStr;
    auto serValue = wrapper.getSerialisedValue();

    for (auto byte : serValue) {
        if (!serValueStr.isEmpty()) {
            serValueStr.append(QChar(' '));
        }
        serValueStr.append(QString("%1").arg(byte, 2, 16, QChar('0')));
    }

    text.setPlainText(serValueStr);
}

void FieldWidget::editEnabledUpdatedImpl()
{
}

void FieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    static_cast<void>(props);
}

void FieldWidget::performUiElementsVisibilityCheck(const property::field::Common& props)
{
    auto allHidden =
        (props.isHidden()) ||
        (props.isReadOnly() && props.isHiddenWhenReadOnly());
    setHidden(allHidden);
    if (allHidden) {
        return;
    }

    if ((m_valueWidget == nullptr) &&
        (m_sepWidget == nullptr) &&
        (m_serValueWidget == nullptr)) {
        return;
    }

    auto setWidgetHiddenFunc =
        [](QWidget* widget, bool hidden)
        {
            if (widget != nullptr) {
                widget->setHidden(hidden);
            }
        };

    auto serHidden = props.isSerialisedHidden();
    setWidgetHiddenFunc(m_sepWidget, serHidden);
    setWidgetHiddenFunc(m_serValueWidget, serHidden);
}

void FieldWidget::performUiReadOnlyCheck(const property::field::Common& props)
{
    auto readOnly = props.isReadOnly();
    if (m_forcedReadOnly != readOnly) {
        m_forcedReadOnly = readOnly;
        editEnabledUpdatedImpl();
    }
}

void FieldWidget::performNameLabelUpdate(const property::field::Common& props)
{
    if (m_nameLabel == nullptr) {
        return;
    }

    auto str = props.name();
    if (str.isEmpty()) {
        m_nameLabel->hide();
        return;
    }

    if (!m_nameSuffix.isEmpty()) {
        str.append(m_nameSuffix);
    }

    str.append(':');
    m_nameLabel->setText(str);
    m_nameLabel->show();
}

}  // namespace comms_champion

