//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
CC_ENABLE_WARNINGS()

#include "comms_champion/Message.h"
#include "comms_champion/property/message.h"

namespace comms_champion
{

namespace
{

const QString EmptyStr;

}  // namespace

MsgListWidget::MsgListWidget(
    const QString& title,
    QWidget* toolbar,
    QWidget* parentObj)
  : Base(parentObj),
    m_title(title)
{
    m_ui.setupUi(this);
    m_ui.m_groupBoxLayout->insertWidget(0, toolbar);
    updateTitle();

    m_ui.m_listWidget->setUniformItemSizes(true);
    connect(
        m_ui.m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(itemClicked(QListWidgetItem*)));
    connect(
        m_ui.m_listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(currentItemChanged(QListWidgetItem*, QListWidgetItem*)));
    connect(
        m_ui.m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(itemDoubleClicked(QListWidgetItem*)));
}

void MsgListWidget::addMessage(MessagePtr msg)
{
    assert(msg);
    m_ui.m_listWidget->addItem(getMsgNameText(msg));
    auto* item = m_ui.m_listWidget->item(m_ui.m_listWidget->count() - 1);
    item->setToolTip(msgTooltipImpl());

    bool valid = msg->isValid();

    auto type = property::message::Type().getFrom(*msg);
    if ((type != MsgType::Invalid) && (!msg->idAsString().isEmpty())) {
        item->setForeground(getItemColourImpl(type, valid));
    }
    else {
        item->setForeground(defaultItemColour(valid));
    }

    item->setData(
        Qt::UserRole,
        QVariant::fromValue(msg));

    if (m_selectOnAdd) {
        m_ui.m_listWidget->blockSignals(true);
        m_ui.m_listWidget->setCurrentRow(m_ui.m_listWidget->count() - 1);
        m_ui.m_listWidget->blockSignals(false);
        assert(m_ui.m_listWidget->currentItem() == item);
    }

    if (m_ui.m_listWidget->currentRow() < 0) {
        m_ui.m_listWidget->scrollToBottom();
    }

    updateTitle();
}

void MsgListWidget::updateCurrentMessage(MessagePtr msg)
{
    auto* item = m_ui.m_listWidget->currentItem();
    if (item == nullptr) {
        assert(!"No item is selected for update");
        return;
    }

    item->setData(
        Qt::UserRole,
        QVariant::fromValue(msg));

    item->setText(getMsgNameText(msg));

    if ((!msg) || (msg->idAsString().isEmpty())) {
        item->setForeground(defaultItemColour(false));
        return;
    }

    bool valid = msg->isValid();

    auto type = property::message::Type().getFrom(*msg);
    if (type != MsgType::Invalid) {
        item->setForeground(getItemColourImpl(type, valid));
    }
    else {
        item->setForeground(defaultItemColour(valid));
    }
}

void MsgListWidget::deleteCurrentMessage()
{
    auto* item = m_ui.m_listWidget->currentItem();
    if (item == nullptr) {
        assert(!"No item is selected for deletion");
        return;
    }

    m_ui.m_listWidget->blockSignals(true);
    delete item; // will remove from the list
    m_ui.m_listWidget->blockSignals(false);


    updateTitle();

    auto* nextItem = m_ui.m_listWidget->currentItem();
    if (nextItem != nullptr) {
        processClick(nextItem);
    }
}

void MsgListWidget::selectOnAdd(bool enabled)
{
    m_selectOnAdd = enabled;
}

void MsgListWidget::clearSelection()
{
    m_ui.m_listWidget->clearSelection();
    m_ui.m_listWidget->setCurrentRow(-1);
}

void MsgListWidget::clearList(bool reportDeleted)
{
    MessagesList msgsList;
    if (reportDeleted) {
        auto count = m_ui.m_listWidget->count();
        for (auto idx = 0; idx < count; ++idx) {
            auto* item = m_ui.m_listWidget->item(idx);
            auto msg = getMsgFromItem(item);
            msgsList.push_back(std::move(msg));
        }
    }

    clearList();

    if (reportDeleted) {
        msgListClearedImpl(std::move(msgsList));
    }
}

void MsgListWidget::clearList()
{
    m_ui.m_listWidget->clear();
    updateTitle();
}

void MsgListWidget::stateChanged(int state)
{
    stateChangedImpl(state);
}

void MsgListWidget::moveSelectedTop()
{
    auto curRow = m_ui.m_listWidget->currentRow();
    if (curRow <= 0) {
        assert(!"No item is selected or moving up top item");
        return;
    }

    moveItem(curRow, 0);
}

void MsgListWidget::moveSelectedUp()
{
    auto curRow = m_ui.m_listWidget->currentRow();
    if (curRow <= 0) {
        assert(!"No item is selected or moving up top item");
        return;
    }

    moveItem(curRow, curRow - 1);
}

void MsgListWidget::moveSelectedDown()
{
    auto curRow = m_ui.m_listWidget->currentRow();
    if ((m_ui.m_listWidget->count() - 1) <= curRow) {
        assert(!"No item is selected or moving down bottom item");
        return;
    }

    moveItem(curRow, curRow + 1);
}

void MsgListWidget::moveSelectedBottom()
{
    auto curRow = m_ui.m_listWidget->currentRow();
    if ((m_ui.m_listWidget->count() - 1) <= curRow) {
        assert(!"No item is selected or moving down bottom item");
        return;
    }

    moveItem(curRow, m_ui.m_listWidget->count() - 1);
}

void MsgListWidget::titleNeedsUpdate()
{
    m_title = getTitleImpl();
    updateTitle();
}

void MsgListWidget::loadMessages(
    bool clearExisting,
    const QString& filename,
    ProtocolPtr protocol)
{
    if (clearExisting) {
        clearList();
    }

    loadMessagesImpl(filename, *protocol);
}

void MsgListWidget::saveMessages(const QString& filename)
{
    saveMessagesImpl(filename);
}

void MsgListWidget::selectMsg(int idx)
{
    assert(idx < m_ui.m_listWidget->count());
    m_ui.m_listWidget->blockSignals(true);
    m_ui.m_listWidget->setCurrentRow(idx);
    m_ui.m_listWidget->blockSignals(false);
}

void MsgListWidget::msgClickedImpl(MessagePtr msg, int idx)
{
    static_cast<void>(msg);
    static_cast<void>(idx);
}

void MsgListWidget::msgDoubleClickedImpl(MessagePtr msg, int idx)
{
    static_cast<void>(msg);
    static_cast<void>(idx);
}

void MsgListWidget::msgListClearedImpl(MessagesList&& msgs)
{
    static_cast<void>(msgs);
}

QString MsgListWidget::msgPrefixImpl(const Message& msg) const
{
    static_cast<void>(msg);
    return QString();
}

const QString& MsgListWidget::msgTooltipImpl() const
{
    return EmptyStr;
}

void MsgListWidget::stateChangedImpl(int state)
{
    static_cast<void>(state);
}

Qt::GlobalColor MsgListWidget::getItemColourImpl(MsgType type, bool valid) const
{
    static_cast<void>(type);
    return defaultItemColour(valid);
}

void MsgListWidget::msgMovedImpl(int idx)
{
    static_cast<void>(idx);
}

QString MsgListWidget::getTitleImpl() const
{
    assert(!"Should not be called");
    return QString();
}

void MsgListWidget::loadMessagesImpl(const QString& filename, Protocol& protocol)
{
    static_cast<void>(filename);
    static_cast<void>(protocol);
}

void MsgListWidget::saveMessagesImpl(const QString& filename)
{
    static_cast<void>(filename);
}

MessagePtr MsgListWidget::currentMsg() const
{
    auto* item = m_ui.m_listWidget->currentItem();
    assert(item != nullptr);
    return getMsgFromItem(item);
}

MsgListWidget::MessagesList MsgListWidget::allMsgs() const
{
    MessagesList allMsgsList;
    for (auto idx = 0; idx < m_ui.m_listWidget->count(); ++idx) {
        auto* item = m_ui.m_listWidget->item(idx);
        auto msgPtr = getMsgFromItem(item);
        assert(msgPtr);
        allMsgsList.push_back(std::move(msgPtr));
    }
    return allMsgsList;
}

void MsgListWidget::itemClicked(QListWidgetItem* item)
{
    assert(item != nullptr);
    if (m_selectedItem == item) {
        assert(0 < m_lastSelectionTimestamp);
        auto timestamp = QDateTime::currentMSecsSinceEpoch();
        static const decltype(timestamp) MinThreshold = 250;
        if (timestamp < (m_lastSelectionTimestamp + MinThreshold)) {
            return;
        }
    }

    processClick(item);
}

void MsgListWidget::currentItemChanged(QListWidgetItem* current, QListWidgetItem* prev)
{
    static_cast<void>(prev);

    m_selectedItem = current;
    if (current != nullptr) {
        m_lastSelectionTimestamp = QDateTime::currentMSecsSinceEpoch();
        processClick(current);
        return;
    }

    m_lastSelectionTimestamp = 0;
    return;
}

void MsgListWidget::itemDoubleClicked(QListWidgetItem* item)
{
    msgDoubleClickedImpl(
        getMsgFromItem(item),
        m_ui.m_listWidget->row(item));
}

MessagePtr MsgListWidget::getMsgFromItem(QListWidgetItem* item) const
{
    auto var = item->data(Qt::UserRole);
    assert(var.canConvert<MessagePtr>());
    return var.value<MessagePtr>();
}

QString MsgListWidget::getMsgNameText(MessagePtr msg)
{
    assert(msg);
    auto itemStr = msgPrefixImpl(*msg);
    if (!itemStr.isEmpty()) {
        itemStr.append(": ");
    }
    itemStr.append(msg->name());
    return itemStr;
}

Qt::GlobalColor MsgListWidget::defaultItemColour(bool valid) const
{
    if (valid) {
        return Qt::black;
    }
    return Qt::red;
}

void MsgListWidget::moveItem(int fromRow, int toRow)
{
    assert(fromRow < m_ui.m_listWidget->count());
    auto* item = m_ui.m_listWidget->takeItem(fromRow);
    assert(toRow <= m_ui.m_listWidget->count());
    m_ui.m_listWidget->insertItem(toRow, item);
    m_ui.m_listWidget->setCurrentRow(toRow);
    msgMovedImpl(toRow);
}

void MsgListWidget::updateTitle()
{
    auto title =
        m_title +
        QString(" [%1]").arg(m_ui.m_listWidget->count(), 1, 10, QChar('0'));
    m_ui.m_groupBox->setTitle(title);
}

void MsgListWidget::processClick(QListWidgetItem* item)
{
    msgClickedImpl(
        getMsgFromItem(item),
        m_ui.m_listWidget->row(item));
}


}  // namespace comms_champion


