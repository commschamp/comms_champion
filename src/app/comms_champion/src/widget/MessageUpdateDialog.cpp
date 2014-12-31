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


#include "MessageUpdateDialog.h"

#include <cassert>

#include <QtWidgets/QVBoxLayout>

#include "DefaultMessageDisplayWidget.h"

namespace comms_champion
{

MessageUpdateDialog::MessageUpdateDialog(
    MessageInfoPtr& msgInfo,
    ProtocolPtr protocol,
    QWidget* parent)
  : Base(parent),
    m_msgInfo(msgInfo),
    m_protocol(std::move(protocol)),
    m_allMsgs(m_protocol->createAllMessages()),
    m_msgDisplayWidget(new DefaultMessageDisplayWidget())
{
    m_ui.setupUi(this);
    assert(m_ui.m_msgDetailsWidget);
    m_ui.m_msgDetailsWidget->setLayout(new QVBoxLayout());
    m_ui.m_msgDetailsWidget->layout()->addWidget(m_msgDisplayWidget);

    refreshDisplayedList();

    connect(m_ui.m_msgListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(itemClicked(QListWidgetItem*)));
}

void MessageUpdateDialog::itemClicked(QListWidgetItem* item)
{
    auto msgInfo = getMsgFromItem(item);
    assert(msgInfo);

    m_msgDisplayWidget->displayMessage(std::move(msgInfo));
}

void MessageUpdateDialog::refreshDisplayedList()
{
    // TODO: filter messages
    auto msgsToDisplay = m_allMsgs;
    for (auto& msgInfo : msgsToDisplay) {
        auto msgPtr = msgInfo->getAppMessage();
        auto* name = msgPtr->name();
        auto itemStr = QString("(%1) %2").arg(msgPtr->idAsString()).arg(msgPtr->name());
        m_ui.m_msgListWidget->addItem(itemStr);
        auto* item = m_ui.m_msgListWidget->item(m_ui.m_msgListWidget->count() - 1);
        item->setData(
            Qt::UserRole,
            QVariant::fromValue(msgInfo));
    }
}

MessageInfoPtr MessageUpdateDialog::getMsgFromItem(QListWidgetItem* item)
{
    auto var = item->data(Qt::UserRole);
    assert(var.canConvert<MessageInfoPtr>());
    return var.value<MessageInfoPtr>();
}

}  // namespace comms_champion

