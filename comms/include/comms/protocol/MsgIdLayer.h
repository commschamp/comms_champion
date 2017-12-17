//
// Copyright 2014 - 2017 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

/// @file comms/protocol/MsgIdLayer.h
/// This file contains "Message ID" protocol layer of the "comms" module.

#pragma once

#include <array>
#include <tuple>
#include <algorithm>
#include <utility>
#include <tuple>
#include <limits>

#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "ProtocolLayerBase.h"
#include "comms/fields.h"
#include "comms/MsgFactory.h"

namespace comms
{

namespace protocol
{

/// @brief Protocol layer that uses uses message ID field as a prefix to all the
///        subsequent data written by other (next) layers.
/// @details The main purpose of this layer is to process the message ID information.
///     Holds instance of comms::MsgFactory as its private member and uses it
///     to create message(s) with the required ID.
/// @tparam TField Field type that contains message ID.
/// @tparam TMessage Interface class for the @b input messages
/// @tparam TAllMessages Types of all @b input messages, bundled in std::tuple,
///     that this protocol stack must be able to read() as well as create (using createMsg()).
/// @tparam TNextLayer Next transport layer type.
/// @tparam TOptions All the options that will be forwarded to definition of
///     message factory type (comms::MsgFactory).
/// @headerfile comms/protocol/MsgIdLayer.h
template <typename TField,
          typename TMessage,
          typename TAllMessages,
          typename TNextLayer,
          typename... TOptions>
class MsgIdLayer : public
        ProtocolLayerBase<
            TField,
            TNextLayer,
            MsgIdLayer<TField, TMessage, TAllMessages, TNextLayer, TOptions...>
        >
{
    static_assert(util::IsTuple<TAllMessages>::Value,
        "TAllMessages must be of std::tuple type");
    using BaseImpl =
        ProtocolLayerBase<
            TField,
            TNextLayer,
            MsgIdLayer<TField, TMessage, TAllMessages, TNextLayer, TOptions...>
        >;

    using Factory = comms::MsgFactory<TMessage, TAllMessages, TOptions...>;

    static_assert(TMessage::InterfaceOptions::HasMsgIdType,
        "Usage of MsgIdLayer requires support for ID type. "
        "Use comms::option::MsgIdType option in message interface type definition.");

public:

    /// @brief All supported message types bundled in std::tuple.
    /// @see comms::MsgFactory::AllMessages.
    using AllMessages = typename Factory::AllMessages;

    /// @brief Type of smart pointer that will hold allocated message object.
    /// @details Same as comms::MsgFactory::MsgPtr.
    using MsgPtr = typename Factory::MsgPtr;

    /// @brief Type of the @b input message interface.
    using Message = TMessage;

    /// @brief Type of message ID
    using MsgIdType = typename Message::MsgIdType;

    /// @brief Type of message ID when passed by the parameter
    using MsgIdParamType = typename Message::MsgIdParamType;

    /// @brief Type of the field object used to read/write message ID value.
    using Field = typename BaseImpl::Field;

    static_assert(
        comms::field::isIntValue<Field>() || comms::field::isEnumValue<Field>() || comms::field::isNoValue<Field>(),
        "Field must be of IntValue or EnumValue types");

    /// @brief Default constructor.
    explicit MsgIdLayer() = default;

    /// @brief Copy constructor.
    MsgIdLayer(const MsgIdLayer&) = default;

    /// @brief Move constructor.
    MsgIdLayer(MsgIdLayer&&) = default;

    /// @brief Copy assignment.
    MsgIdLayer& operator=(const MsgIdLayer&) = default;

    /// @brief Move assignment.
    MsgIdLayer& operator=(MsgIdLayer&&) = default;

    /// @brief Destructor
    ~MsgIdLayer() noexcept = default;

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details The function will read message ID from the data sequence first,
    ///     generate appropriate message object based on the read ID and
    ///     forward the read() request to the next layer.
    ///     If the message object cannot be generated (the message type is not
    ///     provided inside @b TAllMessages template parameter), but
    ///     the @ref comms::option::SupportGenericMessage option has beed used,
    ///     the @ref comms::GenericMessage may be generated instead.
    /// @tparam TIter Type of iterator used for reading.
    /// @tparam TNextLayerReader next layer reader object type.
    /// @param[out] field Field object to read.
    /// @param[in, out] msgPtr Reference to smart pointer that will hold
    ///                 allocated message object
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] missingSize If not nullptr and return value is
    ///             comms::ErrorStatus::NotEnoughData it will contain
    ///             minimal missing data length required for the successful
    ///             read attempt.
    /// @param[in] nextLayerReader Next layer reader object.
    /// @return Status of the operation.
    /// @pre msgPtr doesn't point to any object:
    ///      @code assert(!msgPtr); @endcode
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post Returns comms::ErrorStatus::Success if and only if msgPtr points
    ///       to a valid object.
    /// @post missingSize output value is updated if and only if function
    ///       returns comms::ErrorStatus::NotEnoughData.
    template <typename TIter, typename TNextLayerReader>
    comms::ErrorStatus doRead(
        Field& field,
        MsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize,
        TNextLayerReader&& nextLayerReader)
    {
        GASSERT(!msgPtr);
        auto es = field.read(iter, size);
        if (es == comms::ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, missingSize);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        auto id = field.value();
        auto remLen = size - field.length();

        unsigned idx = 0;
        while (true) {
            msgPtr = createMsg(id, idx);
            if (!msgPtr) {
                break;
            }

            using IterType = typename std::decay<decltype(iter)>::type;
            static_assert(std::is_same<typename std::iterator_traits<IterType>::iterator_category, std::random_access_iterator_tag>::value,
                "Iterator used for reading is expected to be random access one");
            IterType readStart = iter;
            es = nextLayerReader.read(msgPtr, iter, remLen, missingSize);
            if (es == comms::ErrorStatus::Success) {
                return es;
            }

            msgPtr.reset();
            iter = readStart;
            ++idx;
        }

        if ((0U < idx) && Factory::hasUniqueIds()) {
            return es;
        }

        GASSERT(!msgPtr);
        auto idxLimit = factory_.msgCount(id);
        if (idx < idxLimit) {
            return comms::ErrorStatus::MsgAllocFailure;
        }

        msgPtr = factory_.createGenericMsg(id);
        if (!msgPtr) {
            if (idx == 0) {
                return comms::ErrorStatus::InvalidMsgId;
            }

            return es;
        }

        es = nextLayerReader.read(msgPtr, iter, remLen, missingSize);
        if (es != comms::ErrorStatus::Success) {
            msgPtr.reset();
        }

        return es;
    }

    /// @brief Customized write functionality, invoked by @ref write().
    /// @details The function will write ID of the message to the data
    ///     sequence, then call write() member function of the next
    ///     protocol layer. If @b TMsg type is recognised to be actual message
    ///     type (inherited from comms::MessageBase while using
    ///     comms::option::StaticNumIdImpl option to specify its numeric ID),
    ///     its defined @b doGetId() member function (see comms::MessageBase::doGetId())
    ///     non virtual function is called. Otherwise polymorphic @b getId()
    ///     member function is used to retrieve the message ID information, which
    ///     means the message interface class must use comms::option::IdInfoInterface
    ///     option to define appropriate interface.
    /// @tparam TMsg Type of the message being written.
    /// @tparam TIter Type of iterator used for writing.
    /// @tparam TNextLayerWriter next layer writer object type.
    /// @param[out] field Field object to update and write.
    /// @param[in] msg Reference to message object
    /// @param[in, out] iter Output iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @param[in] nextLayerWriter Next layer writer object.
    /// @return Status of the write operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       written. In case of an error, distance between original position
    ///       and advanced will pinpoint the location of the error.
    /// @return Status of the write operation.
    template <typename TMsg, typename TIter, typename TNextLayerWriter>
    ErrorStatus doWrite(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter) const
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        field.value() = getMsgId(msg, IdRetrieveTag<MsgType>());
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field.length());
    }

    /// @copybrief ProtocolLayerBase::createMsg
    /// @details Hides and overrides createMsg() function inherited from
    ///     @ref ProtocolLayerBase. This function forwards the request to the
    ///     message factory object (@ref comms::MsgFactory) embedded as a private
    ///     data member of this class.
    /// @param[in] id ID of the message
    /// @param[in] idx Relative index of the message with the same ID.
    /// @return Smart pointer to the created message object.
    /// @see comms::MsgFactory::createMsg()
    MsgPtr createMsg(MsgIdParamType id, unsigned idx = 0)
    {
        return factory_.createMsg(id, idx);
    }

private:

    struct PolymorphicIdTag {};
    struct DirectIdTag {};

    template <typename TMsg>
    using IdRetrieveTag =
        typename std::conditional<
            details::ProtocolLayerHasDoGetId<TMsg>::Value,
            DirectIdTag,
            PolymorphicIdTag
        >::type;


    template <typename TMsg>
    static MsgIdParamType getMsgId(const TMsg& msg, PolymorphicIdTag)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(details::ProtocolLayerHasInterfaceOptions<MsgType>::Value,
            "The message class is expected to inherit from comms::Message");
        static_assert(MsgType::InterfaceOptions::HasMsgIdInfo,
            "The message interface class must expose polymorphic ID retrieval functionality, "
            "use comms::option::IdInfoInterface option to define it.");

        return msg.getId();
    }

    template <typename TMsg>
    static constexpr MsgIdParamType getMsgId(const TMsg& msg, DirectIdTag)
    {
        return msg.doGetId();
    }


    Factory factory_;

};


}  // namespace protocol

}  // namespace comms

