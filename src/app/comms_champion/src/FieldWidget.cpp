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
    setEditEnabledImpl(enabled);
}

void FieldWidget::propertiesUpdated()
{
    performNameLabelUpdate();
    propertiesUpdatedImpl();
    performUiElementsVisibilityCheck();
}

void FieldWidget::emitFieldUpdated()
{
    emit sigFieldUpdated();
}

bool FieldWidget::isEditEnabled() const
{
    return m_editEnabled;
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

void FieldWidget::updateNameLabel(QLabel& label)
{
    auto nameProperty = property(Property::name());
    if (nameProperty.isValid()) {
        auto str = nameProperty.toString();
        if (!str.isEmpty()) {
            label.setText(nameProperty.toString() + ':');
            label.show();
        }
        else {
            label.hide();
        }
    }
}

void FieldWidget::setEditEnabledImpl(bool enabled)
{
    static_cast<void>(enabled);
}

void FieldWidget::propertiesUpdatedImpl()
{
}

void FieldWidget::performUiElementsVisibilityCheck()
{
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


    auto allHiddenVar = Property::getFieldHiddenVal(*this);
    if (allHiddenVar.isValid() && allHiddenVar.canConvert<bool>()) {
        auto allHidden = allHiddenVar.toBool();
        setWidgetHiddenFunc(m_valueWidget, allHidden);
        setWidgetHiddenFunc(m_sepWidget, allHidden);
        setWidgetHiddenFunc(m_serValueWidget, allHidden);

        if (allHidden) {
            return;
        }
    }


    auto serHiddenVar = Property::getSerialisedHiddenVal(*this);
    if (serHiddenVar.isValid() && serHiddenVar.canConvert<bool>()) {
        auto serHidden = serHiddenVar.toBool();
        setWidgetHiddenFunc(m_sepWidget, serHidden);
        setWidgetHiddenFunc(m_serValueWidget, serHidden);
    }
}

void FieldWidget::performNameLabelUpdate()
{
    if (m_nameLabel != nullptr) {
        updateNameLabel(*m_nameLabel);
    }
}

}  // namespace comms_champion

