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

#include "LongLongIntValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <cmath>

#include "comms_champion/property/field.h"
#include "SpecialValueWidget.h"

namespace comms_champion
{

LongLongIntValueFieldWidget::LongLongIntValueFieldWidget(
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
    m_ui.m_valueLineEdit->setText(QString("%1").arg(adjustRealToDisplayed(m_wrapper->getValue())));

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->minWidth(), m_wrapper->maxWidth());

    connect(m_ui.m_valueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(valueUpdated(const QString&)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    refresh();
}

LongLongIntValueFieldWidget::~LongLongIntValueFieldWidget() noexcept = default;

void LongLongIntValueFieldWidget::refreshImpl()
{
    assert(m_wrapper->canWrite());
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

    auto value = m_wrapper->getValue();
    assert(m_ui.m_valueLineEdit);
    if (adjustDisplayedToReal(getDisplayedValue(m_ui.m_valueLineEdit->text())) != value) {
        m_ui.m_valueLineEdit->setText(QString("%1").arg(adjustRealToDisplayed(value)));
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

void LongLongIntValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_valueLineEdit->setReadOnly(readonly);
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void LongLongIntValueFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    property::field::IntValue actProps(props);

    auto offset =
        static_cast<decltype(m_offset)>(actProps.displayOffset());

    bool needRefresh = false;
    if (std::numeric_limits<double>::epsilon() < std::abs(m_offset - offset)) {
        m_offset = offset;
        needRefresh = true;
    }

    auto& specials = actProps.specials();
    needRefresh = createSpecialsWidget(specials) || needRefresh;

    if (needRefresh) {
        refresh();
    }
}

void LongLongIntValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
}

void LongLongIntValueFieldWidget::valueUpdated(const QString& value)
{
    auto adjustedValue = adjustDisplayedToReal(getDisplayedValue(value));
    if (adjustedValue == m_wrapper->getValue()) {
        return;
    }

    assert(isEditEnabled());
    m_wrapper->setValue(adjustedValue);
    assert(m_wrapper->getValue() == adjustedValue);
    refresh();
    emitFieldUpdated();
}

void LongLongIntValueFieldWidget::specialSelected(long long value)
{
    if (!isEditEnabled()) {
        refresh();
        return;
    }

    valueUpdated(QString("%1").arg(adjustRealToDisplayed(static_cast<UnderlyingType>(value))));
}

LongLongIntValueFieldWidget::UnderlyingType
LongLongIntValueFieldWidget::adjustDisplayedToReal(DisplayedType val)
{
    return static_cast<UnderlyingType>(val - m_offset);
}

LongLongIntValueFieldWidget::DisplayedType
LongLongIntValueFieldWidget::adjustRealToDisplayed(UnderlyingType val)
{
    return static_cast<DisplayedType>(val + m_offset);
}

LongLongIntValueFieldWidget::DisplayedType
LongLongIntValueFieldWidget::getDisplayedValue(const QString& value)
{
    if ((value.isEmpty()) || (value == " ") || (value == "-") || (value == "+")) {
        return 0;
    }

    bool ok = false;
    auto val = value.toLongLong(&ok);
    if (ok) {
        return val;
    }

    static const int MinLength = std::numeric_limits<qlonglong>::digits10;
    assert(MinLength < value.size());
    QString valueCpy(value);
    while ((!ok) && (MinLength < valueCpy.size())) {
        valueCpy.resize(valueCpy.size() - 1);
        val = value.toLongLong(&ok);
    }

    return val;
}

bool LongLongIntValueFieldWidget::createSpecialsWidget(const SpecialsList& specials)
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


