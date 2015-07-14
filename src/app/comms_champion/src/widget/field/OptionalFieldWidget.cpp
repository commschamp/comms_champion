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

#include "OptionalFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

#include <QtWidgets/QCheckBox>

#include "comms_champion/Property.h"

namespace comms_champion
{

OptionalFieldWidget::OptionalFieldWidget(
    WrapperPtr wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);

    connect(m_ui.m_optCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(availabilityChanged(int)));
}

OptionalFieldWidget::~OptionalFieldWidget() = default;

void OptionalFieldWidget::setField(FieldWidget* fieldWidget)
{
    assert(m_field == nullptr);
    assert(fieldWidget != nullptr);
    m_field = fieldWidget;

    m_ui.m_layout->insertWidget(m_ui.m_layout->count() - 1, fieldWidget);

    refreshInternal();

    connect(
        fieldWidget, SIGNAL(sigFieldUpdated()),
        this, SLOT(fieldUpdated()));
}

void OptionalFieldWidget::refreshImpl()
{
    refreshInternal();
    refreshField();
}

void OptionalFieldWidget::editEnabledUpdatedImpl()
{
    assert(m_field != nullptr);
    m_field->setEditEnabled(isEditEnabled());
}

void OptionalFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    auto wrappedPropsVar = props.value(Property::data());
    if (wrappedPropsVar.isValid() && wrappedPropsVar.canConvert<QVariantMap>()) {
        m_field->updateProperties(wrappedPropsVar.value<QVariantMap>());
        refreshInternal();
    }
}

void OptionalFieldWidget::fieldUpdated()
{
    refreshInternal();
    emitFieldUpdated();
}

void OptionalFieldWidget::availabilityChanged(int state)
{
    Mode mode = Mode::Tentative;
    if (state == Qt::Unchecked) {
        mode = Mode::Missing;
    }
    else {
        mode = Mode::Exists;
    }

    if (mode == m_wrapper->getMode()) {
        return;
    }

    if (!isEditEnabled()) {
        refreshInternal();
        return;
    }

    m_wrapper->setMode(mode);
    refresh();
    emitFieldUpdated();
}

void OptionalFieldWidget::refreshInternal()
{
    assert(m_field);
    auto mode = m_wrapper->getMode();
    if (mode == Mode::Exists) {
        m_ui.m_optCheckBox->setCheckState(Qt::Checked);
        m_ui.m_nameLabel->hide();
        m_field->show();
    }
    else if (mode == Mode::Tentative) {
        m_ui.m_optCheckBox->setCheckState(Qt::Unchecked);
        m_ui.m_nameLabel->hide();
        m_field->show();
    }
    else {
        assert(mode == Mode::Missing);
        m_ui.m_optCheckBox->setCheckState(Qt::Unchecked);
        m_ui.m_nameLabel->show();
        m_field->hide();
    }
}

void OptionalFieldWidget::refreshField()
{
    assert(m_field != nullptr);
    m_field->refresh();
}


}  // namespace comms_champion


