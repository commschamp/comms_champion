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
#include "GlobalConstants.h"

namespace comms_champion
{

namespace
{

const QString EmptyStr;

}  // namespace

MsgListWidget::MsgListWidget(
    const QString& title,
    QWidget* toolbar,
    QWidget* parent)
  : Base(parent),
    m_title(title)
{
    m_ui.setupUi(this);
    m_ui.m_groupBoxLayout->insertWidget(0, toolbar);
    updateTitle();

    connect(m_ui.m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(itemClicked(QListWidgetItem*)));
    connect(m_ui.m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(itemDoubleClicked(QListWidgetItem*)));
}

void MsgListWidget::addMessage(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    m_ui.m_listWidget->addItem(getMsgNameText(msgInfo));
    auto* item = m_ui.m_listWidget->item(m_ui.m_listWidget->count() - 1);
    item->setToolTip(msgTooltipImpl());

    bool valid = false;
    auto appMsgPtr = msgInfo->getAppMessage();
    if (appMsgPtr) {
        valid = appMsgPtr->isValid();
    }

    auto typeVar =
        msgInfo->getExtraProperty(GlobalConstants::msgTypePropertyName());
    if (typeVar.isValid()) {
        assert(typeVar.canConvert<int>());
        auto type = static_cast<MsgType>(typeVar.value<int>());
        item->setForeground(getItemColourImpl(type, valid));
    }
    else {
        item->setForeground(defaultItemColour(valid));
    }

    item->setData(
        Qt::UserRole,
        QVariant::fromValue(msgInfo));

    if (m_selectOnAdd) {
//        m_ui.m_listWidget->setCurrentItem(item);
        m_ui.m_listWidget->setCurrentRow(m_ui.m_listWidget->count() - 1);
        assert(m_ui.m_listWidget->currentItem() == item);
    }

    if (m_ui.m_listWidget->currentRow() < 0) {
        m_ui.m_listWidget->scrollToBottom();
    }

    updateTitle();
}

void MsgListWidget::updateCurrentMessage()
{
    auto* item = m_ui.m_listWidget->currentItem();
    if (item == nullptr) {
        assert(!"No item is selected for update");
        return;
    }

    auto msgInfo = getMsgFromItem(item);
    assert(msgInfo);
    item->setText(getMsgNameText(std::move(msgInfo)));
}

void MsgListWidget::deleteCurrentMessage()
{
    auto* item = m_ui.m_listWidget->currentItem();
    if (item == nullptr) {
        assert(!"No item is selected for deletion");
        return;
    }

    auto msgInfo = getMsgFromItem(item);
    delete item; // will remove from the list

    updateTitle();

    auto* nextItem = m_ui.m_listWidget->currentItem();
    if (nextItem != nullptr) {
        itemClicked(nextItem);
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

void MsgListWidget::clear(bool reportDeleted)
{
    MsgInfosList msgInfosList;
    if (reportDeleted) {
        auto count = m_ui.m_listWidget->count();
        for (auto idx = 0; idx < count; ++idx) {
            auto* item = m_ui.m_listWidget->item(idx);
            auto msgInfo = getMsgFromItem(item);
            msgInfosList.push_back(std::move(msgInfo));
        }
    }

    clear();

    if (reportDeleted) {
        msgListClearedImpl(std::move(msgInfosList));
    }
}

void MsgListWidget::clear()
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

void MsgListWidget::msgClickedImpl(MessageInfoPtr msgInfo, int idx)
{
    static_cast<void>(msgInfo);
    static_cast<void>(idx);
}

void MsgListWidget::msgDoubleClickedImpl(MessageInfoPtr msgInfo, int idx)
{
    static_cast<void>(msgInfo);
    static_cast<void>(idx);
}

void MsgListWidget::msgListClearedImpl(MsgInfosList&& msgInfosList)
{
    static_cast<void>(msgInfosList);
}

QString MsgListWidget::msgPrefixImpl(const MessageInfo& msgInfo) const
{
    static_cast<void>(msgInfo);
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

MessageInfoPtr MsgListWidget::currentMsg() const
{
    auto* item = m_ui.m_listWidget->currentItem();
    assert(item != nullptr);
    return getMsgFromItem(item);
}

MsgListWidget::MsgInfosList MsgListWidget::allMsgs() const
{
    MsgInfosList allMsgsList;
    for (auto idx = 0; idx < m_ui.m_listWidget->count(); ++idx) {
        auto* item = m_ui.m_listWidget->item(idx);
        auto msgInfoPtr = getMsgFromItem(item);
        assert(msgInfoPtr);
        allMsgsList.push_back(std::move(msgInfoPtr));
    }
    return allMsgsList;
}

void MsgListWidget::itemClicked(QListWidgetItem* item)
{
    msgClickedImpl(
        getMsgFromItem(item),
        m_ui.m_listWidget->row(item));
}

void MsgListWidget::itemDoubleClicked(QListWidgetItem* item)
{
    msgDoubleClickedImpl(
        getMsgFromItem(item),
        m_ui.m_listWidget->row(item));
}

MessageInfoPtr MsgListWidget::getMsgFromItem(QListWidgetItem* item) const
{
    auto var = item->data(Qt::UserRole);
    assert(var.canConvert<MessageInfoPtr>());
    return var.value<MessageInfoPtr>();
}

QString MsgListWidget::getMsgNameText(MessageInfoPtr msgInfo)
{
    assert(msgInfo);
    auto itemStr = msgPrefixImpl(*msgInfo);
    if (!itemStr.isEmpty()) {
        itemStr.append(": ");
    }

    do {
        auto appMsg = msgInfo->getAppMessage();
        if (appMsg) {
            itemStr.append(appMsg->name());
            break;
        }

        if (msgInfo->getTransportMessage()) {
            static const QString UnknownMsgName("???");
            itemStr.append(UnknownMsgName);
            break;
        }

        assert(msgInfo->getRawDataMessage());
        static const QString GarbageMsgName("-#-");
        itemStr.append(GarbageMsgName);
    } while (false);

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


}  // namespace comms_champion


