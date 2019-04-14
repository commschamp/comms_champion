//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include <list>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtNetwork/QTcpSocket>
CC_ENABLE_WARNINGS()

#include "comms_champion/Socket.h"


namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

namespace client
{

class Socket : public QObject,
               public comms_champion::Socket
{
    Q_OBJECT
    using Base = comms_champion::Socket;

public:
    typedef unsigned short PortType;

    Socket();
    ~Socket() noexcept;

    void setHost(const QString& value)
    {
        m_host = value;
    }

    const QString& getHost() const
    {
        return m_host;
    }

    void setPort(PortType value)
    {
        m_port = value;
    }

    PortType getPort() const
    {
        return m_port;
    }


protected:
    virtual bool socketConnectImpl() override;
    virtual void socketDisconnectImpl() override;
    virtual void sendDataImpl(DataInfoPtr dataPtr) override;

private slots:
    void socketDisconnected();
    void readFromSocket();
    void socketErrorOccurred(QAbstractSocket::SocketError err);

private:
    static const PortType DefaultPort = 20000;
    QString m_host;
    PortType m_port = DefaultPort;
    QTcpSocket m_socket;
};

}  // namespace client

}  // namespace tcp_socket

} // namespace plugin

} // namespace comms_champion
