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

#include <memory>

#include <QtWidgets/QWidget>

#include "comms_champion/MessageInfo.h"
#include "comms_champion/MessageDisplayHandler.h"
#include "ui_MsgDetailsWidget.h"

namespace comms_champion
{

class MsgDetailsWidget: public QWidget
{
    Q_OBJECT
    using Base = QWidget;
public:
    MsgDetailsWidget(QWidget* parent = nullptr);

public slots:
    void setEditEnabled(bool enabled);
    void displayMessage(MessageInfo::MessagePtr msg);
    void updateTitle(MessageInfo::MessagePtr msg);
    void clear();
    void refresh();

signals:
    void sigMsgUpdated();

private:
    typedef std::unique_ptr<MessageDisplayHandler> MsgDisplayHandlerPtr;
    Ui::MsgDetailsWidget m_ui;
    MsgDisplayHandlerPtr m_msgDisplayHandler;
    MessageWidget* m_displayedMsgWidget = nullptr;
    bool m_editEnabled = true;
};

} /* namespace comms_champion */
