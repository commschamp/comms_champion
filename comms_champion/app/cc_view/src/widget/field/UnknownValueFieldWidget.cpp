//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "UnknownValueFieldWidget.h"

#include <algorithm>
#include <cassert>

namespace comms_champion
{

UnknownValueFieldWidget::UnknownValueFieldWidget(
    field_wrapper::UnknownValueWrapperPtr&& wrapper,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);

    connect(m_ui.m_serValueLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    refresh();
}

UnknownValueFieldWidget::~UnknownValueFieldWidget() = default;

void UnknownValueFieldWidget::refreshImpl()
{
    auto curText = m_ui.m_serValueLineEdit->text();
    auto serString = m_wrapper->getSerialisedString();
    if (curText != serString) {

        assert(m_ui.m_serValueLineEdit != nullptr);
        setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());
        m_ui.m_serValueLineEdit->setText(serString);
    }

    setFieldValid(m_wrapper->valid());
}

void UnknownValueFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
}

void UnknownValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    assert(isEditEnabled());

    QString valueCopy(value);
    if ((valueCopy.size() & 0x1) != 0) {
        valueCopy.append(QChar('0'));
    }

    if (m_wrapper->setSerialisedString(valueCopy)) {
        refresh();
        emitFieldUpdated();
    }
    else {
        setFieldValid(false);
    }
}

void UnknownValueFieldWidget::setFieldValid(bool valid)
{
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

}  // namespace comms_champion


