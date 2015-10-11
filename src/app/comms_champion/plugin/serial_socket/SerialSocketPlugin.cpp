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

#include "SerialSocketPlugin.h"

#include <memory>
#include <cassert>

#include "SerialSocket.h"
#include "SerialSocketConfigWidget.h"

namespace comms_champion
{

namespace plugin
{

namespace serial_socket
{

namespace
{

const QString MainConfigKey("cc_serial_socket");
const QString DevNameSubKey("dev");
const QString BaudSubKey("baud");
const QString DataBitsSubKey("data_bits");
const QString ParitySubKey("parity");
const QString StopBitsSubKey("stop_bits");
const QString FlowControlSubKey("flow");

}  // namespace

SerialSocketPlugin::SerialSocketPlugin()
{
}

SerialSocketPlugin::~SerialSocketPlugin()
{
    if (isApplied()) {
        auto* interface = getCtrlInterface();
        assert(interface != nullptr);
        assert(m_socket);
        interface->removeSocket(m_socket);
        m_socket.reset();
    }
}

void SerialSocketPlugin::applyImpl()
{
    assert(!isApplied());
    createSocketIfNeeded();

    auto* interface = getCtrlInterface();
    if (interface != nullptr) {
        interface->addSocket(m_socket);
    }
    assert(m_socket);
}

void SerialSocketPlugin::getCurrentConfigImpl(QVariantMap& config)
{
    createSocketIfNeeded();

    QVariantMap subConfig;
    subConfig.insert(DevNameSubKey, m_socket->name());
    subConfig.insert(BaudSubKey, m_socket->baud());
    subConfig.insert(DataBitsSubKey, static_cast<int>(m_socket->dataBits()));
    subConfig.insert(ParitySubKey, static_cast<int>(m_socket->parity()));
    subConfig.insert(StopBitsSubKey, static_cast<int>(m_socket->stopBits()));
    subConfig.insert(FlowControlSubKey, static_cast<int>(m_socket->flowControl()));
    config.insert(MainConfigKey, QVariant::fromValue(subConfig));
}

void SerialSocketPlugin::reconfigureImpl(const QVariantMap& config)
{
    auto subConfigVar = config.value(MainConfigKey);
    if ((!subConfigVar.isValid()) || (!subConfigVar.canConvert<QVariantMap>())) {
        return;
    }

    createSocketIfNeeded();

    auto subConfig = subConfigVar.value<QVariantMap>();
    auto devNameVar = subConfig.value(DevNameSubKey);
    if (devNameVar.isValid() && devNameVar.canConvert<QString>()) {
        m_socket->name() = devNameVar.toString();
    }

    auto baudVar = subConfig.value(BaudSubKey);
    if (baudVar.isValid() && baudVar.canConvert<int>()) {
        auto baud = baudVar.value<int>();
        if (0 < baud) {
            m_socket->baud() = baud;
        }
    }

    auto dataBitsVar = subConfig.value(DataBitsSubKey);
    if (dataBitsVar.isValid() && dataBitsVar.canConvert<int>()) {
        auto dataBits = static_cast<QSerialPort::DataBits>(dataBitsVar.value<int>());
        if ((QSerialPort::Data5 <= dataBits) && (dataBits <= QSerialPort::Data8)) {
            m_socket->dataBits() = dataBits;
        }
    }

    auto parityVar = subConfig.value(ParitySubKey);
    if (parityVar.isValid() && parityVar.canConvert<int>()) {
        auto parity = static_cast<QSerialPort::Parity>(parityVar.value<int>());
        if ((QSerialPort::NoParity <= parity) &&
            (parity <= QSerialPort::MarkParity) &&
            (parity != 1)) {
            m_socket->parity() = parity;
        }
    }

    auto stopBitsVar = subConfig.value(StopBitsSubKey);
    if (stopBitsVar.isValid() && stopBitsVar.canConvert<int>()) {
        auto stopBits = static_cast<QSerialPort::StopBits>(stopBitsVar.value<int>());
        if ((QSerialPort::OneStop <= stopBits) && (stopBits <= QSerialPort::OneAndHalfStop)) {
            m_socket->stopBits() = stopBits;
        }
    }

    auto flowVar = subConfig.value(FlowControlSubKey);
    if (flowVar.isValid() && flowVar.canConvert<int>()) {
        auto flow = static_cast<QSerialPort::FlowControl>(flowVar.value<int>());
        if ((QSerialPort::NoFlowControl <= flow) && (flow <= QSerialPort::SoftwareControl)) {
            m_socket->flowControl() = flow;
        }
    }
}

SerialSocketPlugin::WidgetPtr SerialSocketPlugin::getConfigWidgetImpl()
{
    createSocketIfNeeded();
    assert(m_socket);
    return WidgetPtr(new SerialSocketConfigWidget(*m_socket));
}

void SerialSocketPlugin::createSocketIfNeeded()
{
    if (!m_socket) {
        m_socket.reset(new SerialSocket());
    }
}

}  // namespace serial_socket

}  // namespace plugin


}  // namespace comms_champion


