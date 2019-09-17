//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "UnsignedLongLongIntValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <cmath>

#include "comms_champion/property/field.h"
#include "SpecialValueWidget.h"

namespace comms_champion
{

UnsignedLongLongIntValueFieldWidget::UnsignedLongLongIntValueFieldWidget(
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

    refresh();

    connect(m_ui.m_valueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(valueUpdated(const QString&)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

}

UnsignedLongLongIntValueFieldWidget::~UnsignedLongLongIntValueFieldWidget() noexcept = default;

void UnsignedLongLongIntValueFieldWidget::refreshImpl()
{
    assert(m_wrapper->canWrite());
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

    auto value = m_wrapper->getValue();
    assert(m_ui.m_valueLineEdit);
    auto valueTxt =
            QString("%1")
                .arg(adjustRealToDisplayed(value), 0, 'f', m_decimals, QChar('0'));
    m_ui.m_valueLineEdit->setText(valueTxt);

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);

    if (m_specialsWidget != nullptr) {
        m_specialsWidget->setIntValue(static_cast<long long>(m_wrapper->getValue()));
    }
}

void UnsignedLongLongIntValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_valueLineEdit->setReadOnly(readonly);
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void UnsignedLongLongIntValueFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    property::field::IntValue parsedProps(props);
    m_offset = parsedProps.displayOffset();
    m_decimals = parsedProps.scaledDecimals();
    auto& specials = parsedProps.specials();
    createSpecialsWidget(specials);
    refresh();
}

void UnsignedLongLongIntValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
}

void UnsignedLongLongIntValueFieldWidget::valueUpdated(const QString& value)
{
    auto adjustedValue = adjustDisplayedToReal(getDisplayedValue(value));
    if (adjustedValue == m_wrapper->getValue()) {
        return;
    }

    assert(isEditEnabled());
    auto oldValue = m_wrapper->getValue();
    m_wrapper->setValue(adjustedValue);
    assert(m_wrapper->getValue() == adjustedValue);
    if (!m_wrapper->canWrite()) {
        m_wrapper->setValue(oldValue);
    }
    refresh();
    emitFieldUpdated();
}

void UnsignedLongLongIntValueFieldWidget::specialSelected(long long value)
{
    if (!isEditEnabled()) {
        refresh();
        return;
    }

    m_wrapper->setValue(static_cast<unsigned long long>(value));
    refresh();
}

UnsignedLongLongIntValueFieldWidget::UnderlyingType
UnsignedLongLongIntValueFieldWidget::adjustDisplayedToReal(DisplayedType val)
{
    auto uVal = static_cast<unsigned long long>(std::llround(val));
    if (0 < m_decimals) {
        uVal = static_cast<decltype(uVal)>(val * std::pow(10, m_decimals));
    }
    return static_cast<UnderlyingType>(uVal - m_offset);
}

UnsignedLongLongIntValueFieldWidget::DisplayedType
UnsignedLongLongIntValueFieldWidget::adjustRealToDisplayed(UnderlyingType val)
{
    auto dVal = static_cast<DisplayedType>(val + m_offset);
    if (0 < m_decimals) {
        dVal /= std::pow(10, m_decimals);
    }
    return dVal;
}

UnsignedLongLongIntValueFieldWidget::DisplayedType
UnsignedLongLongIntValueFieldWidget::getDisplayedValue(const QString& value)
{
    bool ok = false;
    auto val = value.toDouble(&ok);
    return val;
}

bool UnsignedLongLongIntValueFieldWidget::createSpecialsWidget(const SpecialsList& specials)
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

    m_ui.m_valueWidgetLayout->insertWidget(m_ui.m_valueWidgetLayout->count(), m_specialsWidget);

    return true;
}

}  // namespace comms_champion


