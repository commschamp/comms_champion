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


#pragma once

#include <QtWidgets/QWidget>

#include "comms_champion/MessageInfo.h"

#include "ui_ProtocolsStackWidget.h"

namespace comms_champion
{

class ProtocolsStackWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;
public:
    ProtocolsStackWidget(QWidget* parent = nullptr);
    ~ProtocolsStackWidget();

    void displayMessage(MessageInfoPtr msgInfo);
    void clear();

signals:
    void sigMessageSelected(MessageInfo::MessagePtr msgInfo);

private slots:
    void itemClicked(QTreeWidgetItem* item, int column);

private:
    void reportMessageSelected(QTreeWidgetItem* item);

    Ui::ProtocolsStackWidget m_ui;
};

}  // namespace comms_champion


