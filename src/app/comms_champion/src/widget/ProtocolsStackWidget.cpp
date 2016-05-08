//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "ProtocolsStackWidget.h"

#include <cassert>

#include "comms_champion/property/message.h"

namespace comms_champion
{

ProtocolsStackWidget::ProtocolsStackWidget(QWidget* parentObj)
  : Base(parentObj)
{
    m_ui.setupUi(this);

    connect(m_ui.m_protocolsTreeWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(newItemSelected()));
}

ProtocolsStackWidget::~ProtocolsStackWidget() = default;

void ProtocolsStackWidget::displayMessage(MessagePtr msg, bool force)
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
        if (storedAppMsg != msg) {
            break;
        }

        selectionChanged = false;

        auto* secondChild = topProtocolItem->child(1);
        if (secondChild == nullptr) {
            assert(!"Should not happen");
            break;
        }

        secondChild->setData(
            0, Qt::UserRole,
            QVariant::fromValue(property::message::TransportMsg().getFrom(*msg)));

        auto* thirdChild = topProtocolItem->child(2);
        if (thirdChild == nullptr) {
            assert(!"Should not happen");
            break;
        }

        thirdChild->setData(
            0, Qt::UserRole,
            QVariant::fromValue(property::message::RawDataMsg().getFrom(*msg)));

        return;
    } while (false);

    assert(msg);
    m_ui.m_protocolsTreeWidget->clear();
    QStringList colValues(property::message::ProtocolName().getFrom(*msg));
    auto* topLevelItem = new QTreeWidgetItem(colValues);

    auto addMsgFunc =
        [topLevelItem](MessagePtr msgParam, const char* name)
        {
            if (msgParam) {
                QString nameStr(name);
                QStringList msgColValues(nameStr);
                auto* msgItem = new QTreeWidgetItem(msgColValues);
                msgItem->setData(0, Qt::UserRole, QVariant::fromValue(msgParam));
                topLevelItem->addChild(msgItem);
            }
        };

    if (!msg->idAsString().isEmpty()) {
        addMsgFunc(msg, "Application");
    }
    addMsgFunc(property::message::TransportMsg().getFrom(*msg), "Transport");
    addMsgFunc(property::message::RawDataMsg().getFrom(*msg), "Raw Data");

    m_ui.m_protocolsTreeWidget->addTopLevelItem(topLevelItem);

    auto* topProtocolItem = m_ui.m_protocolsTreeWidget->topLevelItem(0);
    assert(topProtocolItem != nullptr);
    auto* firstMsgItem = topProtocolItem->child(0);
    if (firstMsgItem != nullptr) {
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

void ProtocolsStackWidget::newItemSelected()
{
    assert(m_ui.m_protocolsTreeWidget != nullptr);
    auto* item = m_ui.m_protocolsTreeWidget->currentItem();
    assert(item != nullptr);

    auto msgPtrVar = item->data(0, Qt::UserRole);
    if (!msgPtrVar.isValid()) {
        // Top level item
        item = item->child(0);
        assert(item != nullptr);
        msgPtrVar = item->data(0, Qt::UserRole);
        m_ui.m_protocolsTreeWidget->setCurrentItem(item);
        assert(m_ui.m_protocolsTreeWidget->isItemSelected(item));
    }

    assert(msgPtrVar.isValid());
    reportMessageSelected(item);
}

void ProtocolsStackWidget::reportMessageSelected(QTreeWidgetItem* item)
{
    auto msgPtr = msgFromItem(item);
    assert(msgPtr);
    bool editEnabled = (item == m_ui.m_protocolsTreeWidget->topLevelItem(0)->child(0));
    emit sigMessageSelected(msgPtr, editEnabled);
}

MessagePtr ProtocolsStackWidget::msgFromItem(QTreeWidgetItem* item)
{
    auto msgPtrVar = item->data(0, Qt::UserRole);
    assert(msgPtrVar.isValid());
    assert(msgPtrVar.canConvert<MessagePtr>());
    return msgPtrVar.value<MessagePtr>();
}

}  // namespace comms_champion


