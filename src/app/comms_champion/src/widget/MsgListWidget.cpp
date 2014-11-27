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

#include "MsgListWidget.h"

#include <cassert>

#include <QtCore/QVariant>

#include "comms_champion/Message.h"

namespace comms_champion
{

MsgListWidget::MsgListWidget(
    const QString& listName,
    QWidget* toolbar,
    QWidget* parent)
{
    m_ui.setupUi(this);
    m_ui.m_groupBoxLayout->insertWidget(0, toolbar);
    m_ui.m_groupBox->setTitle(listName);

    connect(m_ui.m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(itemClicked(QListWidgetItem*)));
    connect(m_ui.m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(itemDoubleClicked(QListWidgetItem*)));
}

void MsgListWidget::addMessage(Message* msg)
{
    assert(msg != nullptr);
    m_ui.m_listWidget->addItem(msg->name());
    auto* item = m_ui.m_listWidget->item(m_ui.m_listWidget->count() - 1);
    item->setData(
        Qt::UserRole,
        QVariant::fromValue(msg));

    if (m_selectOnAdd) {
        item->setSelected(true);
        m_ui.m_listWidget->scrollToBottom();
    }
}

void MsgListWidget::selectOnAdd(bool enabled)
{
    m_selectOnAdd = enabled;
}

void MsgListWidget::clearSelection()
{
    m_ui.m_listWidget->clearSelection();
}

void MsgListWidget::msgClickedImpl(Message* msg)
{
    static_cast<void>(msg);
}

void MsgListWidget::msgDoubleClickedImpl(Message* msg)
{
    static_cast<void>(msg);
}

void MsgListWidget::itemClicked(QListWidgetItem* item)
{
    msgClickedImpl(getMsgFromItem(item));
}

void MsgListWidget::itemDoubleClicked(QListWidgetItem* item)
{
    msgDoubleClickedImpl(getMsgFromItem(item));
}

Message* MsgListWidget::getMsgFromItem(QListWidgetItem* item)
{
    auto var = item->data(Qt::UserRole);
    assert(var.canConvert<Message*>());
    return var.value<Message*>();
}

}  // namespace comms_champion


