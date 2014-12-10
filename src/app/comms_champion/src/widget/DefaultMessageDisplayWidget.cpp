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

#include "DefaultMessageDisplayWidget.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>

#include "MsgDetailsWidget.h"
#include "ProtocolsStackWidget.h"

namespace comms_champion
{

DefaultMessageDisplayWidget::DefaultMessageDisplayWidget(QWidget* parent)
  : Base(parent),
    m_msgDetailsWidget(new MsgDetailsWidget()),
    m_protocolsDetailsWidget(new ProtocolsStackWidget())
{
    auto* splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(m_msgDetailsWidget);
    splitter->addWidget(m_protocolsDetailsWidget);

    auto* layout = new QVBoxLayout();
    layout->addWidget(splitter);

    setLayout(layout);
}

void DefaultMessageDisplayWidget::displayMessageImpl(
    ProtocolsInfoPtr protocolsInfo)
{
    m_msgDetailsWidget->displayMessage(protocolsInfo);
    static_cast<void>(protocolsInfo);
}

}  // namespace comms_champion


