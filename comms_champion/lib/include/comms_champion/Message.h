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

/// @brief Main interface class used by <b>CommsChampion Tools</b>
///     to display and manipulate messages.
class CC_API Message : public QObject
{
    using Base = QObject;
public:
    /// @brief Type for sequence of raw bytes
    using DataSeq = std::vector<std::uint8_t>;

    /// @brief Type of the message
    enum class Type {
        Invalid, ///< Invalid type
        Received, ///< Message has been received
        Sent, ///< Message has been sent
        NumOfValues ///< Number of available values
    };

    /// @brief Constructor
    Message() = default;

    /// @brief Copy constructor
    Message(const Message&) = default;

    /// @brief Move constructor
    Message(Message&) = default;

    /// @brief Destructor
    /// @details virtual to allow polymorphic destruction
    virtual ~Message();

    /// @brief Copy assignment
    Message& operator=(const Message&) = default;

    /// @brief Move assignment
    Message& operator=(Message&&) = default;

    /// @brief Get message name
    /// @details Invokes virtual nameImpl().
    const char* name() const;

    /// @brief Get properties describing message fields
    /// @details Invokes fieldsPropertiesImpl()
    const QVariantList& fieldsProperties() const;

    /// @brief Dispatch message to message handler used by <b>CommsChampion Tools</b>
    /// @details Invokes dispatchImpl()
    void dispatch(MessageHandler& handler);

    /// @brief Refresh message contents
    /// @details Needs to be invoked by the <b>CommsChampion Tools</b> when
    ///     some field get modified. Invokes refreshMsgImpl().
    /// @return true, if message contentes where refreshed (other fields changed).
    bool refreshMsg();

    /// @brief Get string representation of message ID.
    /// @details Invokes idAsStringImpl().
    QString idAsString() const;

    /// @brief Reset message contents to default constructed values
    void reset();

    /// @brief Assign contents of other message to this ones
    /// @return true in case the messages are of the same type and the assignement
    ///     is successful, false othewise. Invokes assignImpl().
    bool assign(const Message& other);

    /// @brief Check the message contents are valid.
    /// @details Invokes isValidImpl().
    bool isValid() const;

    /// @brief Encode (or serialise) message contents
    /// @details Invokes encodeDataImpl().
    DataSeq encodeData() const;

    /// @brief Decode (or deserialise) message contents
    /// @details Invokes decodeDataImpl().
    bool decodeData(const DataSeq& data);

protected:

    /// @brief Polymorphic name retrieval functionality.
    /// @details Invoked by name().
    virtual const char* nameImpl() const = 0;

    /// @brief Polymorphic fields properties retrieval function.
    /// @details Default implementation returns empty list, should be
    ///     overriden by the derived classes if message contains at least
    ///     one field. Invoked by fieldsProperties().
    virtual const QVariantList& fieldsPropertiesImpl() const;

    /// @brief Polymorphic dispatch functionality.
    /// @details Invoked by dispatch().
    virtual void dispatchImpl(MessageHandler& handler) = 0;

    /// @brief Polymorphic refresh functionality.
    /// @details Invoked by refresh().
    virtual bool refreshMsgImpl() = 0;

    /// @brief Polymophic functionality to get string representation of message ID.
    /// @details Invoked by idAsString().
    virtual QString idAsStringImpl() const = 0;

    /// @brief Polymorphic reset functionality.
    /// @details Invoked by reset().
    virtual void resetImpl() = 0;

    /// @brief Polymophic assignment functionality.
    /// @details Invoked by assign().
    virtual bool assignImpl(const Message& other) = 0;

    /// @brief Polymorphic validity check functionality.
    /// @details Invoked by isValid().
    virtual bool isValidImpl() const = 0;

    /// @brief Polymorphic serialisation functionality.
    /// @details Invoked by encodeData().
    virtual DataSeq encodeDataImpl() const = 0;

    /// @brief Polymorphic deserialisation functionality.
    /// @details Invoked by decodeData().
    virtual bool decodeDataImpl(const DataSeq& data) = 0;
};

/// @brief Smart pointer to @ref Message
using MessagePtr = std::shared_ptr<Message>;

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::MessagePtr);
Q_DECLARE_METATYPE(comms_champion::Message::DataSeq);
