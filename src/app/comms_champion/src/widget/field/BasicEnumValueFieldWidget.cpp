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

#include "BasicEnumValueFieldWidget.h"

#include <algorithm>
#include <cassert>

#include "GlobalConstants.h"

namespace comms_champion
{

namespace
{

const QString InvalidValueComboText("???");
const int EnumValuesStartIndex = 2;

}  // namespace

BasicEnumValueFieldWidget::BasicEnumValueFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());
}

BasicEnumValueFieldWidget::~BasicEnumValueFieldWidget() = default;

void BasicEnumValueFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateNumericSerialisedValue(
        *m_ui.m_serValueLineEdit,
        m_wrapper->serialisedValue(),
        m_wrapper->width());

    bool valid = m_wrapper->valid();
    auto comboIdx = m_ui.m_valueComboBox->currentIndex();

    auto comboRetrieveValueFunc =
        [this](int idx)
        {
            auto comboValueVar = m_ui.m_valueComboBox->itemData(idx);
            assert(comboValueVar.isValid());
            assert(comboValueVar.canConvert<UnderlyingType>());
            return comboValueVar.value<UnderlyingType>();
        };

    do {
        if ((!valid) && (comboIdx < EnumValuesStartIndex)) {
            break;
        }

        auto value = m_wrapper->value();
        auto comboValue = comboRetrieveValueFunc(m_ui.m_valueComboBox->currentIndex());
        if (value == comboValue) {
            break;
        }

        bool foundMatch = false;
        auto itemsCount = m_ui.m_valueComboBox->count();
        for (auto idx = EnumValuesStartIndex; idx < itemsCount; ++idx) {
            comboValue = comboRetrieveValueFunc(idx);
            if (value == comboValue) {
                m_ui.m_valueComboBox->setCurrentIndex(idx);
                foundMatch = true;
                break;
            }
        }

        if (foundMatch) {
            break;
        }

        m_ui.m_valueComboBox->setCurrentIndex(0);

    } while (false);

    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void BasicEnumValueFieldWidget::setEditEnabledImpl(bool enabled)
{
    bool readonly = !enabled;
//    m_ui.m_valueSpinBox->setReadOnly(readonly);
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void BasicEnumValueFieldWidget::propertiesUpdatedImpl()
{
    readPropertiesAndUpdateUi();
    refresh();
}

void BasicEnumValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    static_assert(std::is_same<long long int, UnderlyingType>::value,
        "Underlying type assumption is wrong");

    bool ok = false;
    UnderlyingType serValue = value.toLongLong(&ok, 16);
    assert(ok);
    static_cast<void>(ok);
    if (serValue == m_wrapper->serialisedValue()) {
        return;
    }
    m_wrapper->setSerialisedValue(serValue);
    emitFieldUpdated();
    refresh();
}

void BasicEnumValueFieldWidget::valueUpdated(int idx)
{
    if ((!m_wrapper->valid()) && (idx < EnumValuesStartIndex)) {
        return;
    }

    if (isEditEnabled()) {
        auto valueVar = m_ui.m_valueComboBox->currentData();
        assert(valueVar.isValid());
        assert(valueVar.canConvert<UnderlyingType>());
        auto value = valueVar.value<UnderlyingType>();
        if (value == m_wrapper->value()) {
            return;
        }

        assert(isEditEnabled());
        m_wrapper->setValue(value);
        emitFieldUpdated();
    }
    refresh();
}

void BasicEnumValueFieldWidget::readPropertiesAndUpdateUi()
{
    if (m_signalsConnected) {
        disconnect(m_ui.m_valueComboBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(valueUpdated(int)));

        disconnect(m_ui.m_serValueLineEdit, SIGNAL(textChanged(const QString&)),
                   this, SLOT(serialisedValueUpdated(const QString&)));
    }

    assert(m_ui.m_nameLabel != nullptr);
    updateNameLabel(*m_ui.m_nameLabel);

    m_ui.m_valueComboBox->clear();
    m_ui.m_valueComboBox->addItem(InvalidValueComboText, QVariant(m_wrapper->maxValue()));
    m_ui.m_valueComboBox->insertSeparator(1);
    assert(m_ui.m_valueComboBox->count() == EnumValuesStartIndex);

    auto maxValidValue = m_wrapper->maxValidValue();
    for (auto value = m_wrapper->minValidValue(); value <= maxValidValue; ++value) {
        auto valueName = property(GlobalConstants::indexedNamePropertyName(value).toUtf8().data());
        if ((valueName.isValid()) &&
            (valueName.canConvert<QString>())) {
            m_ui.m_valueComboBox->addItem(valueName.value<QString>(), QVariant(value));
        }
    }

    connect(m_ui.m_valueComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(valueUpdated(int)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    m_signalsConnected = true;
}

}  // namespace comms_champion


