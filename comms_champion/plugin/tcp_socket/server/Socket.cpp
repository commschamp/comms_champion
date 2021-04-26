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

#include <cassert>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtNetwork/QHostAddress>
CC_ENABLE_WARNINGS()

#include "Socket.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

namespace server
{

namespace
{

const QString FromPropName("tcp.from");
const QString ToPropName("tcp.to");

}  // namespace

Socket::Socket()
{
    QObject::connect(
        &m_server, SIGNAL(acceptError(QAbstractSocket::SocketError)),
        this, SLOT(acceptErrorOccurred(QAbstractSocket::SocketError)));

    QObject::connect(
        &m_server, SIGNAL(newConnection()),
        this, SLOT(newConnection()));
}

Socket::~Socket() noexcept
{
    for (auto* socket : m_sockets) {
        socket->flush();
    }
}

bool Socket::socketConnectImpl()
{
    if (m_server.isListening()) {
        static constexpr bool Already_listening = false;
        static_cast<void>(Already_listening);
        assert(Already_listening); 
        static const QString AlreadyListeningError(
            tr("Previous run of TCP/IP Server socket wasn't terminated properly."));
        reportError(AlreadyListeningError);
        return false;
    }

    if (!m_server.listen(QHostAddress::Any, m_port)) {
        static const QString FailedToListenError(
            tr("Failed to listen on specified TCP/IP port."));
        reportError(FailedToListenError);
        return false;
    }

    return true;
}

void Socket::socketDisconnectImpl()
{
    m_server.close();
    assert(!m_server.isListening());
}

void Socket::sendDataImpl(DataInfoPtr dataPtr)
{
    assert(dataPtr);

    QVariantList toList;

    for (auto* socketTmp : m_sockets) {
        auto* socket = qobject_cast<QTcpSocket*>(socketTmp);
        assert(socket != nullptr);
        socket->write(
            reinterpret_cast<const char*>(&dataPtr->m_data[0]),
            static_cast<qint64>(dataPtr->m_data.size()));

        QString to =
            socket->peerAddress().toString() + ':' +
                        QString("%1").arg(socket->peerPort());
        toList.append(to);
    }

    QString from =
        m_server.serverAddress().toString() + ':' +
                    QString("%1").arg(m_server.serverPort());

    dataPtr->m_extraProperties.insert(FromPropName, from);
    dataPtr->m_extraProperties.insert(ToPropName, toList);
}

unsigned Socket::connectionPropertiesImpl() const
{
    return ConnectionProperty_Autoconnect;
}

void Socket::newConnection()
{
    auto *newConnSocket = m_server.nextPendingConnection();
    m_sockets.push_back(newConnSocket);
    connect(
        newConnSocket, SIGNAL(disconnected()),
        newConnSocket, SLOT(deleteLater()));
    connect(
        newConnSocket, SIGNAL(disconnected()),
        this, SLOT(connectionTerminated()));
    connect(
        newConnSocket, SIGNAL(readyRead()),
        this, SLOT(readFromSocket()));
    connect(
        newConnSocket, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(socketErrorOccurred(QAbstractSocket::SocketError)));
}

void Socket::connectionTerminated()
{
    auto* socket = sender();
    auto iter = std::find(m_sockets.begin(), m_sockets.end(), socket);
    if (iter == m_sockets.end()) {
        static constexpr bool Must_have_found_socket = false;
        static_cast<void>(Must_have_found_socket);
        assert(Must_have_found_socket);          
        return;
    }

    m_sockets.erase(iter);
}

void Socket::readFromSocket()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    assert(socket != nullptr);

    auto dataPtr = makeDataInfo();
    dataPtr->m_timestamp = DataInfo::TimestampClock::now();

    auto dataSize = socket->bytesAvailable();
    dataPtr->m_data.resize(static_cast<std::size_t>(dataSize));
    auto result =
        socket->read(reinterpret_cast<char*>(&dataPtr->m_data[0]), dataSize);
    if (result != dataSize) {
        dataPtr->m_data.resize(static_cast<std::size_t>(result));
    }

    QString from =
        socket->peerAddress().toString() + ':' +
                    QString("%1").arg(socket->peerPort());
    dataPtr->m_extraProperties.insert(FromPropName, from);

    QString to =
        m_server.serverAddress().toString() + ':' +
                    QString("%1").arg(m_server.serverPort());
    dataPtr->m_extraProperties.insert(ToPropName, to);

    reportDataReceived(std::move(dataPtr));
}

void Socket::socketErrorOccurred(QAbstractSocket::SocketError err)
{
    if (err == QAbstractSocket::RemoteHostClosedError) {
        // Ignore remote client disconnection
        return;
    }

    static_cast<void>(err);
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    assert(socket != nullptr);

    reportError(socket->errorString());
}

void Socket::acceptErrorOccurred(QAbstractSocket::SocketError err)
{
    static_cast<void>(err);
    reportError(m_server.errorString());
    if (!m_server.isListening()) {
        reportDisconnected();
    }
}

}  // namespace server

}  // namespace tcp_socket

} // namespace plugin

} // namespace comms_champion
