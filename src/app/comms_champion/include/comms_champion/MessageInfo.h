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

#include <map>
#include <memory>
#include <string>

#include <QtCore/QMetaType>
#include <QtCore/QVariant>

#include "Message.h"

namespace comms_champion
{

class MessageInfo
{
public:

    using  MessagePtr = std::shared_ptr<Message>;

    MessageInfo() = default;
    MessageInfo(const MessageInfo&) = default;
    MessageInfo(MessageInfo&&) = default;
    ~MessageInfo() = default;
    MessageInfo& operator=(const MessageInfo&) = default;
    MessageInfo& operator=(MessageInfo&&) = default;

    MessagePtr getAppMessage() const;
    void setAppMessage(MessagePtr msg);

    MessagePtr getTransportMessage() const;
    void setTransportMessage(MessagePtr msg);

    MessagePtr getRawDataMessage() const;
    void setRawDataMessage(MessagePtr msg);

    std::string getProtocolName() const;
    void setProtocolName(const std::string& value);

    QVariant getExtraProperty(const std::string& property) const;
    bool setExtraProperty(const std::string& property, QVariant&& value);


private:
    MessagePtr getMessage(const std::string& property) const;
    void setMessage(const std::string& property, MessagePtr msg);

    typedef std::map<std::string, QVariant> PropertiesMap;
    PropertiesMap m_map;
};

using MessageInfoPtr = std::shared_ptr<MessageInfo>;

inline
MessageInfoPtr makeMessageInfo()
{
    return MessageInfoPtr(new MessageInfo());
}

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::MessageInfo::MessagePtr);
Q_DECLARE_METATYPE(comms_champion::MessageInfoPtr);


