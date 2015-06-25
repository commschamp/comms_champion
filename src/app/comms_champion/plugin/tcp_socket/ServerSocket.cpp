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
#include <QtNetwork/QHostAddress>

#include "ServerSocket.h"

namespace comms_champion
{

namespace plugin
{

namespace tcp_socket
{

ServerSocket::ServerSocket()
{
    connect(
        &m_server, SIGNAL(newConnection()),
        this, SLOT(newConnection()));
}

ServerSocket::~ServerSocket() = default;

bool ServerSocket::startImpl()
{
    if (m_server.isListening()) {
        assert(!"Already listening");
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

void ServerSocket::stopImpl()
{
    m_server.close();
}

void ServerSocket::sendDataImpl(DataInfoPtr dataPtr)
{
    assert(dataPtr);
    for (auto* socketTmp : m_sockets) {
        auto* socket = qobject_cast<QTcpSocket*>(socketTmp);
        assert(socket != nullptr);
        socket->write(
            reinterpret_cast<const char*>(&dataPtr->m_data[0]),
            dataPtr->m_data.size());
    }
}

void ServerSocket::feedInDataImpl(DataInfoPtr dataPtr)
{
    static_cast<void>(dataPtr);
    assert(!"Expected to be bottom socket, should not be called");
}

void ServerSocket::newConnection()
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

void ServerSocket::connectionTerminated()
{
    auto* socket = sender();
    auto iter = std::find(m_sockets.begin(), m_sockets.end(), socket);
    if (iter == m_sockets.end()) {
        assert(!"Must have found socket");
        return;
    }

    m_sockets.erase(iter);
}

void ServerSocket::readFromSocket()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    assert(socket != nullptr);

    auto dataPtr = makeDataInfo();
    dataPtr->m_timestamp = DataInfo::TimestampClock::now();

    auto dataSize = socket->bytesAvailable();
    dataPtr->m_data.resize(dataSize);
    auto result =
        socket->read(reinterpret_cast<char*>(&dataPtr->m_data[0]), dataSize);
    if (result != dataSize) {
        dataPtr->m_data.resize(result);
    }

    // TODO: provide origin information
    reportDataReceived(std::move(dataPtr));
}

void ServerSocket::socketErrorOccurred(QAbstractSocket::SocketError err)
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

}  // namespace tcp_socket

} // namespace plugin

} // namespace comms_champion
