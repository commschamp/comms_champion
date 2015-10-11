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
#include <QtWidgets/QFrame>

#include "comms_champion/Property.h"

namespace comms_champion
{

BitfieldFieldWidget::BitfieldFieldWidget(
    WrapperPtr&& wrapper,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper))
{
    m_ui.setupUi(this);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);

    assert(m_ui.m_serValueLineEdit != nullptr);
    setSerialisedInputMask(*m_ui.m_serValueLineEdit, m_wrapper->width());

    connect(m_ui.m_serValueLineEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(serialisedValueUpdated(const QString&)));
}

BitfieldFieldWidget::~BitfieldFieldWidget() = default;

void BitfieldFieldWidget::addMemberField(FieldWidget* memberFieldWidget)
{
    m_members.push_back(memberFieldWidget);

    if (m_ui.m_membersLayout->count() != 0) {
        auto* line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        m_ui.m_membersLayout->addWidget(line);
    }

    m_ui.m_membersLayout->addWidget(memberFieldWidget);
    assert((std::size_t)m_ui.m_membersLayout->count() == ((m_members.size() * 2) - 1));

    refreshInternal();

    connect(
        memberFieldWidget, SIGNAL(sigFieldUpdated()),
        this, SLOT(memberFieldUpdated()));
}

void BitfieldFieldWidget::refreshImpl()
{
    refreshInternal();
    refreshMembers();
}

void BitfieldFieldWidget::editEnabledUpdatedImpl()
{
    bool readonly = !isEditEnabled();
    m_ui.m_serValueLineEdit->setReadOnly(readonly);
    for (auto* memberFieldWidget : m_members) {
        memberFieldWidget->setEditEnabled(!readonly);
    }
}

void BitfieldFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    auto dataListVar = Property::getData(props);
    if ((!dataListVar.isValid()) || (!dataListVar.canConvert<QVariantList>())) {
        return;
    }

    auto dataList = dataListVar.value<QVariantList>();
    auto count = std::min((std::size_t)dataList.size(), m_members.size());
    for (auto idx = 0U; idx < count; ++idx) {
        auto& memberPropsVar = dataList[idx];
        if ((!memberPropsVar.isValid()) || (!memberPropsVar.canConvert<QVariantMap>())) {
            continue;
        }

        auto memberProps = memberPropsVar.value<QVariantMap>();

        auto* memberFieldWidget = m_members[idx];
        assert(memberFieldWidget != nullptr);
        memberFieldWidget->updateProperties(memberProps);
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
    updateValue(*m_ui.m_serValueLineEdit, m_wrapper->getSerialisedString());

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

}  // namespace comms_champion


