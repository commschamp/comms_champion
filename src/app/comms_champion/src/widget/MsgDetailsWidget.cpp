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

#include "MsgDetailsWidget.h"

#include <cassert>

CC_DISABLE_WARNINGS()
#include <QtWidgets/QScrollBar>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"
#include "GuiAppMgr.h"

namespace comms_champion
{

namespace
{

const QString& getTitlePrefix()
{
    static const QString Str(QObject::tr("Message Details"));
    return Str;
}

}  // namespace

MsgDetailsWidget::MsgDetailsWidget(QWidget* parentObj)
  : Base(parentObj)
{
    m_ui.setupUi(this);
    auto* scrollBar = m_ui.m_scrollArea->verticalScrollBar();
    assert(scrollBar != nullptr);
    connect(
        scrollBar, SIGNAL(valueChanged(int)),
        this, SLOT(widgetScrolled(int)));
}

void MsgDetailsWidget::setEditEnabled(bool enabled)
{
    m_editEnabled = enabled;
    if (m_displayedMsgWidget != nullptr) {
        m_displayedMsgWidget->setEditEnabled(enabled);
    }
}

void MsgDetailsWidget::displayMessage(MessagePtr msg)
{
    assert(msg);
    msg->dispatch(m_msgDisplayHandler);
    auto msgWidget = m_msgDisplayHandler.getMsgWidget();
    assert(msgWidget);
    msgWidget->setEditEnabled(m_editEnabled);

    connect(
        msgWidget.get(), SIGNAL(sigMsgUpdated()),
        this, SIGNAL(sigMsgUpdated()));

    m_displayedMsgWidget = msgWidget.get();

    auto* scrollBar = m_ui.m_scrollArea->verticalScrollBar();
    assert(scrollBar != nullptr);
    scrollBar->blockSignals(true);
    m_ui.m_scrollArea->setWidget(msgWidget.release());
    scrollBar->blockSignals(false);

    auto scrollValue = property::message::ScrollPos().getFrom(*msg);
    scrollBar->setValue(scrollValue);
    m_displayedMsg = std::move(msg);
}

void MsgDetailsWidget::updateTitle(MessagePtr msg)
{
    auto title = getTitlePrefix();
    title.append(": ");
    title.append(msg->name());
    auto idStr = msg->idAsString();
    if (idStr != msg->name()) {
        title.append(" (");
        title.append(idStr);
        title.append(")");
    }
    m_ui.m_groupBox->setTitle(title);
}

void MsgDetailsWidget::clear()
{
    m_displayedMsgWidget = nullptr;
    m_ui.m_scrollArea->setWidget(new QWidget());
    m_ui.m_groupBox->setTitle(getTitlePrefix());
}

void MsgDetailsWidget::refresh()
{
    if ((m_displayedMsgWidget != nullptr) && (m_editEnabled)) {
        m_displayedMsgWidget->refresh();
    }
}

void MsgDetailsWidget::widgetScrolled(int value)
{
    if (m_displayedMsg == nullptr) {
        return;
    }

    property::message::ScrollPos().setTo(value, *m_displayedMsg);
}

} // namespace comms_champion
