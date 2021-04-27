//
// Copyright 2014 - 2021 (C). Alex Robenko. All rights reserved.
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

#include "BitmaskValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QCheckBox>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/field.h"

namespace comms_champion
{

BitmaskValueFieldWidget::BitmaskValueFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper)),
    m_checkboxes(m_wrapper->bitIdxLimit())
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));
}

BitmaskValueFieldWidget::~BitmaskValueFieldWidget() noexcept = default;

void BitmaskValueFieldWidget::refreshImpl()
{
    assert(m_wrapper->canWrite());
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

    auto bitIdxLimit = m_wrapper->bitIdxLimit();
    assert(bitIdxLimit == m_checkboxes.size());
    for (auto idx = 0U; idx < bitIdxLimit; ++idx) {
        auto* checkbox = m_checkboxes[idx];
        if (checkbox == nullptr) {
            continue;
        }

        bool showedBitValue = checkbox->checkState() != 0;
        bool actualBitValue = m_wrapper->bitValue(idx);
        if (showedBitValue != actualBitValue) {
            Qt::CheckState state = Qt::Unchecked;
            if (actualBitValue) {
                state = Qt::Checked;
            }
            checkbox->setCheckState(state);
        }
    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void BitmaskValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void BitmaskValueFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    for (auto* checkbox : m_checkboxes) {
        delete checkbox;
    }

    property::field::BitmaskValue bitmaskProps(props);

    auto& bitNamesList = bitmaskProps.bits();

    m_checkboxes.clear();

    auto count = std::min(static_cast<unsigned>(bitNamesList.size()), m_wrapper->bitIdxLimit());
    m_checkboxes.resize(m_wrapper->bitIdxLimit());

    for (unsigned idx = 0; idx < count; ++idx) {
        auto& nameVar = bitNamesList[static_cast<int>(idx)];
        if ((!nameVar.isValid()) || (!nameVar.canConvert<QString>())) {
            continue;
        }

        auto* checkbox = new QCheckBox(nameVar.toString());
        m_ui.m_checkboxesLayout->addWidget(checkbox);
        m_checkboxes[idx] = checkbox;

        connect(checkbox, SIGNAL(stateChanged(int)),
                this, SLOT(checkBoxUpdated(int)));
    }

    refresh();
}

void BitmaskValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
}

void BitmaskValueFieldWidget::checkBoxUpdated(int value)
{
    bool updated = false;
    if (isEditEnabled()) {
        auto* checkbox = sender();
        auto iter = std::find(m_checkboxes.begin(), m_checkboxes.end(), checkbox);
        if (iter == m_checkboxes.end()) {
            static constexpr bool Should_not_happen = false;
            static_cast<void>(Should_not_happen);
            assert(Should_not_happen);    
            return;
        }
        auto idx = static_cast<unsigned>(std::distance(m_checkboxes.begin(), iter));
        m_wrapper->setBitValue(idx, value != 0);
        updated = true;
        if (!m_wrapper->canWrite()) {
            m_wrapper->reset();
            assert(m_wrapper->canWrite());
        }
    }

    refresh();
    if (updated) {
        emitFieldUpdated();
    }
}

}  // namespace comms_champion


