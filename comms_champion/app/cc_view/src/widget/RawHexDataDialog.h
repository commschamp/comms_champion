//
// Copyright 2016 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "ui_RawHexDataDialog.h"
CC_ENABLE_WARNINGS()

#include "comms_champion/Protocol.h"


namespace comms_champion
{

class RawHexDataDialog : public QDialog
{
    Q_OBJECT
    using Base = QDialog;
public:
    typedef Protocol::MessagesList MessagesList;
    RawHexDataDialog(
        MessagesList& msgs,
        ProtocolPtr protocol,
        QWidget* parentObj = nullptr);
    ~RawHexDataDialog() noexcept;

private slots:
    void valueChanged();
    void accept();
    void reset();

private:
    MessagesList& m_msgs;
    ProtocolPtr m_protocol;
    Ui::RawHexDataDialog m_ui;
};

} // namespace comms_champion


