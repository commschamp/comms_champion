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

#include <cassert>
#include "SerialSocket.h"
#include <QtSerialPort/QSerialPortInfo>

#include <algorithm>

namespace comms_champion
{

namespace plugin
{

namespace serial_socket
{

SerialSocket::SerialSocket()
{
    auto list = QSerialPortInfo::availablePorts();
    auto firstDev = std::min_element(
        list.begin(), list.end(),
        [](const QSerialPortInfo& info1, const QSerialPortInfo& info2) -> bool
        {
            return info1.systemLocation() < info2.systemLocation();
        });

    if (firstDev != list.end()) {
        m_name = firstDev->systemLocation();
    }

    connect(
        &m_serial, SIGNAL(error(QSerialPort::SerialPortError)),
        this, SLOT(errorOccurred(QSerialPort::SerialPortError)));

    connect(
        &m_serial, SIGNAL(readyRead()),
        this, SLOT(performRead()));
}

SerialSocket::~SerialSocket() = default;

bool SerialSocket::startImpl()
{
    m_serial.setPortName(m_name);
    if (!m_serial.open(QSerialPort::ReadWrite)) {
        static const QString FailedToOpenError(
            tr("Failed to open serial port."));
        reportError(FailedToOpenError);
        return false;
    }

    m_serial.setBaudRate(m_baud);
    m_serial.setDataBits(m_dataBits);
    m_serial.setParity(m_parity);
    m_serial.setStopBits(m_stopBits);
    m_serial.setFlowControl(m_flowControl);

    return true;
}

void SerialSocket::stopImpl()
{
    m_serial.close();
}

void SerialSocket::sendDataImpl(DataInfoPtr dataPtr)
{
    assert(dataPtr);
    m_serial.write(
        reinterpret_cast<const char*>(&dataPtr->m_data[0]),
        dataPtr->m_data.size());
}

void SerialSocket::performRead()
{
    assert(sender() == &m_serial);

    auto dataPtr = makeDataInfo();
    dataPtr->m_timestamp = DataInfo::TimestampClock::now();

    auto dataSize = m_serial.bytesAvailable();
    dataPtr->m_data.resize(dataSize);
    auto result =
        m_serial.read(reinterpret_cast<char*>(&dataPtr->m_data[0]), dataSize);
    if (result != dataSize) {
        dataPtr->m_data.resize(result);
    }

    reportDataReceived(std::move(dataPtr));
}

void SerialSocket::errorOccurred(QSerialPort::SerialPortError err)
{
    if (err == QSerialPort::NoError) {
        return;
    }

    static_cast<void>(err);
    reportError(m_serial.errorString());
    m_serial.clearError();
}

}  // namespace serial_socket

} // namespace plugin

} // namespace comms_champion
