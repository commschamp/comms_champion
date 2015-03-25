//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include "ClientSocket.h"
#include "ui_ClientSocketConfigWidget.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

class ClientSocketConfigWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget Base;
public:
    typedef ClientSocket::PortType PortType;

    explicit ClientSocketConfigWidget(
        ClientSocket& socket,
        QWidget* parent = nullptr);

    ~ClientSocketConfigWidget();

private slots:
    void hostValueChanged(const QString& value);
    void portValueChanged(int value);

private:
    ClientSocket& m_socket;
    Ui::ClientSocketConfigWidget m_ui;
};




}  // namespace tcp_socket

}  // namespace plugin

}  // namespace comms_champion


