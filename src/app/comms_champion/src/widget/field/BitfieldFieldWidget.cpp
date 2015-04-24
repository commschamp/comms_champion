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

#include "BitfieldFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

#include <QtWidgets/QCheckBox>

#include "GlobalConstants.h"

namespace comms_champion
{

BitfieldFieldWidget::BitfieldFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));
}

BitfieldFieldWidget::~BitfieldFieldWidget() = default;

void BitfieldFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->serialisedString());

//    auto bitIdxLimit = m_wrapper->bitIdxLimit();
//    assert(bitIdxLimit == m_checkboxes.size());
//    for (auto idx = 0U; idx < bitIdxLimit; ++idx) {
//        auto* checkbox = m_checkboxes[idx];
//        if (checkbox == nullptr) {
//            continue;
//        }
//
//        bool showedBitValue = checkbox->checkState() != 0;
//        bool actualBitValue = m_wrapper->bitValue(idx);
//        if (showedBitValue != actualBitValue) {
//            Qt::CheckState state = Qt::Unchecked;
//            if (actualBitValue) {
//                state = Qt::Checked;
//            }
//            checkbox->setCheckState(state);
//        }
//    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void BitfieldFieldWidget::setEditEnabledImpl(bool enabled)
{
    bool readonly = !enabled;
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void BitfieldFieldWidget::propertiesUpdatedImpl()
{
    readPropertiesAndUpdateUi();
    refresh();
}

void BitfieldFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
}

void BitfieldFieldWidget::readPropertiesAndUpdateUi()
{

    //createCheckboxes();
}

}  // namespace comms_champion


