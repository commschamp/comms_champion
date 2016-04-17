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

#include <map>
#include <memory>
#include <string>
#include <list>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QMetaType>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "Api.h"
#include "Message.h"

namespace comms_champion
{

class CC_API MessageInfo
{
public:

    using  MessagePtr = std::shared_ptr<Message>;

    MessageInfo();
    ~MessageInfo();

    MessageInfo(const MessageInfo&) = delete;
    MessageInfo(MessageInfo&&) = delete;
    MessageInfo& operator=(const MessageInfo&) = delete;
    MessageInfo& operator=(MessageInfo&&) = delete;

    MessagePtr getAppMessage() const;
    void setAppMessage(MessagePtr msg);

    MessagePtr getTransportMessage() const;
    void setTransportMessage(MessagePtr msg);

    MessagePtr getRawDataMessage() const;
    void setRawDataMessage(MessagePtr msg);

    QString getProtocolName() const;
    void setProtocolName(const QString& value);

    unsigned long long getDelay() const;
    void setDelay(unsigned long long value);

    QString getDelayUnits() const;
    void setDelayUnits(const QString& value);

    unsigned long long getRepeatDuration() const;
    void setRepeatDuration(unsigned long long value);

    QString getRepeatDurationUnits() const;
    void setRepeatDurationUnits(const QString& value);

    unsigned long long getRepeatCount() const;
    void setRepeatCount(unsigned long long value);

    QVariant getExtraProperty(const QString& name) const;
    bool setExtraProperty(const QString& name, QVariant&& value);

    const QVariantMap& getAllProperties() const;
    void setAllProperties(const QVariantMap& props);
    void setAllProperties(QVariantMap&& props);

private:
    MessagePtr m_appMsg;
    MessagePtr m_transportMsg;
    MessagePtr m_rawDataMsg;
    QVariantMap m_props;
};

using MessageInfoPtr = std::shared_ptr<MessageInfo>;

using MsgInfosList = std::list<MessageInfoPtr>;

inline
CC_API MessageInfoPtr makeMessageInfo()
{
    return MessageInfoPtr(new MessageInfo());
}

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::MessageInfo::MessagePtr);
Q_DECLARE_METATYPE(comms_champion::MessageInfoPtr);


