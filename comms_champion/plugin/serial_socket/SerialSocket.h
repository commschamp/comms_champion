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

#pragma once

#include <list>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtSerialPort/QSerialPort>
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "comms_champion/Socket.h"


namespace comms_champion
{

namespace plugin
{

namespace serial_socket
{

class SerialSocket : public QObject,
                     public comms_champion::Socket
{
    Q_OBJECT
    using Base = comms_champion::Socket;

public:

    typedef qint32 Baud;
    typedef QSerialPort::DataBits DataBits;
    typedef QSerialPort::Parity Parity;
    typedef QSerialPort::StopBits StopBits;
    typedef QSerialPort::FlowControl FlowControl;

    SerialSocket();
    ~SerialSocket();

    QString& name()
    {
        return m_name;
    }

    Baud& baud()
    {
        return m_baud;
    }

    DataBits& dataBits()
    {
        return m_dataBits;
    }

    Parity& parity()
    {
        return m_parity;
    }

    StopBits& stopBits()
    {
        return m_stopBits;
    }

    FlowControl& flowControl()
    {
        return m_flowControl;
    }

protected:
    virtual bool socketConnectImpl() override;
    virtual void socketDisconnectImpl() override;
    virtual void sendDataImpl(DataInfoPtr dataPtr) override;

private slots:
    void performRead();
    void errorOccurred(QSerialPort::SerialPortError err);

private:
    QSerialPort m_serial;
    QString m_name;
    Baud m_baud = 115200;
    DataBits m_dataBits = DataBits::Data8;
    Parity m_parity = Parity::NoParity;
    StopBits m_stopBits = StopBits::OneStop;
    FlowControl m_flowControl = FlowControl::NoFlowControl;
};

}  // namespace serial_socket

} // namespace plugin

} // namespace comms_champion
