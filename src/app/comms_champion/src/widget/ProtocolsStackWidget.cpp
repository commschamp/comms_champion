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

ProtocolsStackWidget::ProtocolsStackWidget(QWidget* parentObj)
  : Base(parentObj)
{
    m_ui.setupUi(this);

    connect(m_ui.m_protocolsTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(itemClicked(QTreeWidgetItem*, int)));
}

ProtocolsStackWidget::~ProtocolsStackWidget() = default;

void ProtocolsStackWidget::displayMessage(MessageInfoPtr msgInfo, bool force)
{
    bool selectionChanged = true;
    do {
        if (force) {
            break;
        }

        assert(m_ui.m_protocolsTreeWidget != nullptr);
        auto* topProtocolItem = m_ui.m_protocolsTreeWidget->topLevelItem(0);
        if (topProtocolItem == nullptr) {
            break;
        }

        auto firstChild = topProtocolItem->child(0);
        if (firstChild == nullptr) {
            break;
        }

        assert(m_ui.m_protocolsTreeWidget->currentItem() != nullptr);
        if (firstChild != m_ui.m_protocolsTreeWidget->currentItem()) {
            break;
        }

        auto storedAppMsg = msgFromItem(firstChild);
        if (storedAppMsg != msgInfo->getAppMessage()) {
            break;
        }

        selectionChanged = false;
    } while (false);

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
        if (selectionChanged) {
            reportMessageSelected(firstMsgItem);
        }
    }
}

void ProtocolsStackWidget::clear()
{
    m_ui.m_protocolsTreeWidget->clear();
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
    auto msgPtr = msgFromItem(item);
    bool editEnabled = (item == m_ui.m_protocolsTreeWidget->topLevelItem(0)->child(0));
    emit sigMessageSelected(msgPtr, editEnabled);
}

MessageInfo::MessagePtr ProtocolsStackWidget::msgFromItem(QTreeWidgetItem* item)
{
    auto msgPtrVar = item->data(0, Qt::UserRole);
    assert(msgPtrVar.isValid());
    assert(msgPtrVar.canConvert<MessageInfo::MessagePtr>());
    return msgPtrVar.value<MessageInfo::MessagePtr>();
}

}  // namespace comms_champion


