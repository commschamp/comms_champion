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

#include <vector>
#include <cstdint>
#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
CC_ENABLE_WARNINGS()

#include "Api.h"

namespace comms_champion
{

class MessageHandler;
class MessageWidget;
class CC_API Message : public QObject
{
    typedef QObject Base;
public:
    typedef std::vector<std::uint8_t> DataSeq;

    enum class Type {
        Invalid,
        Received,
        Sent,
        NumOfValues // Must be last
    };

    Message() = default;
    Message(const Message&) = default;
    virtual ~Message();

    Message& operator=(const Message&) = default;

    const char* name() const;
    const QVariantList& fieldsProperties() const;
    void dispatch(MessageHandler& handler);
    bool refreshMsg();
    QString idAsString() const;
    void reset();
    bool assign(const Message& other);
    bool isValid() const;
    DataSeq encodeData() const;
    bool decodeData(const DataSeq& data);

protected:

    virtual const char* nameImpl() const = 0;
    virtual const QVariantList& fieldsPropertiesImpl() const = 0;
    virtual void dispatchImpl(MessageHandler& handler) = 0;
    virtual bool refreshMsgImpl() = 0;
    virtual QString idAsStringImpl() const = 0;
    virtual void resetImpl() = 0;
    virtual bool assignImpl(const Message& other) = 0;
    virtual bool isValidImpl() const = 0;
    virtual DataSeq encodeDataImpl() const = 0;
    virtual bool decodeDataImpl(const DataSeq& data) = 0;
};

typedef std::shared_ptr<Message> MessagePtr;

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::MessagePtr);
Q_DECLARE_METATYPE(comms_champion::Message::DataSeq);
