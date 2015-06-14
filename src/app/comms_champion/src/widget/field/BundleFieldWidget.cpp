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

#include "BundleFieldWidget.h"

#include <algorithm>
#include <cassert>
#include <type_traits>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>

#include "comms_champion/Property.h"

namespace comms_champion
{

BundleFieldWidget::BundleFieldWidget(
    QWidget* parent)
  : Base(parent)
{
    m_membersLayout = new QVBoxLayout();
    setLayout(m_membersLayout);
}

BundleFieldWidget::~BundleFieldWidget() = default;

void BundleFieldWidget::addMemberField(FieldWidget* memberFieldWidget)
{
    m_members.push_back(memberFieldWidget);
    auto idx = m_members.size() - 1;
    updateMemberProperties(idx);

    if (m_membersLayout->count() != 0) {
        auto* line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        m_membersLayout->addWidget(line);
    }

    m_membersLayout->addWidget(memberFieldWidget);
    assert((std::size_t)m_membersLayout->count() == ((m_members.size() * 2) - 1));

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

void BundleFieldWidget::setEditEnabledImpl(bool enabled)
{
    for (auto* memberFieldWidget : m_members) {
        memberFieldWidget->setEditEnabled(enabled);
    }
}

void BundleFieldWidget::propertiesUpdatedImpl()
{
    for (auto idx = 0U; idx < m_members.size(); ++idx) {
        updateMemberProperties(idx);
    }
}

void BundleFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    for (auto idx = 0U; idx < m_members.size(); ++idx) {
        auto* memberFieldWidget = m_members[idx];
        assert(memberFieldWidget != nullptr);

        auto memberPropsVar = props.value(Property::indexedData(idx));
        if ((!memberPropsVar.isValid()) || (!memberPropsVar.canConvert<QVariantMap>())) {
            continue;
        }

        memberFieldWidget->updateProperties(memberPropsVar.value<QVariantMap>());
    }
}

void BundleFieldWidget::memberFieldUpdated()
{
    emitFieldUpdated();
}

void BundleFieldWidget::updateMemberProperties(std::size_t idx)
{
    assert(idx < m_members.size());
    FieldWidget* memberFieldWidget = m_members[idx];
    assert(memberFieldWidget != nullptr);
    auto propsVar = Property::getIndexedDataVal(*this, idx);
    do {
        if ((!propsVar.isValid()) || (!propsVar.canConvert<QVariantMap>())) {
            break;
        }

        auto map = propsVar.value<QVariantMap>();
        auto keys = map.keys();
        for (auto& k : keys) {
            memberFieldWidget->setProperty(k.toUtf8().data(), map[k]);
        }
    } while (false);

    memberFieldWidget->propertiesUpdated();
}

}  // namespace comms_champion


