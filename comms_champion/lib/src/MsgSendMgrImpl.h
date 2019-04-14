//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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

#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QTimer>
CC_ENABLE_WARNINGS()

#include "comms_champion/MsgSendMgr.h"
#include "comms_champion/Protocol.h"

namespace comms_champion
{

class MsgSendMgrImpl : public QObject
{
    Q_OBJECT
public:
    typedef MsgSendMgr::MessagesList MessagesList;
    typedef MsgSendMgr::SendMsgsCallbackFunc SendMsgsCallbackFunc;
    typedef MsgSendMgr::SendCompleteCallbackFunc SendCompleteCallbackFunc;

    MsgSendMgrImpl();
    ~MsgSendMgrImpl() noexcept;

    template <typename TFunc>
    void setSendMsgsCallbackFunc(TFunc&& func)
    {
        m_sendCallback = std::forward<TFunc>(func);
    }

    template <typename TFunc>
    void setSendCompleteCallbackFunc(TFunc&& func)
    {
        m_sendCompleteCallback = std::forward<TFunc>(func);
    }

    void start(ProtocolPtr protocol, const MessagesList& msgs);

    void stop();

private slots:
    void sendPendingAndWait();

private:
    SendMsgsCallbackFunc m_sendCallback;
    SendCompleteCallbackFunc m_sendCompleteCallback;
    ProtocolPtr m_protocol;
    MessagesList m_msgsToSend;
    QTimer m_timer;
};

}  // namespace comms_champion
