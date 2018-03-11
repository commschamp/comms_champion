//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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

#include "VariantFieldWidget.h"

#include <cassert>

#include "comms_champion/property/field.h"

namespace comms_champion
{

    VariantFieldWidget::VariantFieldWidget(
    WrapperPtr&& wrapper,
    CreateMemberFieldWidgetFunc&& func,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper)),
    m_createFunc(std::move(func))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);\

    m_ui.m_idxSpinBox->setMaximum(m_wrapper->getMembersCount() - 1);
    m_ui.m_idxSpinBox->setValue(m_wrapper->getCurrentIndex());

    connect(
        m_ui.m_idxSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(indexUpdated(int)));
}

VariantFieldWidget::~VariantFieldWidget() noexcept = default;

void VariantFieldWidget::setMemberField(FieldWidget* memberFieldWidget)
{
    assert(memberFieldWidget != nullptr);
    if (m_member != nullptr) {
        delete m_member;
    }
    m_member = memberFieldWidget;
    m_ui.m_membersLayout->addWidget(m_member);

    assert(m_ui.m_membersLayout->count() == 1);

    refreshInternal();

    connect(
        m_member, SIGNAL(sigFieldUpdated()),
        this, SLOT(memberFieldUpdated()));
}

void VariantFieldWidget::refreshImpl()
{
    refreshInternal();
    refreshMember();
}

void VariantFieldWidget::editEnabledUpdatedImpl()
{
    bool readOnly = !isEditEnabled();

    if (m_member != nullptr) {
        m_member->setEditEnabled(!readOnly);
    }

    updateIndexInfo();
}

void VariantFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    property::field::Variant variantProps(props);
    m_membersProps = variantProps.members();
    updateMemberProps();

    m_indexHidden = variantProps.isIndexHidden();
    updateIndexInfo();
}

void VariantFieldWidget::memberFieldUpdated()
{
    refreshInternal();
    emitFieldUpdated();
}

void VariantFieldWidget::indexUpdated(int value)
{
    if (value == m_wrapper->getCurrentIndex()) {
        return;
    }

    delete m_member;
    m_member = nullptr;
    m_wrapper->setCurrent(field_wrapper::FieldWrapperPtr());
    m_wrapper->setCurrentIndex(-1);

    if (0 <= value) {
        m_wrapper->setCurrentIndex(value);
        m_wrapper->updateCurrent();
        assert(m_wrapper->getCurrent());
        assert(m_createFunc);
        auto fieldWidget = m_createFunc(*m_wrapper->getCurrent());
        m_member = fieldWidget.release();
        m_ui.m_membersLayout->addWidget(m_member);
        updateMemberProps();

        connect(
            m_member, SIGNAL(sigFieldUpdated()),
            this, SLOT(memberFieldUpdated()));
    }

    refresh();
    emitFieldUpdated();
}

void VariantFieldWidget::refreshInternal()
{
    assert(m_ui.m_serValuePlainTextEdit != nullptr);
    updateSerValue(*m_ui.m_serValuePlainTextEdit, *m_wrapper);

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValuePlainTextEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void VariantFieldWidget::refreshMember()
{
    if (m_member != nullptr) {
        m_member->refresh();
    }
}

void VariantFieldWidget::updateMemberProps()
{
    if (m_member == nullptr) {
        return;
    }

    assert(0 <= m_ui.m_idxSpinBox->value());
    assert(0 <= m_wrapper->getCurrentIndex());
    if (m_membersProps.size() <= m_wrapper->getCurrentIndex()) {
        return;
    }

    m_member->updateProperties(m_membersProps[m_wrapper->getCurrentIndex()]);
}

void VariantFieldWidget::updateIndexInfo()
{
    bool readOnly = !isEditEnabled();
    m_ui.m_idxSpinBox->setReadOnly(readOnly);

    if (readOnly) {
        m_ui.m_idxSpinBox->setButtonSymbols(QSpinBox::NoButtons);
    }
    else {
        m_ui.m_idxSpinBox->setButtonSymbols(QSpinBox::UpDownArrows);
    }

    bool hidden = readOnly && m_indexHidden;
    m_ui.m_idxLabel->setHidden(hidden);
    m_ui.m_idxSpinBox->setHidden(hidden);

    bool idxWidgetHidden = hidden && (m_ui.m_nameLabel->isHidden());
    m_ui.m_idxWidget->setHidden(idxWidgetHidden);
}

}  // namespace comms_champion


