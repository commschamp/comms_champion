//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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
#include <iostream>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtNetwork/QHostAddress>
CC_ENABLE_WARNINGS()

#include "Socket.h"

namespace comms_champion
{

namespace plugin
{

namespace udp_socket
{

namespace client
{

namespace
{

const QString DefaultHost("127.0.0.1");
const QString DefaultBroadcastPropName("broadcast");
const QString FromPropName("udp.from");
const QString ToPropName("udp.to");

}  // namespace


Socket::Socket()
  : m_host(DefaultHost),
    m_broadcastPropName(DefaultBroadcastPropName)
{
    connect(
        &m_socket, SIGNAL(disconnected()),
        this, SLOT(socketDisconnected()));
    connect(
        &m_socket, SIGNAL(readyRead()),
        this, SLOT(readFromSocket()));
    connect(
        &m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(socketErrorOccurred(QAbstractSocket::SocketError)));
    connect(
        &m_broadcastSocket, SIGNAL(readyRead()),
        this, SLOT(readFromBroadcastSocket()));
    connect(
        &m_broadcastSocket, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(socketErrorOccurred(QAbstractSocket::SocketError)));
}

Socket::~Socket() noexcept
{
    m_socket.blockSignals(true);
}

bool Socket::socketConnectImpl()
{
    if ((!m_host.isEmpty()) && (m_port == 0)) {
        static const QString Error =
            "Remote port must be greater than 0.";
        reportError(Error);
        return false;
    }

    if ((m_host.isEmpty()) && (m_localPort == 0)) {
        static const QString Error =
            "If no remote host specified, provide a local port information.";
        reportError(Error);
        return false;
    }

    assert(!m_socket.isOpen());
    assert(!m_broadcastSocket.isOpen());
    m_running = true;

    do {
        if (m_localPort == 0) {
            break;
        }

        if (!bindSocket(m_socket)) {
            reportError("Failed to bind UDP socket to port " + QString("%1").arg(m_localPort));
        }

        if (!bindSocket(m_broadcastSocket)) {
            reportError("Failed to bind broadcast UDP socket to port " + QString("%1").arg(m_localPort));
        }

    } while (false);

    do {
        if (m_host.isEmpty()) {
            break;
        }

        m_socket.connectToHost(m_host, m_port);
        if (!m_socket.waitForConnected()) {
            reportError("Failed to connect UDP socket to " + QString("%1:%2").arg(m_host).arg(m_port));
        }
    } while (false);
    return true;
}

void Socket::socketDisconnectImpl()
{
    m_socket.blockSignals(true);
    m_socket.close();
    m_broadcastSocket.close();
    m_running = false;
    m_socket.blockSignals(false);
}

void Socket::sendDataImpl(DataInfoPtr dataPtr)
{
    assert(dataPtr);
    QString from =
        m_socket.localAddress().toString() + ':' +
                    QString("%1").arg(m_socket.localPort());
    dataPtr->m_extraProperties.insert(FromPropName, from);

    do {
        if ((!dataPtr->m_extraProperties.contains(m_broadcastPropName)) ||
            (!m_broadcastSocket.isOpen()) ||
            (m_port == 0)) {
            break;
        }

        std::size_t writtenCount = 0;
        while (writtenCount < dataPtr->m_data.size()) {
            auto remSize = dataPtr->m_data.size() - writtenCount;
            auto count =
                m_broadcastSocket.writeDatagram(
                    reinterpret_cast<const char*>(&dataPtr->m_data[writtenCount]),
                    remSize,
                    QHostAddress::Broadcast,
                    m_port);

            if (count < 0) {
                return;
            }

            writtenCount += count;
        }

        QString to =
            QHostAddress(QHostAddress::Broadcast).toString() + ':' +
                        QString("%1").arg(m_port);

        dataPtr->m_extraProperties.insert(ToPropName, to);
        return;
    } while (false);

    if (!m_socket.isOpen()) {
        return;
    }

    std::size_t writtenCount = 0;
    while (writtenCount < dataPtr->m_data.size()) {
        auto remSize = dataPtr->m_data.size() - writtenCount;
        auto count =
            m_socket.write(
                reinterpret_cast<const char*>(&dataPtr->m_data[writtenCount]),
                remSize);
        if (count < 0) {
            return;
        }

        writtenCount += count;
    }

    QString to =
        m_socket.peerAddress().toString() + ':' +
                    QString("%1").arg(m_socket.peerPort());

    dataPtr->m_extraProperties.insert(ToPropName, to);

}

void Socket::socketDisconnected()
{
    reportDisconnected();
}

void Socket::readFromSocket()
{
    readData(m_socket);
}

void Socket::readFromBroadcastSocket()
{
    readData(m_broadcastSocket);
}

void Socket::socketErrorOccurred(QAbstractSocket::SocketError err)
{
    static_cast<void>(err);
    std::cout << "ERROR: UDP Socket: " << m_socket.errorString().toStdString() << std::endl;
}

void Socket::readData(QUdpSocket& socket)
{
    while (socket.hasPendingDatagrams()) {
        QHostAddress senderAddress;
        quint16 senderPort;

        auto dataPtr = makeDataInfo();
        dataPtr->m_timestamp = DataInfo::TimestampClock::now();
        dataPtr->m_data.resize(socket.pendingDatagramSize());
        socket.readDatagram(
            reinterpret_cast<char*>(&dataPtr->m_data[0]),
            dataPtr->m_data.size(),
            &senderAddress,
            &senderPort);

        QString from =
            senderAddress.toString() + ':' +
                        QString("%1").arg(senderPort);
        QString to =
            m_socket.localAddress().toString() + ':' +
                        QString("%1").arg(m_socket.localPort());

        dataPtr->m_extraProperties.insert(FromPropName, from);
        dataPtr->m_extraProperties.insert(ToPropName, to);
        reportDataReceived(std::move(dataPtr));

        if (m_socket.state() != QUdpSocket::ConnectedState) {
            m_socket.connectToHost(senderAddress, senderPort);
            m_socket.waitForConnected();
            assert(m_socket.isOpen());
            assert(m_socket.state() == QUdpSocket::ConnectedState);
        }
    }
}

bool Socket::bindSocket(QUdpSocket& socket)
{
    if (!socket.bind(QHostAddress::AnyIPv4, m_localPort, QUdpSocket::ShareAddress)) {
        return false;
    }

    return socket.open(QUdpSocket::ReadWrite);
}

}  // namespace client

}  // namespace udp_socket

} // namespace plugin

} // namespace comms_champion
