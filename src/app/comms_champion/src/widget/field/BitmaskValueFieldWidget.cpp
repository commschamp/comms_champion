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

#include "GlobalConstants.h"

namespace comms_champion
{

BitmaskValueFieldWidget::BitmaskValueFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());

    connect(m_ui.m_serValueLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));

    refresh();
    readPropertiesAndUpdateUi();
}

BitmaskValueFieldWidget::~BitmaskValueFieldWidget() = default;

void BitmaskValueFieldWidget::refreshImpl()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateNumericSerialisedValue(
        *m_ui.m_serValueLineEdit,
        m_wrapper->serialisedValue(),
        m_wrapper->width());

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
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
}

void BitmaskValueFieldWidget::serialisedValueUpdated(const QString& value)
{
    static_assert(std::is_same<unsigned long long, UnderlyingType>::value,
        "Underlying type assumption is wrong");

    bool ok = false;
    UnderlyingType serValue = value.toULongLong(&ok, 16);
    assert(ok);
    static_cast<void>(ok);
    if (serValue == m_wrapper->serialisedValue()) {
        return;
    }
    m_wrapper->setSerialisedValue(serValue);
    emitFieldUpdated();
    refresh();
}

void BitmaskValueFieldWidget::readPropertiesAndUpdateUi()
{
    assert(m_ui.m_nameLabel != nullptr);
    updateNameLabel(*m_ui.m_nameLabel);
}

}  // namespace comms_champion


