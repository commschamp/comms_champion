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

#include "LongIntValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <limits>

#include "comms_champion/Property.h"

namespace comms_champion
{

LongIntValueFieldWidget::LongIntValueFieldWidget(
    WrapperPtr wrapper,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->minWidth(), m_wrapper->maxWidth());

    m_ui.m_valueSpinBox->setRange(
        static_cast<double>(m_wrapper->minValue()),
        static_cast<double>(m_wrapper->maxValue()));
    m_ui.m_valueSpinBox->setDecimals(0);

    connect(m_ui.m_valueSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(valueUpdated(double)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    refresh();
}

LongIntValueFieldWidget::~LongIntValueFieldWidget() = default;

void LongIntValueFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

    auto value = m_wrapper->getValue();
    assert(m_ui.m_valueSpinBox);
    if (static_cast<UnderlyingType>(m_ui.m_valueSpinBox->value()) != value) {
        m_ui.m_valueSpinBox->setValue(static_cast<double>(value));
    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void LongIntValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_valueSpinBox->setReadOnly(readonly);
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void LongIntValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
}

void LongIntValueFieldWidget::valueUpdated(double value)
{
    auto castedValue = static_cast<UnderlyingType>(value);
    if (castedValue == m_wrapper->getValue()) {
        return;
    }

    assert(isEditEnabled());
    m_wrapper->setValue(castedValue);
    refresh();
    emitFieldUpdated();
}

}  // namespace comms_champion


