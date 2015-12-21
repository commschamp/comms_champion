//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include "DefaultMessageDisplayWidget.h"

#include <cassert>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>
CC_ENABLE_WARNINGS()

#include "MsgDetailsWidget.h"
#include "ProtocolsStackWidget.h"

namespace comms_champion
{

DefaultMessageDisplayWidget::DefaultMessageDisplayWidget(QWidget* parentObj)
  : Base(parentObj),
    m_msgDetailsWidget(new MsgDetailsWidget()),
    m_protocolsDetailsWidget(new ProtocolsStackWidget())
{
    connect(
        m_protocolsDetailsWidget, SIGNAL(sigMessageSelected(MessageInfo::MessagePtr, bool)),
        this, SLOT(msgSelectedInProtocol(MessageInfo::MessagePtr, bool)));

    connect(
        m_msgDetailsWidget, SIGNAL(sigMsgUpdated()),
        this, SIGNAL(sigMsgUpdated()));

    auto* splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(m_msgDetailsWidget);
    splitter->addWidget(m_protocolsDetailsWidget);

    auto* widgetLayout = new QVBoxLayout();
    widgetLayout->addWidget(splitter);

    setLayout(widgetLayout);
}

void DefaultMessageDisplayWidget::displayMessageImpl(
    MessageInfoPtr msgInfo,
    bool force)
{
    assert(msgInfo);
    auto appMsg = msgInfo->getAppMessage();
    if (appMsg) {
        m_msgDetailsWidget->updateTitle(appMsg);
    }
    else {
        m_msgDetailsWidget->clear();
    }
    m_protocolsDetailsWidget->displayMessage(std::move(msgInfo), force);
}

void DefaultMessageDisplayWidget::setEditEnabledImpl(bool enabled)
{
    m_globalEditEnabled = enabled;
    m_msgDetailsWidget->setEditEnabled(enabled);
}

void DefaultMessageDisplayWidget::clearImpl()
{
    m_msgDetailsWidget->clear();
    m_protocolsDetailsWidget->clear();
}

void DefaultMessageDisplayWidget::refreshImpl()
{
    if (m_msgDetailsWidget != nullptr) {
        m_msgDetailsWidget->refresh();
    }
}

void DefaultMessageDisplayWidget::msgSelectedInProtocol(
    MessageInfo::MessagePtr msg,
    bool editEnabled)
{
    m_msgDetailsWidget->displayMessage(msg);
    m_msgDetailsWidget->setEditEnabled(m_globalEditEnabled && editEnabled);
}

}  // namespace comms_champion


