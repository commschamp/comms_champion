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

#include "ProtocolsStackWidget.h"

#include <cassert>

namespace comms_champion
{

ProtocolsStackWidget::ProtocolsStackWidget(QWidget* parent)
  : Base(parent)
{
    m_ui.setupUi(this);

    connect(m_ui.m_protocolsTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(itemClicked(QTreeWidgetItem*, int)));
}

ProtocolsStackWidget::~ProtocolsStackWidget() = default;

void ProtocolsStackWidget::displayMessage(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    m_ui.m_protocolsTreeWidget->clear();
    QStringList colValues(QString(msgInfo->getProtocolName().c_str()));
    auto* topLevelItem = new QTreeWidgetItem(colValues);

    auto addMsgFunc =
        [&msgInfo, topLevelItem](MessageInfo::MessagePtr msg, const char* name)
        {
            if (msg) {
                QString nameStr(name);
                QStringList msgColValues(nameStr);
                auto* msgItem = new QTreeWidgetItem(msgColValues);
                msgItem->setData(0, Qt::UserRole, QVariant::fromValue(msg));
                topLevelItem->addChild(msgItem);
            }
        };

    addMsgFunc(msgInfo->getAppMessage(), "Application");
    addMsgFunc(msgInfo->getTransportMessage(), "Transport");
    addMsgFunc(msgInfo->getRawDataMessage(), "Raw Data");

    m_ui.m_protocolsTreeWidget->addTopLevelItem(topLevelItem);

    auto* topProtocolItem = m_ui.m_protocolsTreeWidget->topLevelItem(0);
    assert(topProtocolItem != nullptr);
    auto* firstMsgItem = topProtocolItem->child(0);
    if (firstMsgItem != 0) {
        m_ui.m_protocolsTreeWidget->setCurrentItem(firstMsgItem);
        reportMessageSelected(firstMsgItem);
    }
}

void ProtocolsStackWidget::itemClicked(QTreeWidgetItem* item, int column)
{
    static_cast<void>(column);

    auto msgPtrVar = item->data(0, Qt::UserRole);
    if (!msgPtrVar.isValid()) {
        // Top level item
        item = item->child(0);
        assert(item != nullptr);
        msgPtrVar = item->data(0, Qt::UserRole);
        m_ui.m_protocolsTreeWidget->setCurrentItem(item);
    }

    assert(msgPtrVar.isValid());
    reportMessageSelected(item);
}

void ProtocolsStackWidget::reportMessageSelected(QTreeWidgetItem* item)
{
    auto msgPtrVar = item->data(0, Qt::UserRole);
    assert(msgPtrVar.isValid());
    assert(msgPtrVar.canConvert<MessageInfo::MessagePtr>());
    auto msgPtr = msgPtrVar.value<MessageInfo::MessagePtr>();
    emit sigMessageSelected(msgPtr);
}

}  // namespace comms_champion


