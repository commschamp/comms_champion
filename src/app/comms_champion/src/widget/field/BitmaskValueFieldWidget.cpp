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

#include "BitmaskValueFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

#include <QtWidgets/QCheckBox>

#include "GlobalConstants.h"

namespace comms_champion
{

BitmaskValueFieldWidget::BitmaskValueFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper)),
    m_checkboxes(m_wrapper->bitIdxLimit())
{
    m_ui.setupUi(this);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));
}

BitmaskValueFieldWidget::~BitmaskValueFieldWidget() = default;

void BitmaskValueFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->serialisedString());

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

void BitmaskValueFieldWidget::setEditEnabledImpl(bool enabled)
{
    bool readonly = !enabled;
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void BitmaskValueFieldWidget::propertiesUpdatedImpl()
{
    readPropertiesAndUpdateUi();
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
            assert(!"Should not happen");
            return;
        }
        auto idx = static_cast<unsigned>(std::distance(m_checkboxes.begin(), iter));
        m_wrapper->setBitValue(idx, value != 0);
        updated = true;
    }

    refresh();
    if (updated) {
        emitFieldUpdated();
    }
}

void BitmaskValueFieldWidget::readPropertiesAndUpdateUi()
{
    assert(m_ui.m_nameLabel != nullptr);
    updateNameLabel(*m_ui.m_nameLabel);
    for (auto& checkbox : m_checkboxes) {
        if (checkbox != nullptr) {
            m_ui.m_checkboxesLayout->removeWidget(checkbox);
            checkbox = nullptr;
        }
    }
    createCheckboxes();
}

void BitmaskValueFieldWidget::createCheckboxes()
{
    auto bitIdxLimit = m_wrapper->bitIdxLimit();
    assert(m_checkboxes.size() == bitIdxLimit);
    for (unsigned idx = 0; idx < bitIdxLimit; ++idx) {

        auto indexedName = property(GlobalConstants::indexedNamePropertyName(idx).toUtf8().data());
        if ((indexedName.isValid()) &&
            (indexedName.canConvert<QString>())) {
            auto* checkbox = new QCheckBox(indexedName.value<QString>());
            m_ui.m_checkboxesLayout->addWidget(checkbox);
            m_checkboxes[idx] = checkbox;

            connect(checkbox, SIGNAL(stateChanged(int)),
                    this, SLOT(checkBoxUpdated(int)));
        }
    }
}

}  // namespace comms_champion


