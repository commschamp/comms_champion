//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
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
CC_ENABLE_WARNINGS()

#include "SerialSocket.h"
#include "ui_SerialSocketConfigWidget.h"

namespace comms_champion
{

namespace plugin
{

namespace serial_socket
{

class SerialSocketConfigWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget Base;
public:
    explicit SerialSocketConfigWidget(
        SerialSocket& socket,
        QWidget* parentObj = nullptr);

    ~SerialSocketConfigWidget() noexcept;

private slots:
    void nameChanged(const QString& value);
    void baudChanged(int value);
    void dataBitsChanged(int value);
    void parityChanged(int value);
    void stopBitsChanged(int value);
    void flowControlChanged(int value);

private:
    SerialSocket& m_socket;
    Ui::SerialSocketConfigWidget m_ui;
};




}  // namespace serial_socket

}  // namespace plugin

}  // namespace comms_champion


