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

#include "BasicIntValueFieldWidget.h"

#include <algorithm>
#include <cassert>

#include "GlobalConstants.h"

namespace comms_champion
{

BasicIntValueFieldWidget::BasicIntValueFieldWidget(
    field_wrapper::BasicIntValueWrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);

    auto maskWidth = static_cast<std::size_t>(m_wrapper->length() * 2);
    std::string mask;
    mask.reserve(maskWidth);
    std::fill_n(std::back_inserter(mask), maskWidth, 'H');
    m_ui.m_serValueLineEdit->setInputMask(mask.c_str());

    m_ui.m_valueSpinBox->setRange(m_wrapper->minValue(), m_wrapper->maxValue());

    m_defaultStyleSheet = m_ui.m_nameLabel->styleSheet();

    connect(m_ui.m_valueSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(valueUpdated(int)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    refresh();
    readPropertiesAndUpdateUi();
}

BasicIntValueFieldWidget::~BasicIntValueFieldWidget() = default;

void BasicIntValueFieldWidget::refreshImpl()
{
    auto serValueStr =
        QString("%1").arg(m_wrapper->serialisedValue(), m_wrapper->width(), 16, QChar('0'));
    m_ui.m_serValueLineEdit->setText(serValueStr);
    m_ui.m_valueSpinBox->setValue(m_wrapper->value());
    static const QString InvalidLabelStyleSheet("QLabel { color: red }");
    if (m_wrapper->valid()) {
        m_ui.m_nameLabel->setStyleSheet(m_defaultStyleSheet);
        m_ui.m_serFrontLabel->setStyleSheet(m_defaultStyleSheet);
        m_ui.m_serBackLabel->setStyleSheet(m_defaultStyleSheet);
    }
    else {
        m_ui.m_nameLabel->setStyleSheet(InvalidLabelStyleSheet);
        m_ui.m_serFrontLabel->setStyleSheet(InvalidLabelStyleSheet);
        m_ui.m_serBackLabel->setStyleSheet(InvalidLabelStyleSheet);
    }
}

void BasicIntValueFieldWidget::setEditEnabledImpl(bool enabled)
{
    m_ui.m_valueSpinBox->setReadOnly(enabled);
    m_ui.m_serValueLineEdit->setReadOnly(enabled);
}

void BasicIntValueFieldWidget::propertiesUpdatedImpl()
{
    readPropertiesAndUpdateUi();
}

void BasicIntValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    static_cast<void>(value);
    bool ok = false;
    int serValue = value.toInt(&ok, 16);
    assert(ok);
    static_cast<void>(ok);
    if (serValue == m_wrapper->serialisedValue()) {
        return;
    }
    m_wrapper->setSerialisedValue(serValue);
    refresh();
}

void BasicIntValueFieldWidget::valueUpdated(int value)
{
    if (value == m_wrapper->value()) {
        return;
    }

    assert(isEditEnabled());
    m_wrapper->setValue(value);
    refresh();
}

void BasicIntValueFieldWidget::readPropertiesAndUpdateUi()
{
    auto nameProperty = property(GlobalConstants::namePropertyName());
    if (nameProperty.isValid()) {
        m_ui.m_nameLabel->setText(nameProperty.toString() + ':');
    }
}

}  // namespace comms_champion


