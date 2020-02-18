//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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


#include "MsgMgrImpl.h"

#include <cassert>
#include <algorithm>
#include <iterator>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariant>
CC_ENABLE_WARNINGS()

#include "comms/util/ScopeGuard.h"
#include "comms_champion/property/message.h"

namespace comms_champion
{

namespace
{

class SeqNumber : public property::message::PropBase<unsigned long long>
{
    typedef property::message::PropBase<unsigned long long> Base;
public:
    SeqNumber() : Base(Name, PropName) {};

private:
    static const QString Name;
    static const QByteArray PropName;
};

const QString SeqNumber::Name("cc.msg_num");
const QByteArray SeqNumber::PropName = SeqNumber::Name.toUtf8();

void updateMsgTimestamp(Message& msg, const DataInfo::Timestamp& timestamp)
{
    auto sinceEpoch = timestamp.time_since_epoch();
    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(sinceEpoch);
    property::message::Timestamp().setTo(milliseconds.count(), msg);
}

}  // namespace

MsgMgrImpl::MsgMgrImpl()
{
    m_allMsgs.reserve(1024);
}

MsgMgrImpl::~MsgMgrImpl() noexcept = default;

void MsgMgrImpl::start()
{
    if (m_running) {
        assert(!"Already running");
        return;
    }

    if (m_socket) {
        m_socket->start();
    }

    for (auto& f : m_filters) {
        f->start();
    }

    m_running = true;
}

void MsgMgrImpl::stop()
{
    if (!m_running) {
        assert(!"Already stopped.");
        return;
    }

    for (auto& f : m_filters) {
        f->stop();
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
    m_filters.clear();
}

SocketPtr MsgMgrImpl::getSocket() const
{
    return m_socket;
}

ProtocolPtr MsgMgrImpl::getProtocol() const
{
    return m_protocol;
}

void MsgMgrImpl::setRecvEnabled(bool enabled)
{
    m_recvEnabled = enabled;
}

void MsgMgrImpl::deleteMsg(MessagePtr msg)
{
    assert(!m_allMsgs.empty());
    assert(msg);

    auto msgNum = SeqNumber().getFrom(*msg);

    auto iter = std::lower_bound(
        m_allMsgs.begin(),
        m_allMsgs.end(),
        msgNum,
        [](const MessagePtr& msgTmp, MsgNumberType val) -> bool
        {
            return SeqNumber().getFrom(*msgTmp) < val;
        });

    if (iter == m_allMsgs.end()) {
        assert(!"Deleting non existing message.");
        return;
    }

    assert(msg.get() == iter->get()); // Make sure that the right message is found
    m_allMsgs.erase(iter);
}

void MsgMgrImpl::sendMsgs(MessagesList&& msgs)
{
    if (msgs.empty() || (!m_socket) || (!m_protocol)) {
        return;
    }

    for (auto& msgPtr : msgs) {
        if (!msgPtr) {
            continue;
        }

        auto updateMsgGuard =
            comms::util::makeScopeGuard(
                [this, &msgPtr]()
                {
                    updateInternalId(*msgPtr);
                    property::message::Type().setTo(MsgType::Sent, *msgPtr);
                    auto now = DataInfo::TimestampClock::now();
                    updateMsgTimestamp(*msgPtr, now);
                    m_allMsgs.push_back(msgPtr);
                    reportMsgAdded(msgPtr);
                });

        auto dataInfoPtr = m_protocol->write(*msgPtr);
        if (!dataInfoPtr) {
            continue;
        }

        QList <DataInfoPtr> data;
        data.append(std::move(dataInfoPtr));
        for (auto& filter : m_filters) {
            if (data.isEmpty()) {
                break;
            }

            QList <DataInfoPtr> dataTmp;
            for (auto& d : data) {
                dataTmp.append(filter->sendData(d));
            }

            data.swap(dataTmp);
        }

        if (data.isEmpty()) {
            continue;
        }

        for (auto& d : data) {
            m_socket->sendData(d);

            if (!d->m_extraProperties.isEmpty()) {
                auto map = property::message::ExtraInfo().getFrom(*msgPtr);
                for (auto& key : d->m_extraProperties.keys()) {
                    map.insert(key, d->m_extraProperties.value(key));
                }
                property::message::ExtraInfo().setTo(std::move(map), *msgPtr);
                m_protocol->updateMessage(*msgPtr);
            }
        }
    }
}

void MsgMgrImpl::addMsgs(const MessagesList& msgs, bool reportAdded)
{
    m_allMsgs.reserve(m_allMsgs.size() + msgs.size());

    for (auto& m : msgs) {
        if (!m) {
            assert(!"Invalid message in the list");
            continue;
        }

        if (property::message::Type().getFrom(*m) == MsgType::Invalid) {
            assert(!"Invalid type of the message");
            continue;
        }

        if (property::message::Timestamp().getFrom(*m) == 0) {
            auto now = DataInfo::TimestampClock::now();
            updateMsgTimestamp(*m, now);
        }

        updateInternalId(*m);
        if (reportAdded) {
            reportMsgAdded(m);
        }
        m_allMsgs.push_back(m);
    }
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

    socket->setDisconnectedReportCallback(
        [this]()
        {
            reportSocketDisconnected();
        });

    m_socket = std::move(socket);
}

void MsgMgrImpl::setProtocol(ProtocolPtr protocol)
{
    m_protocol = std::move(protocol);
}

void MsgMgrImpl::addFilter(FilterPtr filter)
{
    if (!filter) {
        return;
    }

    auto filterIdx = m_filters.size();
    filter->setDataToSendCallback(
        [this, filterIdx](DataInfoPtr dataPtr)
        {
            if (!dataPtr) {
                return;
            }

            assert(filterIdx < m_filters.size());
            auto revIdx = m_filters.size() - filterIdx;

            QList<DataInfoPtr> data;
            data.append(std::move(dataPtr));
            for (auto iter = m_filters.rbegin() + revIdx; iter != m_filters.rend(); ++iter) {

                if (!data.isEmpty()) {
                    break;
                }

                auto nextFilter = *iter;

                QList<DataInfoPtr> dataTmp;
                for (auto& d : data) {
                    dataTmp.append(nextFilter->sendData(d));
                }

                data.swap(dataTmp);
            }

            if (!m_socket) {
                return;
            }

            for (auto& d : data) {
                m_socket->sendData(std::move(d));
            }
        });

    filter->setErrorReportCallback(
        [this](const QString& msg)
        {
            reportError(msg);
        });

    m_filters.push_back(std::move(filter));
}

void MsgMgrImpl::socketDataReceived(DataInfoPtr dataInfoPtr)
{
    if ((!m_recvEnabled) || !(m_protocol) || (!dataInfoPtr)) {
        return;
    }

    QList<DataInfoPtr> data;
    data.append(std::move(dataInfoPtr));
    for (auto filt : m_filters) {
        assert(filt);

        if (data.isEmpty()) {
            return;
        }

        QList<DataInfoPtr> dataTmp;
        for (auto& d : data) {
            dataTmp.append(filt->recvData(d));
        }

        data.swap(dataTmp);
    }

    if (data.isEmpty()) {
        return;
    }

    MessagesList msgsList;
    while (!data.isEmpty()) {
        auto nextDataPtr = data.front();
        data.pop_front();

        auto msgs = m_protocol->read(*nextDataPtr);
        msgsList.insert(msgsList.end(), msgs.begin(), msgs.end());
    }

    if (msgsList.empty()) {
        return;
    }

    for (auto& m : msgsList) {
        assert(m);
        updateInternalId(*m);
        property::message::Type().setTo(MsgType::Received, *m);

        static const DataInfo::Timestamp DefaultTimestamp;
        if (dataInfoPtr->m_timestamp != DefaultTimestamp) {
            updateMsgTimestamp(*m, dataInfoPtr->m_timestamp);
        }
        else {
            auto now = DataInfo::TimestampClock::now();
            updateMsgTimestamp(*m, now);
        }

        reportMsgAdded(m);
    }

    m_allMsgs.reserve(m_allMsgs.size() + msgsList.size());
    std::move(msgsList.begin(), msgsList.end(), std::back_inserter(m_allMsgs));
}

void MsgMgrImpl::updateInternalId(Message& msg)
{
    SeqNumber().setTo(m_nextMsgNum, msg);
    ++m_nextMsgNum;
    assert(0 < m_nextMsgNum); // wrap around is not supported
}

void MsgMgrImpl::reportMsgAdded(MessagePtr msg)
{
    if (m_msgAddedCallback) {
        m_msgAddedCallback(std::move(msg));
    }
}

void MsgMgrImpl::reportError(const QString& error)
{
    if (m_errorReportCallback) {
        m_errorReportCallback(error);
    }
}

void MsgMgrImpl::reportSocketDisconnected()
{
    if (m_socketDisconnectReportCallback) {
        m_socketDisconnectReportCallback();
    }
}

}  // namespace comms_champion

