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

namespace proxy
{

Socket::Socket()
{
    connect(
        &m_server, SIGNAL(newConnection()),
        this, SLOT(newConnection()));
}

Socket::~Socket()
{
    while (!m_sockets.empty()) {
        removeConnection(m_sockets.begin());
    }
}

bool Socket::startImpl()
{
    if (m_server.isListening()) {
        assert(!"Already listening");
        static const QString AlreadyListeningError(
            tr("Previous run of TCP/IP Server socket wasn't terminated properly."));
        reportError(AlreadyListeningError);
        return false;
    }

    do {
        if (m_port != m_remotePort) {
            break;
        }

        if ((!m_remoteHost.isEmpty()) &&
            (m_remoteHost != "localhost") &&
            (m_remoteHost != "127.0.0.1")) {
            break;
        }

        static const QString PortsErrorError(
            tr("Cannot have the same port for local and remote endpoints."));
        reportError(PortsErrorError);
        return false;
    } while (false);

    if (!m_server.listen(QHostAddress::Any, m_port)) {
        static const QString FailedToListenError(
            tr("Failed to listen on specified TCP/IP port."));
        reportError(FailedToListenError);
        return false;
    }

    return true;
}

void Socket::stopImpl()
{
    m_server.close();
}

void Socket::sendDataImpl(DataInfoPtr dataPtr)
{
    assert(dataPtr);
    for (auto& connectedPair : m_sockets) {
        assert(connectedPair.first != nullptr);
        assert(connectedPair.second);
        connectedPair.first->write(
            reinterpret_cast<const char*>(&dataPtr->m_data[0]),
            dataPtr->m_data.size());
        connectedPair.second->write(
            reinterpret_cast<const char*>(&dataPtr->m_data[0]),
            dataPtr->m_data.size());
    }
}

void Socket::newConnection()
{
    auto *newConnSocket = m_server.nextPendingConnection();
    connect(
        newConnSocket, SIGNAL(disconnected()),
        newConnSocket, SLOT(deleteLater()));
    connect(
        newConnSocket, SIGNAL(disconnected()),
        this, SLOT(clientConnectionTerminated()));
    connect(
        newConnSocket, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(socketErrorOccurred(QAbstractSocket::SocketError)));

    ConnectionSocketPtr connectionSocket(new QTcpSocket);
    connect(
        connectionSocket.get(), SIGNAL(connected()),
        this, SLOT(connectionSocketConnected()));
    connect(
        connectionSocket.get(), SIGNAL(disconnected()),
        this, SLOT(connectionSocketDisconnected()));
    connect(
        connectionSocket.get(), SIGNAL(readyRead()),
        this, SLOT(readFromConnectionSocket()));
    connect(
        connectionSocket.get(), SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(socketErrorOccurred(QAbstractSocket::SocketError)));

    if (m_remoteHost.isEmpty()) {
        m_remoteHost = QHostAddress(QHostAddress::LocalHost).toString();
    }

    connectionSocket->connectToHost(m_remoteHost, m_remotePort);
    m_sockets.emplace_back(newConnSocket, std::move(connectionSocket));
}

void Socket::clientConnectionTerminated()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr) {
        assert(!"Signal from unkown object");
        return;
    }

    auto iter = findByClient(socket);
    if (iter == m_sockets.end()) {
        return;
    }

    removeConnection(iter);
}

void Socket::readFromClientSocket()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    assert(socket != nullptr);

    auto iter = findByClient(socket);
    assert (iter != m_sockets.end());
    assert(iter->second);
    auto& connectionSocket = *(iter->second);

    performReadWrite(*socket, connectionSocket);
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

void Socket::connectionSocketConnected()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr) {
        assert(!"Signal from unkown object");
        return;
    }

    auto iter = findByConnection(socket);
    assert(iter != m_sockets.end());
    assert(iter->first != nullptr);
    connect(
        iter->first, SIGNAL(readyRead()),
        this, SLOT(readFromClientSocket()));
}

void Socket::connectionSocketDisconnected()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr) {
        assert(!"Signal from unkown object");
        return;
    }

    auto iter = findByConnection(socket);
    if (iter == m_sockets.end()) {
        return;
    }

    removeConnection(iter);
}

void Socket::readFromConnectionSocket()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    assert(socket != nullptr);

    auto iter = findByConnection(socket);
    assert (iter != m_sockets.end());
    assert(iter->first != nullptr);
    auto& clientSocket = *(iter->first);
    performReadWrite(*socket, clientSocket);
}

Socket::SocketsList::iterator Socket::findByClient(QTcpSocket* socket)
{
    return std::find_if(
        m_sockets.begin(), m_sockets.end(),
        [socket](const ConnectedPair& elem) -> bool
        {
            return elem.first == socket;
        });
}

Socket::SocketsList::iterator Socket::findByConnection(QTcpSocket* socket)
{
    return std::find_if(
        m_sockets.begin(), m_sockets.end(),
        [socket](const ConnectedPair& elem) -> bool
        {
            return elem.second.get() == socket;
        });

}

void Socket::removeConnection(SocketsList::iterator iter)
{
    assert(iter != m_sockets.end());
    auto* clientSocket = iter->first;
    assert(clientSocket);

    ConnectionSocketPtr connectionSocket(std::move(iter->second));
    assert(connectionSocket);
    assert(!iter->second);

    m_sockets.erase(iter);

    if (clientSocket->state() == QTcpSocket::ConnectedState) {
        clientSocket->disconnectFromHost();
    }
    delete clientSocket;

    if (connectionSocket->state() == QTcpSocket::ConnectedState) {
        connectionSocket->disconnectFromHost();
    }
}

void Socket::performReadWrite(QTcpSocket& readFromSocket, QTcpSocket& writeToSocket)
{
    auto dataPtr = makeDataInfo();
    dataPtr->m_timestamp = DataInfo::TimestampClock::now();

    auto dataSize = readFromSocket.bytesAvailable();
    dataPtr->m_data.resize(dataSize);
    auto result =
        readFromSocket.read(reinterpret_cast<char*>(&dataPtr->m_data[0]), dataSize);
    if (result != dataSize) {
        dataPtr->m_data.resize(result);
    }

    writeToSocket.write(
        reinterpret_cast<const char*>(&dataPtr->m_data[0]),
        dataPtr->m_data.size());

    // TODO: provide origin information
    reportDataReceived(std::move(dataPtr));
}

}  // namespace proxy

}  // namespace tcp_socket

} // namespace plugin

} // namespace comms_champion
