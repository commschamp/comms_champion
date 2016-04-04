//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include "DefaultMessageWidget.h"

#include <memory>
#include <cassert>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QFrame>
#include <QtCore/QVariant>
CC_ENABLE_WARNINGS()

#include "GlobalConstants.h"

namespace comms_champion
{

DefaultMessageWidget::DefaultMessageWidget(
    Message& msg,
    QWidget* parentObj)
  : Base(parentObj),
    m_msg(msg),
    m_layout(new LayoutType())
{
    setLayout(m_layout);
    m_layout->addItem(
        new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

}

void DefaultMessageWidget::addFieldWidget(FieldWidget* field)
{
    if (field == nullptr) {
        assert(!"Field object should be provided");
        return;
    }

    auto& props = m_msg.fieldsProperties();
    if (m_curFieldIdx < static_cast<decltype(m_curFieldIdx)>(props.size())) {
        auto& propsMapVar = props.at(m_curFieldIdx);
        if (propsMapVar.isValid() && propsMapVar.canConvert<QVariantMap>()) {
            auto propsMap = propsMapVar.value<QVariantMap>();
            field->updateProperties(propsMap);
        }
    }

    if (m_curFieldIdx != 0) {
        m_layout->insertWidget(m_layout->count() - 1, createFieldSeparator().release());
    }
    m_layout->insertWidget(m_layout->count() - 1, field);
    connectFieldSignals(field);

    ++m_curFieldIdx;
}

void DefaultMessageWidget::insertFieldWidget(int idx, FieldWidget* field)
{
    if (field == nullptr) {
        assert(!"Field object should be provided");
        return;
    }

    if (idx < 0) {
        assert(!"Unexpected field index");
        return;
    }

    auto adjustedIdx = std::min(idx * 2, m_layout->count() - 1);

    m_layout->insertWidget(adjustedIdx, field);
    connectFieldSignals(field);

    if (m_layout->count() <= 2) {
        return;
    }

    auto sep = createFieldSeparator();
    if ((m_layout->count() - 2) <= adjustedIdx) {

        m_layout->insertWidget(adjustedIdx, sep.release());
        return;
    }

    m_layout->insertWidget(adjustedIdx + 1, sep.release());
}

void DefaultMessageWidget::refreshImpl()
{
    emit sigRefreshFields();
}

void DefaultMessageWidget::setEditEnabledImpl(bool enabled)
{
    emit sigSetEditEnabled(enabled);
}

std::unique_ptr<QFrame> DefaultMessageWidget::createFieldSeparator()
{
    std::unique_ptr<QFrame> line(new QFrame(this));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return std::move(line);
}

void DefaultMessageWidget::connectFieldSignals(FieldWidget* field)
{
    assert(field != nullptr);
    connect(this, SIGNAL(sigRefreshFields()), field, SLOT(refresh()));
    connect(this, SIGNAL(sigSetEditEnabled(bool)), field, SLOT(setEditEnabled(bool)));
    connect(field, SIGNAL(sigFieldUpdated()), this, SIGNAL(sigMsgUpdated()));
}

}  // namespace comms_champion


