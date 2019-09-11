//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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

#include "SpecialValueWidget.h"

#include <cassert>
#include <cmath>

namespace comms_champion
{

template <typename T>
void SpecialValueWidget::commonSetup(const T& infos)
{
    m_ui.setupUi(this);
    for (auto& i : infos) {
        m_ui.m_specialComboBox->addItem(i.first, i.second);
    }

    connect(
        m_ui.m_specialComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(itemSelected(int)));
}

SpecialValueWidget::SpecialValueWidget(
    const IntValueInfosList& infos,
    QWidget* parentObj)
  : Base(parentObj)
{
    commonSetup(infos);
}

SpecialValueWidget::SpecialValueWidget(
    const FpValueInfosList& infos,
    QWidget* parentObj)
  : Base(parentObj),
    m_fpSpecials(true)
{
    commonSetup(infos);
}

SpecialValueWidget::~SpecialValueWidget() noexcept = default;

void SpecialValueWidget::setIntValue(long long value)
{
    assert(!m_fpSpecials);
    m_ui.m_specialComboBox->blockSignals(true);
    bool foundValue = false;
    for (auto idx = 0; idx < m_ui.m_specialComboBox->count(); ++idx) {
        auto valueVar = m_ui.m_specialComboBox->itemData(idx);
        if (!valueVar.isValid()) {
            continue;
        }

        assert(valueVar.canConvert<long long>());
        auto storedValue = valueVar.value<long long>();
        if (storedValue != value) {
            continue;
        }


        m_ui.m_specialComboBox->setCurrentIndex(idx);
        foundValue = true;
        break;
    }

    if (!foundValue) {
        m_ui.m_specialComboBox->setCurrentIndex(0);
    }

    m_ui.m_specialComboBox->blockSignals(false);
}

void SpecialValueWidget::setFpValue(double value, double cmpEpsilon)
{
    assert(m_fpSpecials);
    m_ui.m_specialComboBox->blockSignals(true);
    bool foundValue = false;
    for (auto idx = 0; idx < m_ui.m_specialComboBox->count(); ++idx) {
        auto valueVar = m_ui.m_specialComboBox->itemData(idx);
        if (!valueVar.isValid()) {
            continue;
        }


        assert(valueVar.canConvert<double>());
        auto storedValue = valueVar.value<double>();

        bool valueIsNan = std::isnan(value);
        bool storedValueIsNan = std::isnan(storedValue);

        if (valueIsNan && storedValueIsNan) {
            m_ui.m_specialComboBox->setCurrentIndex(idx);
            foundValue = true;
            break;
        }

        if (valueIsNan != storedValueIsNan) {
            continue;
        }

        bool valueIsInf = std::isinf(value);
        bool storedValueIsInf = std::isinf(storedValue);
        if (valueIsInf != storedValueIsInf) {
            continue;
        }

        if (valueIsInf) {
            bool valueIsPositive = (value > 0.0);
            bool storedValueIsPositive = (storedValue > 0.0);
            if (valueIsPositive != storedValueIsPositive) {
                continue;
            }

            m_ui.m_specialComboBox->setCurrentIndex(idx);
            foundValue = true;
            break;
        }

        if (cmpEpsilon < std::abs(storedValue - value)) {
            continue;
        }


        m_ui.m_specialComboBox->setCurrentIndex(idx);
        foundValue = true;
        break;
    }

    if (!foundValue) {
        m_ui.m_specialComboBox->setCurrentIndex(0);
    }

    m_ui.m_specialComboBox->blockSignals(false);
}


void SpecialValueWidget::itemSelected(int idx)
{
    static_cast<void>(idx);
    auto valueVar = m_ui.m_specialComboBox->currentData();
    if (!valueVar.isValid()) {
        emit sigRefreshReq();
        return;
    }

    if (m_fpSpecials) {
        assert(valueVar.canConvert<double>());
        auto value = valueVar.value<double>();
        emit sigFpValueChanged(value);
        return;
    }

    assert(valueVar.canConvert<long long>());
    auto value = valueVar.value<long long>();
    emit sigIntValueChanged(value);
}

}  // namespace comms_champion


