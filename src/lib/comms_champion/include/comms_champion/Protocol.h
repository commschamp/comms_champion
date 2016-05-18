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


#pragma once

#include <memory>
#include <cstdint>
#include <cstddef>
#include <list>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QMetaType>
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "Api.h"
#include "Message.h"
#include "ErrorStatus.h"
#include "DataInfo.h"

namespace comms_champion
{

class CC_API Protocol
{
public:
    typedef std::list<MessagePtr> MessagesList;
    typedef std::list<DataInfoPtr> DataInfosList;

    enum class UpdateStatus
    {
        NoChange,
        Changed
    };

    virtual ~Protocol();

    const QString& name() const;

    MessagesList read(const DataInfo& dataInfo);

    DataInfosList write(const MessagesList& msgs);

    MessagesList createAllMessages();

    MessagePtr createMessage(const QString& idAsString, unsigned idx = 0);

    UpdateStatus updateMessage(Message& msg);

    MessagePtr cloneMessage(const Message& msg);

    MessagePtr createInvalidMessage();

    MessagePtr createRawDataMessage();

protected:
    virtual const QString& nameImpl() const = 0;

    virtual MessagesList readImpl(const DataInfo& dataInfo) = 0;

    virtual DataInfosList writeImpl(const MessagesList& msgs) = 0;

    virtual MessagesList createAllMessagesImpl() = 0;

    virtual MessagePtr createMessageImpl(const QString& idAsString, unsigned idx) = 0;

    virtual UpdateStatus updateMessageImpl(Message& msg) = 0;

    virtual MessagePtr cloneMessageImpl(const Message& msg) = 0;

    virtual MessagePtr createInvalidMessageImpl() = 0;

    virtual MessagePtr createRawDataMessageImpl() = 0;

    void setNameToMessageProperties(Message& msg);
    static void setTransportToMessageProperties(MessagePtr transportMsg, Message& msg);
    static void setRawDataToMessageProperties(MessagePtr rawDataMsg, Message& msg);
};

typedef std::shared_ptr<Protocol> ProtocolPtr;

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::ProtocolPtr);
