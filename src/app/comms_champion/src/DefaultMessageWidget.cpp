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

#include "comms_champion/DefaultMessageWidget.h"

#include <memory>

#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QFrame>

namespace comms_champion
{

DefaultMessageWidget::DefaultMessageWidget(QWidget* parent)
  : Base(parent),
    m_layout(new LayoutType())
{
    setLayout(m_layout);
    m_layout->addItem(
        new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

}

void DefaultMessageWidget::addFieldWidget(FieldWidget* field)
{
    std::unique_ptr<FieldWidget> fieldPtr(field);
    if (!m_layout->isEmpty()) {
        std::unique_ptr<QFrame> line(new QFrame(this));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        m_layout->insertWidget(m_layout->count() - 1, line.release());
    }
    m_layout->insertWidget(m_layout->count() - 1, fieldPtr.release());
    connect(this, SIGNAL(sigRefreshFields()), field, SLOT(refresh()));
    connect(this, SIGNAL(sigSetEditEnabled(bool)), field, SLOT(setEditEnabled(bool)));
    connect(field, SIGNAL(sigFieldUpdated), this, SIGNAL(sigMsgUpdated()));
}

void DefaultMessageWidget::refreshImpl()
{
    emit sigRefreshFields();
}

void DefaultMessageWidget::setEditEnabledImpl(bool enabled)
{
    emit sigSetEditEnabled(enabled);
}

}  // namespace comms_champion


