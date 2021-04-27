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

#include "OptionalFieldWidget.h"

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

OptionalFieldWidget::OptionalFieldWidget(
    WrapperPtr wrapper,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);

    if (m_wrapper->getMode() == Mode::Tentative) {
        m_wrapper->setMode(Mode::Missing);
    }

    connect(m_ui.m_optCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(availabilityChanged(int)));
}

OptionalFieldWidget::~OptionalFieldWidget() noexcept = default;

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
    property::field::Optional optProps(props);
    auto& fieldPropsMap = optProps.field();
    assert(m_field);
    m_field->updateProperties(fieldPropsMap);
    refreshInternal();

    bool uncheckable = optProps.isUncheckable();
    m_ui.m_optCheckBox->setHidden(uncheckable);
    m_ui.m_optSep->setHidden(uncheckable);
}

void OptionalFieldWidget::fieldUpdated()
{
    if (!m_wrapper->canWrite()) {
        m_wrapper->reset();
        assert(m_wrapper->canWrite());
        refreshField();
    }
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
    assert(m_wrapper->canWrite());
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


