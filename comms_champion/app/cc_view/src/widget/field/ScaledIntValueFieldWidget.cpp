//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "ScaledIntValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <cmath>

#include "comms_champion/property/field.h"

namespace comms_champion
{

namespace
{

const int DefaultInitialDecimals = 6;

} // namespace

ScaledIntValueFieldWidget::ScaledIntValueFieldWidget(
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
        m_wrapper->scaleValue(m_wrapper->minValue()),
        m_wrapper->scaleValue(m_wrapper->maxValue()));
    m_ui.m_valueSpinBox->setDecimals(DefaultInitialDecimals);

    refresh();

    connect(m_ui.m_valueSpinBox, SIGNAL(valueChanged(double)),
            this, SLOT(valueUpdated(double)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));
}

ScaledIntValueFieldWidget::~ScaledIntValueFieldWidget() noexcept = default;

void ScaledIntValueFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

    auto value = m_wrapper->getScaled();
    assert(m_ui.m_valueSpinBox);
    if (m_ui.m_valueSpinBox->value() != value) {
        m_ui.m_valueSpinBox->setValue(value);
    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void ScaledIntValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_valueSpinBox->setReadOnly(readonly);
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void ScaledIntValueFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    auto decimals = property::field::IntValue(props).scaledDecimals();
    if (decimals <= 0) {
        assert(!"Should not happen");
        m_ui.m_valueSpinBox->setDecimals(0);
        return;
    }

    m_ui.m_valueSpinBox->setDecimals(decimals);
}

void ScaledIntValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    m_ui.m_valueSpinBox->blockSignals(true);
    handleNumericSerialisedValueUpdate(
        value,
        *m_wrapper,
        [this]()
        {
            m_ui.m_valueSpinBox->blockSignals(false);
        });
}

void ScaledIntValueFieldWidget::valueUpdated(double value)
{
    if (std::abs(value - m_wrapper->getScaled()) < std::numeric_limits<double>::epsilon()) {
        return;
    }

    assert(isEditEnabled());
    m_wrapper->setScaled(value);

    refresh();
    emitFieldUpdated();
}

}  // namespace comms_champion


