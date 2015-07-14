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


#include "comms_champion/FieldWidget.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <cassert>

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpinBox>

#include "comms_champion/Property.h"

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

FieldWidget::FieldWidget(QWidget* parent)
  : Base(parent)
{
}

void FieldWidget::refresh()
{
    refreshImpl();
}

void FieldWidget::setEditEnabled(bool enabled)
{
    m_editEnabled = enabled;
    editEnabledUpdatedImpl();
}

void FieldWidget::updateProperties(const QVariantMap& props)
{
    performNameLabelUpdate(props);
    updatePropertiesImpl(props);
    performUiElementsVisibilityCheck(props);
    performUiReadOnlyCheck(props);
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

void FieldWidget::editEnabledUpdatedImpl()
{
}

void FieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    static_cast<void>(props);
}

void FieldWidget::performUiElementsVisibilityCheck(const QVariantMap& props)
{
    auto allHiddenVar = props.value(Property::fieldHidden());
    if (allHiddenVar.isValid() && allHiddenVar.canConvert<bool>()) {
        auto allHidden = allHiddenVar.toBool();
        setHidden(allHidden);

        if (allHidden) {
            return;
        }
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

    auto serHiddenVar = props.value(Property::serialisedHidden());
    if (serHiddenVar.isValid() && serHiddenVar.canConvert<bool>()) {
        auto serHidden = serHiddenVar.toBool();
        setWidgetHiddenFunc(m_sepWidget, serHidden);
        setWidgetHiddenFunc(m_serValueWidget, serHidden);
    }
}

void FieldWidget::performUiReadOnlyCheck(const QVariantMap& props)
{
    auto readOnlyVar = props.value(Property::readOnly());
    if (readOnlyVar.isValid() && readOnlyVar.canConvert<bool>()) {
        m_forcedReadOnly = readOnlyVar.value<bool>();
        editEnabledUpdatedImpl();
    }
}

void FieldWidget::performNameLabelUpdate(const QVariantMap& props)
{
    if (m_nameLabel == nullptr) {
        return;
    }

    auto nameProperty = props.value(Property::name());
    if ((!nameProperty.isValid()) || (!nameProperty.canConvert<QString>())) {
        return;
    }

    auto str = nameProperty.toString();
    if (str.isEmpty()) {
        m_nameLabel->hide();
        return;
    }

    str.append(':');
    m_nameLabel->setText(str);
    m_nameLabel->show();
}

}  // namespace comms_champion

