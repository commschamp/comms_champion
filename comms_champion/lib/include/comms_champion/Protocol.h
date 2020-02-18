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

/// @brief Main polymorphic interface class for protocols.
/// @details It is used by @b CommsChampion @b Tools to create and manipulate
///     protocol messages.
/// @headerfile comms_champion/Protocol.h

class CC_API Protocol
{
public:
    /// @brief List of messages
    using MessagesList = std::list<MessagePtr>;

    /// @brief List of raw data buffers
    using DataInfosList = std::list<DataInfoPtr>;

    /// @brief Type used to contain raw bytes seqence
    using MsgDataSeq = Message::DataSeq ;

    /// @brief Status of message "update" operation.
    enum class UpdateStatus
    {
        NoChange, ///< The message contents haven't been changed
        Changed ///< The message contents have been changed
    };

    /// @brief Destructor
    virtual ~Protocol() noexcept;

    /// @brief Retrieve name of the protocol.
    /// @details Invokes virtual nameImpl().
    const QString& name() const;

    /// @brief Read the received data input.
    /// @details Invokes virtual readImpl().
    /// @param[in] dataInfo Received data information
    /// @param[in] final Final input indication, if @b true no more data is expected
    /// @return List of created messages
    MessagesList read(const DataInfo& dataInfo, bool final = false);

    /// @brief Serialse message.
    /// @details Invokes writeImpl().
    /// @param[in] msg Reference to message object, passed by non-const reference
    ///     to allow update of the message properties.
    /// @return Serialised data.
    DataInfoPtr write(Message& msg);

    /// @brief Create all messages supported by the protocol.
    /// @details Invokes createAllMessagesImpl().
    MessagesList createAllMessages();

    /// @brief Create message object given string representation of the message ID.
    /// @details Invokes createMessageImpl().
    /// @param[in] idAsString String representation of the message ID.
    /// @param[in] idx Index of the message type within the range of message types
    ///     with the same ID.
    MessagePtr createMessage(const QString& idAsString, unsigned idx = 0);

    /// @brief Update (or refresh) message contents
    /// @details Invokes updateMessageImpl().
    /// @return Status of the update.
    UpdateStatus updateMessage(Message& msg);

    /// @brief Clone the message object
    /// @details Invokes cloneMessageImpl().
    /// @return Pointer to newly created message with the same contents
    MessagePtr cloneMessage(const Message& msg);

    /// @brief Create dummy message containing invalid input
    /// @brief Invokes createInvalidMessageImpl().
    MessagePtr createInvalidMessage(const MsgDataSeq& data);

protected:
    /// @brief Polymorphic protocol name retrieval.
    /// @details Invoked by name().
    virtual const QString& nameImpl() const = 0;

    /// @brief Polymorphic read functionality.
    /// @details Invoked by read().
    virtual MessagesList readImpl(const DataInfo& dataInfo, bool final) = 0;

    /// @brief Polymorphic write functionality.
    /// @details invoked by write().
    virtual DataInfoPtr writeImpl(Message& msg) = 0;

    /// @brief Polymorphic creation of all messages protocol supports.
    /// @details Invoked by createAllMessages().
    virtual MessagesList createAllMessagesImpl() = 0;

    /// @brief Polymorphic message creation functionality.
    /// @details Invoked by createMessage().
    virtual MessagePtr createMessageImpl(const QString& idAsString, unsigned idx) = 0;

    /// @brief Polymorphic message update (refresh) functionality.
    /// @details Invoked by updateMessage().
    virtual UpdateStatus updateMessageImpl(Message& msg) = 0;

    /// @brief Polymorphic message cloning functionality.
    /// @details Invoked by cloneMessage().
    virtual MessagePtr cloneMessageImpl(const Message& msg) = 0;

    /// @brief Polymorphic creation of invalid message representation.
    /// @details Invoked by createInvalidMessage().
    virtual MessagePtr createInvalidMessageImpl() = 0;

    /// @brief Polymorphic creation of message object representing raw data
    virtual MessagePtr createRawDataMessageImpl() = 0;

    /// @brief Polymorphic creation of message object representing extra info.
    virtual MessagePtr createExtraInfoMessageImpl() = 0;

    /// @brief Helper function to assign protocol name to message properties.
    /// @details Expected to be used by the derived class.
    void setNameToMessageProperties(Message& msg);

    /// @brief Helper function to assign "tranport message" object as a property
    ///     of application message object.
    static void setTransportToMessageProperties(MessagePtr transportMsg, Message& msg);

    /// @brief Helper function to assign "raw data message" object as a property
    ///     of application message object.
    static void setRawDataToMessageProperties(MessagePtr rawDataMsg, Message& msg);

    /// @brief Helper function to assign "extra info message" object as a property
    ///     of application message object.
    static void setExtraInfoMsgToMessageProperties(MessagePtr extraInfoMsg, Message& msg);

    /// @brief Helper function to retrieve "extra info message" object from properties
    ///     of the application message object.
    static MessagePtr getExtraInfoMsgToMessageProperties(const Message& msg);

    /// @brief Helper function to retrieve "extra info" from message properties.
    static QVariantMap getExtraInfoFromMessageProperties(const Message& msg);

    /// @brief Helper function to set "extra info" to message properties.
    static void setExtraInfoToMessageProperties(const QVariantMap& extraInfo, Message& msg);

    /// @brief Helper function to merge existing "extra info" and provided one.
    static void mergeExtraInfoToMessageProperties(const QVariantMap& extraInfo, Message& msg);

    /// @brief Helper function to force "extra info" existence.
    static void setForceExtraInfoExistenceToMessageProperties(Message& msg);

    /// @brief Helper function to check whether "extra info" existence is force.
    static bool getForceExtraInfoExistenceFromMessageProperties(const Message& msg);

};

/// @brief Pointer to @ref Protocol object.
using ProtocolPtr = std::shared_ptr<Protocol>;

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::ProtocolPtr);
