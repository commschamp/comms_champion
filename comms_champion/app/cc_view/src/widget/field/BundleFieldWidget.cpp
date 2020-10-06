//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "BundleFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/field.h"

namespace comms_champion
{

BundleFieldWidget::BundleFieldWidget(
    WrapperPtr wrapper,
    QWidget* parentObj)
  : Base(parentObj),
    m_wrapper(std::move(wrapper)),
    m_membersLayout(new QVBoxLayout),
    m_label(new QLabel)
{
    m_label->hide();
    m_membersLayout->addWidget(m_label);
    setLayout(m_membersLayout);
    setNameLabelWidget(m_label);
}

BundleFieldWidget::~BundleFieldWidget() noexcept = default;

void BundleFieldWidget::addMemberField(FieldWidget* memberFieldWidget)
{
    m_members.push_back(memberFieldWidget);

    if (1 < m_membersLayout->count()) {
        auto* line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        m_membersLayout->addWidget(line);
    }

    m_membersLayout->addWidget(memberFieldWidget);
    assert(static_cast<std::size_t>(m_membersLayout->count()) == (m_members.size() * 2));

    connect(
        memberFieldWidget, SIGNAL(sigFieldUpdated()),
        this, SLOT(memberFieldUpdated()));
}

void BundleFieldWidget::refreshImpl()
{
    for (auto* memberFieldWidget : m_members) {
        memberFieldWidget->refresh();
    }
}

void BundleFieldWidget::editEnabledUpdatedImpl()
{
    bool enabled = isEditEnabled();
    for (auto* memberFieldWidget : m_members) {
        memberFieldWidget->setEditEnabled(enabled);
    }
}

void BundleFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    property::field::Bundle bundleProps(props);
    auto& membersProps = bundleProps.members();
    auto count = std::min(static_cast<std::size_t>(membersProps.size()), m_members.size());
    for (auto idx = 0U; idx < count; ++idx) {
        auto* memberFieldWidget = m_members[idx];
        assert(memberFieldWidget != nullptr);
        memberFieldWidget->updateProperties(membersProps[static_cast<int>(idx)]);
    }
}

void BundleFieldWidget::memberFieldUpdated()
{
    auto senderIter = std::find(m_members.begin(), m_members.end(), qobject_cast<FieldWidget*>(sender()));
    assert(senderIter != m_members.end());
    auto idx = static_cast<unsigned>(std::distance(m_members.begin(), senderIter));
    auto& memWrappers = m_wrapper->getMembers();
    assert(idx < memWrappers.size());
    auto& memWrapPtr = memWrappers[idx];
    if (!memWrapPtr->canWrite()) {
        memWrapPtr->reset();
        assert(memWrapPtr->canWrite());
        (*senderIter)->refresh();
    }

    emitFieldUpdated();
}

}  // namespace comms_champion


