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

#include "EnumValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <limits>

#include "comms_champion/Property.h"

namespace comms_champion
{

namespace
{

const QString InvalidValueComboText("???");
const int EnumValuesStartIndex = 2;

}  // namespace

EnumValueFieldWidget::EnumValueFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());
}

EnumValueFieldWidget::~EnumValueFieldWidget() = default;

void EnumValueFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

    bool valid = m_wrapper->valid();
    auto comboIdx = m_ui.m_valueComboBox->currentIndex();

    auto comboRetrieveValueFunc =
        [this](int idx) -> UnderlyingType
        {
            auto comboValueVar = m_ui.m_valueComboBox->itemData(idx);
            assert(comboValueVar.isValid());
            assert(comboValueVar.canConvert<UnderlyingType>());
            return comboValueVar.value<UnderlyingType>();
        };

    do {
        if ((!valid) && (comboIdx < m_idxOffset)) {
            break;
        }

        auto value = m_wrapper->getValue();
        auto comboValue = comboRetrieveValueFunc(m_ui.m_valueComboBox->currentIndex());
        if (value == comboValue) {
            break;
        }

        bool foundMatch = false;
        auto itemsCount = m_ui.m_valueComboBox->count();
        for (auto idx = m_idxOffset; idx < itemsCount; ++idx) {
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

void EnumValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void EnumValueFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    if (m_signalsConnected) {
        disconnect(m_ui.m_valueComboBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(valueUpdated(int)));

        disconnect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
                   this, SLOT(serialisedValueUpdated(const QString&)));
    }

    m_ui.m_valueComboBox->clear();

    auto dataListVar = Property::getData(props);
    auto maxValue = std::numeric_limits<long long int>::min();
    do {
        if ((!dataListVar.isValid()) || (!dataListVar.canConvert<QVariantList>())) {
            break;
        }

        auto dataList = dataListVar.value<QVariantList>();
        for (auto& elemPropsVar : dataList) {
            if ((!elemPropsVar.isValid()) || (!elemPropsVar.canConvert<QVariantMap>())) {
                continue;
            }

            auto elemProps = elemPropsVar.value<QVariantMap>();
            auto nameVar = Property::getName(elemProps);
            auto dataVar = Property::getData(elemProps);

            if ((!nameVar.isValid()) ||
                (!dataVar.isValid()) ||
                (!nameVar.canConvert<QString>()) ||
                (!dataVar.canConvert<long long int>())) {
                continue;
            }

            auto name = nameVar.toString();
            auto data = dataVar.value<long long int>();
            m_ui.m_valueComboBox->addItem(name, data);

            maxValue = std::max(maxValue, data);
        }

    } while (false);

    auto invValue = maxValue + 1;
    auto len = m_wrapper->length();
    auto shift = len * std::numeric_limits<std::uint8_t>::digits;
    auto maxAllowedValue =
        static_cast<UnderlyingType>((static_cast<long long unsigned>(1) << shift) - 1);

    do {
        if (invValue < 0) {
            invValue = maxAllowedValue;
            break;
        }

        if (invValue <= maxAllowedValue) {
            break;
        }

        std::vector<UnderlyingType> storedValues;
        for (auto idx = 0; idx < m_ui.m_valueComboBox->count(); ++idx) {
            storedValues.push_back(m_ui.m_valueComboBox->itemData(idx).value<UnderlyingType>());
        }

        invValue = maxAllowedValue - 1;
        while (0 < invValue) {
            if (std::none_of(
                storedValues.begin(), storedValues.end(),
                [invValue](UnderlyingType val) -> bool
                {
                    return val == invValue;
                })) {

                break;
            }

            --invValue;
        }

    } while (false);

    m_idxOffset = 0;
    if (0 <= invValue) {
        m_ui.m_valueComboBox->insertItem(0, InvalidValueComboText, QVariant(maxValue + 1));
        m_ui.m_valueComboBox->insertSeparator(1);
        m_idxOffset = EnumValuesStartIndex;
    }

    refresh();

    connect(m_ui.m_valueComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(valueUpdated(int)));

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    m_signalsConnected = true;
}

void EnumValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
}

void EnumValueFieldWidget::valueUpdated(int idx)
{
    if ((!m_wrapper->valid()) && (idx < m_idxOffset)) {
        return;
    }

    if (isEditEnabled()) {
        auto valueVar = m_ui.m_valueComboBox->currentData();
        assert(valueVar.isValid());
        assert(valueVar.canConvert<UnderlyingType>());
        auto value = valueVar.value<UnderlyingType>();
        if (value == m_wrapper->getValue()) {
            return;
        }

        assert(isEditEnabled());
        m_wrapper->setValue(value);
        emitFieldUpdated();
    }
    refresh();
}

}  // namespace comms_champion


