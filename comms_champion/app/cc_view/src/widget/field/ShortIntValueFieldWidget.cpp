//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "ShortIntValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <limits>

#include "comms_champion/property/field.h"
#include "SpecialValueWidget.h"

namespace comms_champion
{

ShortIntValueFieldWidget::ShortIntValueFieldWidget(
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

    m_ui.m_valueSpinBox->setRange((int)m_wrapper->minValue(), (int)m_wrapper->maxValue());

    connect(m_ui.m_valueSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(valueUpdated(int)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    refresh();
}

ShortIntValueFieldWidget::~ShortIntValueFieldWidget() noexcept = default;

void ShortIntValueFieldWidget::refreshImpl()
{
    assert(m_wrapper->canWrite());
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

    auto value = adjustRealToDisplayed(m_wrapper->getValue());
    assert(m_ui.m_valueSpinBox);
    if (m_ui.m_valueSpinBox->value() != value) {
        m_ui.m_valueSpinBox->setValue(value);
    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);

    if (m_specialsWidget != nullptr) {
        m_specialsWidget->setIntValue(m_wrapper->getValue());
    }
}

void ShortIntValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_valueSpinBox->setReadOnly(readonly);
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void ShortIntValueFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    property::field::IntValue actProps(props);

    auto offset =
        static_cast<decltype(m_offset)>(actProps.displayOffset());

    bool needRefresh = false;
    if (m_offset != offset) {
        m_offset = offset;
        needRefresh = true;
    }

    auto& specials = actProps.specials();
    needRefresh = createSpecialsWidget(specials) || needRefresh;

    if (needRefresh) {
        refresh();
    }
}

void ShortIntValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
}

void ShortIntValueFieldWidget::valueUpdated(int value)
{
    if (value == adjustRealToDisplayed(m_wrapper->getValue())) {
        return;
    }

    assert(isEditEnabled());
    m_wrapper->setValue(adjustDisplayedToReal(value));
    refresh();
    emitFieldUpdated();
}

void ShortIntValueFieldWidget::specialSelected(long long value)
{
    if (!isEditEnabled()) {
        refresh();
        return;
    }

    valueUpdated(adjustRealToDisplayed(static_cast<int>(value)));
}

int ShortIntValueFieldWidget::adjustDisplayedToReal(int val)
{
    return val - m_offset;
}

int ShortIntValueFieldWidget::adjustRealToDisplayed(int val)
{
    return val + m_offset;
}

bool ShortIntValueFieldWidget::createSpecialsWidget(const SpecialsList& specials)
{
    delete m_specialsWidget;
    if (specials.empty()) {
        m_specialsWidget = nullptr;
        return false;
    }

    m_specialsWidget = new SpecialValueWidget(specials);
    connect(
        m_specialsWidget, SIGNAL(sigIntValueChanged(long long)),
        this, SLOT(specialSelected(long long)));

    connect(
        m_specialsWidget, SIGNAL(sigRefreshReq()),
        this, SLOT(refresh()));

    m_ui.m_valueWidgetLayout->insertWidget(m_ui.m_valueWidgetLayout->count() - 1, m_specialsWidget);

    return true;
}

}  // namespace comms_champion


