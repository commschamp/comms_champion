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

void BitfieldFieldWidget::addMemberField(FieldWidget* memberFieldWidget)
{
    m_members.push_back(memberFieldWidget);
    auto idx = m_members.size() - 1;
    updateMemberProperties(idx);

    m_ui.m_membersLayout->addWidget(memberFieldWidget);
    assert((std::size_t)m_ui.m_membersLayout->count() == m_members.size());

    refreshInternal();
}

void BitfieldFieldWidget::refreshImpl()
{
    refreshInternal();
    refreshMembers();
}

void BitfieldFieldWidget::setEditEnabledImpl(bool enabled)
{
    bool readonly = !enabled;
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
    for (auto* memberFieldWidget : m_members) {
        memberFieldWidget->setEditEnabled(enabled);
    }
}

void BitfieldFieldWidget::propertiesUpdatedImpl()
{
    for (auto idx = 0U; idx < m_members.size(); ++idx) {
        auto* memberFieldWidget = m_members[idx];
        updateMemberProperties(idx);
        memberFieldWidget->propertiesUpdated();
    }
}

void BitfieldFieldWidget::serialisedValueUpdated(const QString& value)
{
    handleNumericSerialisedValueUpdate(value, *m_wrapper);
    refreshMembers();
}

void BitfieldFieldWidget::memberFieldUpdated()
{
    refreshInternal();
    emitFieldUpdated();
}

void BitfieldFieldWidget::refreshInternal()
{
    assert(m_ui.m_serValueLineEdit != nullptr);
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->serialisedString());

    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValueLineEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void BitfieldFieldWidget::refreshMembers()
{
    for (auto* memberFieldWidget : m_members) {
        memberFieldWidget->refresh();
    }
}

void BitfieldFieldWidget::updateMemberProperties(std::size_t idx)
{
    assert(idx < m_members.size());
    FieldWidget* memberFieldWidget = m_members[idx];
    assert(memberFieldWidget != nullptr);
    auto propsVar = property(GlobalConstants::indexedMemberDataPropertyName(idx).toUtf8().data());
    if (propsVar.isValid() && propsVar.canConvert<QVariantMap>()) {
        auto map = propsVar.value<QVariantMap>();
        auto keys = map.keys();
        for (auto& k : keys) {
            memberFieldWidget->setProperty(k.toUtf8().data(), map[k]);
        }
    }
}

}  // namespace comms_champion


