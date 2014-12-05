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
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());

    m_ui.m_valueSpinBox->setRange(m_wrapper->minValue(), m_wrapper->maxValue());

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
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateNumericSerialisedValue(
        *m_ui.m_serValueLineEdit,
        m_wrapper->serialisedValue(),
        m_wrapper->width());

    auto value = m_wrapper->value();
    if (m_ui.m_valueSpinBox->value() != value) {
        m_ui.m_valueSpinBox->setValue(value);
    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void BasicIntValueFieldWidget::setEditEnabledImpl(bool enabled)
{
    bool readonly = !enabled;
    m_ui.m_valueSpinBox->setReadOnly(readonly);
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void BasicIntValueFieldWidget::propertiesUpdatedImpl()
{
    readPropertiesAndUpdateUi();
}

void BasicIntValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    static_assert(std::is_same<int, UnderlyingType>::value,
        "Underlying type assumption is wrong");

    bool ok = false;
    UnderlyingType serValue = value.toInt(&ok, 16);
    assert(ok);
    static_cast<void>(ok);
    if (serValue == m_wrapper->serialisedValue()) {
        return;
    }
    m_wrapper->setSerialisedValue(serValue);
    emitFieldUpdated();
    refresh();
}

void BasicIntValueFieldWidget::valueUpdated(int value)
{
    if (value == m_wrapper->value()) {
        return;
    }

    assert(isEditEnabled());
    m_wrapper->setValue(value);
    emitFieldUpdated();
    refresh();
}

void BasicIntValueFieldWidget::readPropertiesAndUpdateUi()
{
    assert(m_ui.m_nameLabel != nullptr);
    updateNameLabel(*m_ui.m_nameLabel);
}

}  // namespace comms_champion


