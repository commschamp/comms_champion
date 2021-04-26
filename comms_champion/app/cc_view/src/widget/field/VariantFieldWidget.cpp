//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
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

namespace
{

const QString InvalidMemberComboText("???");
const int MemberNamesStartIndex = 2;

} // namespace

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
    updateIndexValue();
    updateMemberCombo();

    connect(
        m_ui.m_idxSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(indexUpdated(int)));

    connect(
        m_ui.m_memberComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(memberComboUpdated(int)));        
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

    updateIndexDisplay();
}

void VariantFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    property::field::Variant variantProps(props);
    m_membersProps = variantProps.members();
    updateMemberProps();

    m_indexHidden = variantProps.isIndexHidden();

    using MemberInfosList = std::vector<std::pair<QString, int> >;
    MemberInfosList membersInfo;
    membersInfo.reserve(static_cast<unsigned>(m_membersProps.size()));
    for (int idx = 0; idx < m_membersProps.size(); ++idx) {
        property::field::Common commonProps(m_membersProps[idx]);
        auto& memName = commonProps.name();
        if (memName.isEmpty()) {
            continue;
        }

        membersInfo.push_back(std::make_pair(memName, idx));
    } 

    std::sort(
        membersInfo.begin(), membersInfo.end(),
        [](MemberInfosList::const_reference elem1, MemberInfosList::const_reference elem2)
        {
            return elem1.first < elem2.first;
        });

    m_ui.m_memberComboBox->blockSignals(true);
    m_ui.m_memberComboBox->clear(); 
    m_ui.m_memberComboBox->addItem(InvalidMemberComboText, QVariant(-1));
    m_ui.m_memberComboBox->insertSeparator(1);

    for (auto& i : membersInfo) {
        m_ui.m_memberComboBox->addItem(i.first, QVariant(i.second));
    }
    m_ui.m_memberComboBox->blockSignals(false);
       
    updateIndexDisplay();
    updateMemberCombo();
}

void VariantFieldWidget::memberFieldUpdated()
{
    if (!m_wrapper->canWrite()) {
        m_wrapper->reset();
        assert(m_wrapper->canWrite());
    }

    refreshInternal();
    emitFieldUpdated();
}

void VariantFieldWidget::indexUpdated(int value)
{
    assert(isEditEnabled());
    if (value == m_wrapper->getCurrentIndex()) {
        return;
    }

    destroyMemberWidget();

    if (0 <= value) {
        m_wrapper->setCurrentIndex(value);
        m_wrapper->updateCurrent();
        createMemberWidget();
    }

    updateMemberCombo();
    refresh();
    emitFieldUpdated();
}

void VariantFieldWidget::memberComboUpdated(int value)
{
    if (!isEditEnabled()) {
        updateMemberCombo();
        return;
    }

    if ((value < MemberNamesStartIndex) && (m_wrapper->getCurrentIndex() < 0)) {
        return;
    }

    destroyMemberWidget();

    if (MemberNamesStartIndex <= value) {
        auto dataVar = m_ui.m_memberComboBox->itemData(value, Qt::UserRole);
        assert(dataVar.isValid() && dataVar.canConvert<int>());
        int memIdx = dataVar.value<int>();
        m_wrapper->setCurrentIndex(memIdx);
        m_wrapper->updateCurrent();
        createMemberWidget();
    }

    updateIndexValue();
    refresh();
    emitFieldUpdated();
}

void VariantFieldWidget::refreshInternal()
{
    assert(m_wrapper->canWrite());
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

    if (m_membersProps.size() <= m_wrapper->getCurrentIndex()) {
        return;
    }

    m_member->updateProperties(m_membersProps[m_wrapper->getCurrentIndex()]);
}

void VariantFieldWidget::updateIndexDisplay()
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
    m_ui.m_idxWidget->setHidden(hidden);

    bool idxWidgetHidden = hidden && (m_ui.m_nameLabel->isHidden());
    m_ui.m_infoWidget->setHidden(idxWidgetHidden);
}

void VariantFieldWidget::updateIndexValue()
{
    auto memIdx = m_wrapper->getCurrentIndex();
    m_ui.m_idxSpinBox->blockSignals(true);
    m_ui.m_idxSpinBox->setValue(memIdx);
    m_ui.m_idxSpinBox->blockSignals(false);
}

void VariantFieldWidget::updateMemberCombo()
{
    auto memIdx = m_wrapper->getCurrentIndex();
    m_ui.m_memberComboBox->blockSignals(true);
    bool foundValid = false;
    for (int comboIdx = MemberNamesStartIndex; comboIdx < m_ui.m_memberComboBox->count(); ++comboIdx) {
        auto dataVar = m_ui.m_memberComboBox->itemData(comboIdx, Qt::UserRole);
        if ((!dataVar.isValid()) || (!dataVar.canConvert<int>())) {
            continue;
        }

        auto storedIdx = dataVar.value<int>();
        if (storedIdx == memIdx) {
            m_ui.m_memberComboBox->setCurrentIndex(comboIdx);
            foundValid = true;
            break;
        }
    }

    if (!foundValid) {
        m_ui.m_memberComboBox->setCurrentIndex(0); // Set unknown
    }

    m_ui.m_memberComboBox->blockSignals(false);    
}

void VariantFieldWidget::destroyMemberWidget()
{
    delete m_member;
    m_member = nullptr;
    m_wrapper->setCurrent(field_wrapper::FieldWrapperPtr());
    m_wrapper->setCurrentIndex(-1);
}

void VariantFieldWidget::createMemberWidget()
{
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

}  // namespace comms_champion


