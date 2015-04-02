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

#include "StringFieldWidget.h"

#include <algorithm>
#include <cassert>

#include "GlobalConstants.h"

namespace comms_champion
{

StringFieldWidget::StringFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);


    connect(
        m_ui.m_valueLineEdit, SIGNAL(textEdited(const QString&)),
        this, SLOT(stringChanged(const QString&)));


    refresh();
    readPropertiesAndUpdateUi();
}

StringFieldWidget::~StringFieldWidget() = default;

void StringFieldWidget::refreshImpl()
{
    QString serValueStr;
    auto serValue = m_wrapper->serialisedValue();

    for (auto byte : serValue) {
        if (!serValueStr.isEmpty()) {
            serValueStr.append(QChar(' '));
        }
        serValueStr.append(QString("%1").arg(byte, 2, 16, QChar('0')));
    }

    assert(m_ui.m_serValueLineEdit != nullptr);
    m_ui.m_serValueLineEdit->setText(serValueStr);

    assert(m_ui.m_valueLineEdit != nullptr);
    auto value = m_wrapper->value();
    if (m_ui.m_valueLineEdit->text() != value) {
        m_ui.m_valueLineEdit->setText(value);
    }

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void StringFieldWidget::setEditEnabledImpl(bool enabled)
{
    bool readonly = !enabled;
    m_ui.m_valueLineEdit->setReadOnly(readonly);
}

void StringFieldWidget::propertiesUpdatedImpl()
{
    readPropertiesAndUpdateUi();
}

void StringFieldWidget::stringChanged(const QString& str)
{
    if (str.size() < m_wrapper->maxSize()) {
        QString strCpy(str);
        strCpy.resize(m_wrapper->maxSize());
        updateString(strCpy);
        return;
    }

    updateString(str);
}

void StringFieldWidget::readPropertiesAndUpdateUi()
{
    assert(m_ui.m_nameLabel != nullptr);
    updateNameLabel(*m_ui.m_nameLabel);
}

void StringFieldWidget::updateString(const QString& str)
{
    m_wrapper->setValue(str);
    refresh();
    emitFieldUpdated();
}

}  // namespace comms_champion


