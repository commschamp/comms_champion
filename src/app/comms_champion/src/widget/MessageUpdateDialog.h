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

#include <QtWidgets/QDialog>

#include "comms_champion/Protocol.h"
#include "comms_champion/MessageDisplayWidget.h"

#include "ui_MessageUpdateDialog.h"


namespace comms_champion
{

class MessageUpdateDialog : public QDialog
{
    Q_OBJECT
    using Base = QDialog;
public:
    MessageUpdateDialog(
        MessageInfoPtr& msgInfo,
        ProtocolPtr protocol,
        QWidget* parent = nullptr);

private slots:
    void itemClicked(QListWidgetItem* item);

private:
    void refreshDisplayedList();
    MessageInfoPtr getMsgFromItem(QListWidgetItem* item);

    MessageInfoPtr& m_msgInfo;
    ProtocolPtr m_protocol;
    Protocol::MessagesList m_allMsgs;
    MessageDisplayWidget* m_msgDisplayWidget = nullptr;
    Ui::MessageUpdateDialog m_ui;
};

}  // namespace comms_champion

