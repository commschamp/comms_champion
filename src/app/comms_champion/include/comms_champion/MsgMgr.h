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

#include "Message.h"
#include "Protocol.h"
#include "ProtocolPlugin.h"

namespace comms_champion
{

class MsgMgr : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    typedef QObject Base;
public:
    typedef ProtocolPlugin::ProtocolPtr ProtocolPtr;

    static MsgMgr* instance();
    static void qmlRegister();

    QString name() const;
    void setName(const QString& name);

    void addProtocol(ProtocolPtr&& protocol);

public slots:
    void timeout();

signals:
    void msgReceived(Message* msg);
    void nameChanged();

private:
    typedef Protocol::MsgPtr MsgPtr;
    typedef Protocol::ReadIterType ReadIterType;

    MsgMgr(QObject* parent = nullptr);
    std::vector<MsgPtr> m_recvMsgs;

    typedef std::list<ProtocolPtr> ProtocolStack;
    ProtocolStack m_protStack;
};

}  // namespace comms_champion
