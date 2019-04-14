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

#include "StringFieldWidget.h"

#include <algorithm>
#include <cassert>

namespace comms_champion
{

StringFieldWidget::StringFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);

    connect(
        m_ui.m_valuePlainTextEdit, SIGNAL(textChanged()),
        this, SLOT(stringChanged()));


    refresh();
}

StringFieldWidget::~StringFieldWidget() noexcept = default;

void StringFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValuePlainTextEdit != nullptr);
    updateSerValue(*m_ui.m_serValuePlainTextEdit, *m_wrapper);

    assert(m_ui.m_valuePlainTextEdit != nullptr);
    auto value = m_wrapper->getValue();
    if (m_ui.m_valuePlainTextEdit->toPlainText() != value) {
        auto curs = m_ui.m_valuePlainTextEdit->textCursor();
        auto newPosition = std::min(curs.position(), value.size());
        m_ui.m_valuePlainTextEdit->setPlainText(value);
        curs.setPosition(newPosition);
        m_ui.m_valuePlainTextEdit->setTextCursor(curs);
    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_valuePlainTextEdit, valid);
    setValidityStyleSheet(*m_ui.m_serValuePlainTextEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void StringFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_valuePlainTextEdit->setReadOnly(readonly);
}

void StringFieldWidget::stringChanged()
{
    auto str = m_ui.m_valuePlainTextEdit->toPlainText();
    if (m_wrapper->maxSize() < str.size()) {
        str.resize(m_wrapper->maxSize());
    }

    m_wrapper->setValue(str);
    refresh();
    emitFieldUpdated();
}

}  // namespace comms_champion


