//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "VariantFieldWidget.h"

#include <cassert>

//#include "comms/CompileControl.h"

//CC_DISABLE_WARNINGS()
//#include <QtWidgets/QCheckBox>
//#include <QtWidgets/QFrame>
//CC_ENABLE_WARNINGS()

#include "comms_champion/property/field.h"

namespace comms_champion
{

VariantFieldWidget::VariantFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);\

    m_ui.m_idxSpinBox->setMaximum(m_wrapper->getMembersCount());
    m_ui.m_idxSpinBox->setValue(m_wrapper->getCurrentIndex());

    // TODO: connect spin box
}

VariantFieldWidget::~VariantFieldWidget() = default;

void VariantFieldWidget::setMemberField(FieldWidget* memberFieldWidget)
{
    GASSERT(memberFieldWidget != nullptr);
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
    m_ui.m_idxSpinBox->setReadOnly(readOnly);

    if (readOnly) {
        m_ui.m_idxSpinBox->setButtonSymbols(QSpinBox::NoButtons);
    }
    else {
        m_ui.m_idxSpinBox->setButtonSymbols(QSpinBox::UpDownArrows);
    }

    if (m_member != nullptr) {
        m_member->setEditEnabled(!readOnly);
    }
}

void VariantFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    property::field::Variant variantProps(props);
    if (variantProps.isIndexHidden()) {
        m_ui.m_idxLabel->hide();
        m_ui.m_idxSpinBox->hide();
    }
    m_membersProps = variantProps.members();
    updateMemberProps();
}

void VariantFieldWidget::memberFieldUpdated()
{
    refreshInternal();
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

}  // namespace comms_champion


