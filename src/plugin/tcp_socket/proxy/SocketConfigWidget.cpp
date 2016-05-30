//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "SocketConfigWidget.h"

#include <limits>

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

namespace proxy
{

SocketConfigWidget::SocketConfigWidget(
    Socket& socket,
    QWidget* parentObj)
  : Base(parentObj),
    m_socket(socket)
{
    m_ui.setupUi(this);

    m_ui.m_localPortSpinBox->setRange(
        1,
        static_cast<int>(std::numeric_limits<PortType>::max()));

    m_ui.m_localPortSpinBox->setValue(
        static_cast<int>(m_socket.getPort()));

    m_ui.m_remoteHostLineEdit->setText(m_socket.getRemoteHost());

    m_ui.m_remotePortSpinBox->setRange(
        1,
        static_cast<int>(std::numeric_limits<PortType>::max()));

    m_ui.m_remotePortSpinBox->setValue(
        static_cast<int>(m_socket.getRemotePort()));

    connect(
        m_ui.m_localPortSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(localPortValueChanged(int)));

    connect(
        m_ui.m_remoteHostLineEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(remoteHostValueChanged(const QString&)));

    connect(
        m_ui.m_remotePortSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(remotePortValueChanged(int)));
}

SocketConfigWidget::~SocketConfigWidget() = default;

void SocketConfigWidget::localPortValueChanged(int value)
{
    m_socket.setPort(static_cast<PortType>(value));
}

void SocketConfigWidget::remoteHostValueChanged(const QString& value)
{
    m_socket.setRemoteHost(value);
}

void SocketConfigWidget::remotePortValueChanged(int value)
{
    m_socket.setRemotePort(static_cast<PortType>(value));
}

}  // namespace proxy

}  // namespace tcp_socket

}  // namespace plugin

}  // namespace comms_champion


