//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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


#include "MsgMgrImpl.h"

#include <cassert>
#include <algorithm>
#include <iterator>
#include <iostream>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariant>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

namespace
{

void updateMsgTimestamp(MessageInfo& msgInfo, const DataInfo::Timestamp& timestamp)
{
    auto sinceEpoch = timestamp.time_since_epoch();
    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch);
    msgInfo.setTimestamp(milliseconds.count());
}

}  // namespace

MsgMgrImpl::MsgMgrImpl()
{
    m_allMsgs.reserve(1024);
}

MsgMgrImpl::~MsgMgrImpl() = default;

void MsgMgrImpl::start()
{
    if (m_running) {
        assert(!"Already running");
        return;
    }

    if (m_socket) {
        m_socket->start();
    }

    m_running = true;
}

void MsgMgrImpl::stop()
{
    if (!m_running) {
        assert(!"Already stopped.");
        return;
    }

    if (m_socket) {
        m_socket->stop();
    }

    m_running = false;
}

void MsgMgrImpl::clear()
{
    if (m_running) {
        assert(!"Still running");
        stop();
    }

    m_socket.reset();
    m_protocol.reset();
}

ProtocolPtr MsgMgrImpl::getProtocol() const
{
    return m_protocol;
}

void MsgMgrImpl::setRecvEnabled(bool enabled)
{
    m_recvEnabled = enabled;
}

void MsgMgrImpl::deleteMsg(MessageInfoPtr msgInfo)
{
    assert(!m_allMsgs.empty());
    assert(msgInfo);

    auto msgNum = msgInfo->getMsgNum();

    auto iter = std::lower_bound(
        m_allMsgs.begin(),
        m_allMsgs.end(),
        msgNum,
        [](const MessageInfoPtr& msgInfoTmp, MsgNumberType val) -> bool
        {
            return msgInfoTmp->getMsgNum() < val;
        });

    if (iter == m_allMsgs.end()) {
        assert(!"Deleting non existing message.");
        return;
    }

    m_allMsgs.erase(iter);
}

void MsgMgrImpl::deleteAllMsgs()
{
    m_allMsgs.clear();
}

void MsgMgrImpl::sendMsgs(MsgInfosList&& msgs)
{
    if (msgs.empty() || (!m_socket) || (!m_protocol)) {
        return;
    }

    auto dataInfos = m_protocol->write(msgs);
    auto now = DataInfo::TimestampClock::now();

    // TODO: write to the last filter instead of socket
    for (auto& dInfo: dataInfos) {
        dInfo->m_timestamp = now;
        m_socket->sendData(std::move(dInfo));
    }

    m_allMsgs.reserve(m_allMsgs.size() + msgs.size());
    for (auto& msgInfo : msgs) {
        assert(msgInfo);
        updateInternalId(*msgInfo);
        msgInfo->setMsgType(MsgType::Sent);
        updateMsgTimestamp(*msgInfo, now);
        m_allMsgs.push_back(msgInfo);
        reportMsgAdded(msgInfo);
    }
}

const MsgMgrImpl::MsgsList& MsgMgrImpl::getAllMsgs() const
{
    return m_allMsgs;
}

void MsgMgrImpl::setSocket(SocketPtr socket)
{
    if (!socket) {
        m_socket.reset();
        return;
    }

    socket->setDataReceivedCallback(
        [this](DataInfoPtr dataPtr)
        {
            socketDataReceived(std::move(dataPtr));
        });

    socket->setErrorReportCallback(
        [this](const QString& msg)
        {
            reportError(msg);
        });

    m_socket = std::move(socket);
}

//void MsgMgrImpl::removeSocket(SocketPtr socket)
//{
//    auto iter = std::find(m_sockets.begin(), m_sockets.end(), socket);
//    if (iter == m_sockets.end()) {
//        assert(!"Removing sockets that wasn't added before");
//        return;
//    }
//
//    do {
//        socket->disconnect();
//        bool prevExists = (iter != m_sockets.begin());
//        auto prevIter = iter;
//        std::advance(prevIter, -1);
//
//        auto nextIter = iter;
//        std::advance(nextIter, 1);
//        bool nextExists = (nextIter != m_sockets.end());
//
//        if (prevExists) {
//            disconnect(
//                (*prevIter).get(), SIGNAL(sigDataReceived(DataInfoPtr)),
//                socket.get(), SLOT(feedInData(DataInfoPtr)));
//        }
//
//        if (nextExists) {
//            disconnect(
//                (*nextIter).get(), SIGNAL(sigDataToSend(DataInfoPtr)),
//                socket.get(), SLOT(sendData(DataInfoPtr)));
//        }
//
//        if (prevExists && nextExists) {
//            connect(
//                (*prevIter).get(), SIGNAL(sigDataReceived(DataInfoPtr)),
//                (*nextIter).get(), SLOT(feedInData(DataInfoPtr)));
//
//            connect(
//                (*nextIter).get(), SIGNAL(sigDataToSend(DataInfoPtr)),
//                (*prevIter).get(), SLOT(sendData(DataInfoPtr)));
//
//            break;
//        }
//
//        if (prevExists) {
//            // Next doesn't exist
//            connect(
//                (*prevIter).get(), SIGNAL(sigDataReceived(DataInfoPtr)),
//                this, SLOT(socketDataReceived(DataInfoPtr)));
//            break;
//        }
//
//    } while (false);
//
//    m_sockets.erase(iter);
//}

void MsgMgrImpl::setProtocol(ProtocolPtr protocol)
{
    m_protocol = std::move(protocol);
}

void MsgMgrImpl::socketDataReceived(DataInfoPtr dataInfoPtr)
{
    if ((!m_recvEnabled) || !(m_protocol)) {
        return;
    }

    assert(dataInfoPtr);
    auto msgsList = m_protocol->read(*dataInfoPtr);
    if (msgsList.empty()) {
        return;
    }

    for (auto& msgInfo : msgsList) {
        assert(msgInfo);
        updateInternalId(*msgInfo);
        msgInfo->setMsgType(MsgType::Received);

        static const DataInfo::Timestamp DefaultTimestamp;
        if (dataInfoPtr->m_timestamp != DefaultTimestamp) {
            updateMsgTimestamp(*msgInfo, dataInfoPtr->m_timestamp);
        }
        else {
            auto now = DataInfo::TimestampClock::now();
            updateMsgTimestamp(*msgInfo, now);
        }

        reportMsgAdded(msgInfo);
    }

    m_allMsgs.reserve(m_allMsgs.size() + msgsList.size());
    std::move(msgsList.begin(), msgsList.end(), std::back_inserter(m_allMsgs));
}

void MsgMgrImpl::updateInternalId(MessageInfo& msgInfo)
{
    msgInfo.setMsgNum(m_nextMsgNum);
    ++m_nextMsgNum;
    assert(0 < m_nextMsgNum); // wrap around is not supported
}

void MsgMgrImpl::reportMsgAdded(MessageInfoPtr msgInfo)
{
    if (m_msgAddedCallback) {
        m_msgAddedCallback(std::move(msgInfo));
    }
}

void MsgMgrImpl::reportError(const QString& error)
{
    if (m_errorReportCallback) {
        m_errorReportCallback(error);
    }
}

}  // namespace comms_champion

