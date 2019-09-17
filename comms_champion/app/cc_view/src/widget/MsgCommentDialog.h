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


#pragma once

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QDialog>

#include "ui_MsgCommentDialog.h"
CC_ENABLE_WARNINGS()

#include "comms_champion/Message.h"

namespace comms_champion
{

class MsgCommentDialog : public QDialog
{
    Q_OBJECT
    using Base = QDialog;
public:
    MsgCommentDialog(
        MessagePtr msg,
        QWidget* parentObj = nullptr);

private slots:
    void accept();

private:
    Ui::MsgCommentDialog m_ui;
    MessagePtr m_msg;
};

}  // namespace comms_champion

