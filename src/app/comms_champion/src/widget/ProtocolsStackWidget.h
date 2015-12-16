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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QWidget>

#include "ui_ProtocolsStackWidget.h"
CC_ENABLE_WARNINGS()

#include "comms_champion/MessageInfo.h"

namespace comms_champion
{

class ProtocolsStackWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;
public:
    ProtocolsStackWidget(QWidget* parent = nullptr);
    ~ProtocolsStackWidget();

    void displayMessage(MessageInfoPtr msgInfo, bool force);
    void clear();

signals:
    void sigMessageSelected(MessageInfo::MessagePtr msgInfo, bool editEnabled);

private slots:
    void itemClicked(QTreeWidgetItem* item, int column);

private:
    void reportMessageSelected(QTreeWidgetItem* item);
    static MessageInfo::MessagePtr msgFromItem(QTreeWidgetItem* item);

    Ui::ProtocolsStackWidget m_ui;
};

}  // namespace comms_champion


