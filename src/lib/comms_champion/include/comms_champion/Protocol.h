//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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
#include <string>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QMetaType>
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "Api.h"
#include "Message.h"
#include "ErrorStatus.h"
#include "MessageInfo.h"
#include "DataInfo.h"

namespace comms_champion
{

class CC_API Protocol
{
public:
    typedef std::list<MessageInfoPtr> MessagesList;
    typedef std::list<DataInfoPtr> DataInfosList;

    enum class UpdateStatus
    {
        NoChangeToAppMsg,
        AppMsgWasChanged
    };

    virtual ~Protocol() {}

    const std::string& name() const
    {
        return nameImpl();
    }

    MessagesList read(
        const DataInfo& dataInfo)
    {
        return readImpl(dataInfo);
    }

    DataInfosList write(const MessagesList& msgs)
    {
        return writeImpl(msgs);
    }

    MessagesList createAllMessages()
    {
        return createAllMessagesImpl();
    }

    MessageInfoPtr createMessage(const QString& idAsString, unsigned idx = 0)
    {
        return createMessageImpl(idAsString, idx);
    }

    UpdateStatus updateMessageInfo(MessageInfo& msgInfo)
    {
        return updateMessageInfoImpl(msgInfo);
    }

    MessageInfoPtr cloneMessage(const MessageInfo& msgInfo)
    {
        return cloneMessageImpl(msgInfo);
    }

protected:
    virtual const std::string& nameImpl() const = 0;

    virtual MessagesList readImpl(const DataInfo& dataInfo) = 0;

    virtual DataInfosList writeImpl(const MessagesList& msgs) = 0;

    virtual MessagesList createAllMessagesImpl() = 0;

    virtual MessageInfoPtr createMessageImpl(const QString& idAsString, unsigned idx) = 0;

    virtual UpdateStatus updateMessageInfoImpl(MessageInfo& msgInfo) = 0;

    virtual MessageInfoPtr cloneMessageImpl(const MessageInfo& msgInfo) = 0;
};

typedef std::shared_ptr<Protocol> ProtocolPtr;

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::ProtocolPtr);
