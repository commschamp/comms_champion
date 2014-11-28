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

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include "GlobalConstants.h"

namespace comms_champion
{

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
    propertiesUpdatedImpl();
}

void FieldWidget::emitFieldUpdated()
{
    emit sigFieldUpdated();
}

bool FieldWidget::isEditEnabled() const
{
    return m_editEnabled;
}

void FieldWidget::setValidityStyleSheet(QWidget& widget, bool valid)
{
    static const auto DefaultStylesheet = QWidget().styleSheet();
    static const QString InvalidStylesheet("QLabel { color: red }");
    auto* stylesheet = &DefaultStylesheet;
    if (!valid) {
        stylesheet = &InvalidStylesheet;
    }

    widget.setStyleSheet(*stylesheet);
}

void FieldWidget::setSerialisedInputMask(QLineEdit& line, int width)
{
    auto maskWidth = static_cast<std::size_t>(width);
    std::string mask;
    mask.reserve(maskWidth);
    std::fill_n(std::back_inserter(mask), maskWidth, 'H');
    line.setInputMask(mask.c_str());
}

void FieldWidget::updateNameLabel(QLabel& label)
{
    auto nameProperty = property(GlobalConstants::namePropertyName());
    if (nameProperty.isValid()) {
        label.setText(nameProperty.toString() + ':');
    }
}

void FieldWidget::setEditEnabledImpl(bool enabled)
{
    static_cast<void>(enabled);
}

void FieldWidget::propertiesUpdatedImpl()
{
}

void FieldWidget::updateNumericSerialisedValueInternal(
    QLineEdit& line,
    unsigned long long value,
    int width)
{
    auto serValueStr =
        QString("%1").arg(value, width, 16, QChar('0'));
    if (line.text() != serValueStr) {
        line.setText(serValueStr);
    }
}

}  // namespace comms_champion

