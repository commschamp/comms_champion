//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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
#include <vector>

#include <QtCore/QObject>

#include "comms_champion/Message.h"
#include "comms_champion/Protocol.h"
#include "comms_champion/ProtocolPlugin.h"
#include "comms_champion/MessageInfo.h"

namespace comms_champion
{

class MsgMgr : public QObject
{
    Q_OBJECT

    typedef QObject Base;
public:
    typedef ProtocolPlugin::ProtocolPtr ProtocolPtr;
    typedef unsigned long long MsgNumberType;

    static MsgMgr* instance();
    static MsgMgr& instanceRef();

    void addProtocol(ProtocolPtr&& protocol);
    void setRecvEnabled(bool enabled);

public slots:
    void timeout();

signals:
    void sigMsgReceived(MessageInfoPtr msgInfo);

private:
    typedef Protocol::MsgPtr MsgPtr;
    typedef Protocol::ReadIterType ReadIterType;

    MsgMgr(QObject* parent = nullptr);

    std::vector<MessageInfoPtr> m_recvMsgs;
    bool m_recvEnabled = false;

    typedef std::list<ProtocolPtr> ProtocolStack;
    ProtocolStack m_protStack;
    MsgNumberType m_nextMsgNum = 1;
};

}  // namespace comms_champion
