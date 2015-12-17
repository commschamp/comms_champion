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

#include "MsgDetailsWidget.h"

#include <cassert>

#include "comms_champion/DefaultMessageDisplayHandler.h"
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
  : Base(parentObj),
    m_msgDisplayHandler(new DefaultMessageDisplayHandler())
{
    m_ui.setupUi(this);
}

void MsgDetailsWidget::setEditEnabled(bool enabled)
{
    m_editEnabled = enabled;
    if (m_displayedMsgWidget != nullptr) {
        m_displayedMsgWidget->setEditEnabled(enabled);
    }
}

void MsgDetailsWidget::displayMessage(MessageInfo::MessagePtr msg)
{
    assert(msg);
    auto msgWidget = m_msgDisplayHandler->createMsgWidget(*msg);
    assert(msgWidget);
    msgWidget->setEditEnabled(m_editEnabled);

    connect(
        msgWidget.get(), SIGNAL(sigMsgUpdated()),
        this, SIGNAL(sigMsgUpdated()));

    m_displayedMsgWidget = msgWidget.get();
    m_ui.m_scrollArea->setWidget(msgWidget.release());
}

void MsgDetailsWidget::updateTitle(MessageInfo::MessagePtr msg)
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

} /* namespace comms_champion */
