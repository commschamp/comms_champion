//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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
#include "comms/dispatch.h"
#include "comms/protocol/details/MsgIdLayerOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerExtendingClassHelper.h"

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
/// @tparam TOptions Default functionality extension options. Supported options are:
///     @li @ref comms::option::def::ExtendingClass - Use this option to provide a class
///         name of the extending class, which can be used to extend existing functionality.
///         See also @ref page_custom_id_layer tutorial page.
///     @li All the options supported by the @ref comms::MsgFactory. All the options
///         except ones listed above will be forwarded to the definition of the
///         inner instance of @ref comms::MsgFactory.
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
            details::ProtocolLayerExtendingClassT<
                MsgIdLayer<TField, TMessage, TAllMessages, TNextLayer, TOptions...>, 
                details::MsgIdLayerOptionsParser<TOptions...>
            >
        >
{
    static_assert(util::IsTuple<TAllMessages>::Value,
        "TAllMessages must be of std::tuple type");

    using ExtendingClass = 
            details::ProtocolLayerExtendingClassT<
                MsgIdLayer<TField, TMessage, TAllMessages, TNextLayer, TOptions...>, 
                details::MsgIdLayerOptionsParser<TOptions...>
            >;

    using BaseImpl =
        ProtocolLayerBase<
            TField,
            TNextLayer,
            ExtendingClass
        >;

    /// @brief Parsed options
    using ParsedOptionsInternal = details::MsgIdLayerOptionsParser<TOptions...>;
    using FactoryOptions = typename ParsedOptionsInternal::FactoryOptions;
    using Factory = comms::MsgFactory<TMessage, TAllMessages, FactoryOptions>;

    static_assert(TMessage::InterfaceOptions::HasMsgIdType,
        "Usage of MsgIdLayer requires support for ID type. "
        "Use comms::option::def::MsgIdType option in message interface type definition.");

public:

    /// @brief Parsed options
    using ParsedOptions = ParsedOptionsInternal;

    /// @brief Parsed options of the message Factory
    using FactoryParsedOptions = typename Factory::ParsedOptions;

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

    /// @brief Reason for message creation failure
    using CreateFailureReason = typename Factory::CreateFailureReason;

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
    ///     generate appropriate (or validate provided) message object based on the read ID and
    ///     forward the read() request to the next layer.
    ///     If the message object cannot be generated (the message type is not
    ///     provided inside @b TAllMessages template parameter), but
    ///     the @ref comms::option::app::SupportGenericMessage option has beed used,
    ///     the @ref comms::GenericMessage may be generated instead.@n
    ///     @b NOTE, that @b msg parameter can be either reference to a smart pointer,
    ///     which will hold allocated object, or to previously allocated object itself.
    ///     In case of the latter, the function will compare read and expected message
    ///     ID value and will return @ref comms::ErrorStatus::InvalidMsgId in case of mismatch.
    /// @tparam TMsg Type of the @b msg parameter
    /// @tparam TIter Type of iterator used for reading.
    /// @tparam TNextLayerReader next layer reader object type.
    /// @param[out] field Field object to read.
    /// @param[in, out] msg Reference to smart pointer that will hold
    ///                 allocated message object, or to the previously allocated
    ///                 message object itself (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[in] nextLayerReader Reader object, needs to be invoked to
    ///     forward read operation to the next layer.
    /// @param[out] extraValues Variadic extra output parameters passed to the
    ///     "read" operatation of the protocol stack (see
    ///     @ref comms::protocol::ProtocolLayerBase::read() "read()" and
    ///     @ref comms::protocol::ProtocolLayerBase::readFieldsCached() "readFieldsCached()").
    ///     Need to passed on as variadic arguments to the @b nextLayerReader.
    /// @return Status of the operation.
    /// @pre @b msg parameter, in case of being a smart pointer, doesn't point to any object:
    ///      @code assert(!msg); @endcode
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post Returns comms::ErrorStatus::Success if and only if msg points
    ///       to a valid object (in case of being a smart pointer).
    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus doRead(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        TExtraValues... extraValues)
    {
        auto beforeReadIter = iter;
        auto es = field.read(iter, size);
        if (es == comms::ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, extraValues...);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        auto fieldLen = static_cast<std::size_t>(std::distance(beforeReadIter, iter));

        using Tag =
            typename std::conditional<
                comms::isMessageBase<typename std::decay<decltype(msg)>::type>(),
                DirectOpTag,
                PointerOpTag
            >::type;

        return
            doReadInternal(
                field,
                msg,
                iter,
                size - fieldLen,
                std::forward<TNextLayerReader>(nextLayerReader),
                Tag(),
                extraValues...);
    }

    /// @brief Customized write functionality, invoked by @ref write().
    /// @details The function will write ID of the message to the data
    ///     sequence, then call write() member function of the next
    ///     protocol layer. If @b TMsg type is recognised to be actual message
    ///     type (inherited from comms::MessageBase while using
    ///     @ref comms::option::def::StaticNumIdImpl option to specify its numeric ID),
    ///     its defined @b doGetId() member function (see comms::MessageBase::doGetId())
    ///     non virtual function is called. Otherwise polymorphic @b getId()
    ///     member function is used to retrieve the message ID information, which
    ///     means the message interface class must use @ref comms::option::app::IdInfoInterface
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
        static_cast<const ExtendingClass*>(this)->prepareFieldForWrite(
            getMsgId(msg, IdRetrieveTag<MsgType>()), 
            msg, 
            field);

        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(field.length() <= size);

        using Tag =
            typename std::conditional<
                comms::isMessageBase<MsgType>() || MsgType::hasWrite(),
                DirectOpTag,
                StaticBinSearchOpTag
            >::type;

        return writeInternal(field, msg, iter, size - field.length(), std::forward<TNextLayerWriter>(nextLayerWriter), Tag());
    }

    /// @copybrief ProtocolLayerBase::createMsg
    /// @details Hides and overrides createMsg() function inherited from
    ///     @ref ProtocolLayerBase. This function forwards the request to the
    ///     message factory object (@ref comms::MsgFactory) embedded as a private
    ///     data member of this class.
    /// @param[in] id ID of the message
    /// @param[in] idx Relative index of the message with the same ID.
    /// @param[out] reason Failure reason in case creation has failed. May be nullptr.
    /// @return Smart pointer to the created message object.
    /// @see comms::MsgFactory::createMsg()
    MsgPtr createMsg(MsgIdParamType id, unsigned idx = 0, CreateFailureReason* reason = nullptr)
    {
        return factory_.createMsg(id, idx, reason);
    }

    /// @brief Compile time inquiry whether polymorphic dispatch tables are 
    ///     generated internally to map message ID to actual type.
    static constexpr bool isDispatchPolymorphic()
    {
        return Factory::isDispatchPolymorphic();
    }

    /// @brief Compile time inquiry whether static binary search dispatch is 
    ///     generated internally to map message ID to actual type.
    static constexpr bool isDispatchStaticBinSearch()
    {
        return Factory::isDispatchStaticBinSearch();
    }

    /// @brief Compile time inquiry whether linear switch dispatch is 
    ///     generated internally to map message ID to actual type.
    static constexpr bool isDispatchLinearSwitch()
    {
        return Factory::isDispatchLinearSwitch();
    }

protected:

    /// @brief Retrieve message id from the field.
    /// @details May be overridden by the extending class
    /// @param[in] field Field for this layer.
    static MsgIdType getMsgIdFromField(const Field& field)
    {
        static_assert(
            comms::field::isIntValue<Field>() || comms::field::isEnumValue<Field>() || comms::field::isNoValue<Field>(),
            "Field must be of IntValue or EnumValue types");

        return static_cast<MsgIdType>(field.value());
    }

    /// @brief Extra operation before read.
    /// @details Function called after appropriate message object
    ///     has been created and before read operation is 
    ///     forwared to inner layer. @n
    ///     Default implementation does nothing, may be overriden in the
    ///     derived class.
    /// @param[in] field Field of the layer that was successfully read.
    /// @param[in, out] msg Reference to message object, either interface
    ///     class or message object itself (depending on how doRead() was invoked).
    template <typename TMsg>
    static void beforeRead(const Field& field, TMsg& msg)
    {
        static_cast<void>(field);
        static_cast<void>(msg);
    }

    /// @brief Prepare field for writing
    /// @details Must assign provided id value. 
    ///     May be overridden by the extending class if some complex functionality is required.
    /// @param[in] id ID of the message
    /// @param[in] msg Reference to message object being written
    /// @param[out] field Field, value of which needs to be populated
    template <typename TMsg>
    static void prepareFieldForWrite(MsgIdParamType id, const TMsg& msg, Field& field)
    {
        static_assert(
            comms::field::isIntValue<Field>() || comms::field::isEnumValue<Field>() || comms::field::isNoValue<Field>(),
            "Field must be of IntValue or EnumValue types");

        static_cast<void>(msg);
        field.value() = static_cast<typename Field::ValueType>(id);
    }

private:

    struct PolymorphicOpTag {};
    struct DirectOpTag {};
    struct PointerOpTag {};
    struct StaticBinSearchOpTag {};

    template <typename TMsg>
    using IdRetrieveTag =
        typename std::conditional<
            details::protocolLayerHasDoGetId<TMsg>(),
            DirectOpTag,
            PolymorphicOpTag
        >::type;

    struct IdParamAsIsTag {};
    struct IdParamCastTag {};

    template <typename TId>
    using IdParamTag =
        typename std::conditional<
            std::is_base_of<MsgIdType, TId>::value,
            IdParamAsIsTag,
            IdParamCastTag
        >::type;

    struct HasGenericMsgTag {};
    struct NoGenericMsgTag {};

    template <typename TIter, typename TNextLayerReader, typename... TExtraValues>
    class ReadRedirectionHandler
    {
    public:
        using RetType = comms::ErrorStatus;

        ReadRedirectionHandler(
            TIter& iter,
            std::size_t size,
            TNextLayerReader&& nextLayerReader,
            TExtraValues... extraValues)
          : m_iter(iter),
            m_size(size),
            m_nextLayerReader(std::move(nextLayerReader)),
            m_extraValues(extraValues...)
        {
        }

        template <typename TMsg>
        RetType handle(TMsg& msg)
        {
            static_assert(comms::isMessageBase<TMsg>(), "Expected to be a valid message object");
            return handleInternal(msg, m_extraValues);
        }

        RetType handle(TMessage& msg)
        {
            static_cast<void>(msg);
            COMMS_ASSERT(!"Should not happen");
            return comms::ErrorStatus::InvalidMsgId;
        }        

    private:
        template <typename TMsg>
        RetType handleInternal(TMsg& msg, std::tuple<>&)
        {
            return m_nextLayerReader.read(msg, m_iter, m_size);
        }

        template <typename TMsg, typename T0>
        RetType handleInternal(TMsg& msg, std::tuple<T0>& extraValues)
        {
            return m_nextLayerReader.read(msg, m_iter, m_size, std::get<0>(extraValues));
        }

        template <typename TMsg, typename T0, typename T1>
        RetType handleInternal(TMsg& msg, std::tuple<T0, T1>& extraValues)
        {
            return m_nextLayerReader.read(msg, m_iter, m_size, std::get<0>(extraValues), std::get<1>(extraValues));
        }

        template <typename TMsg, typename T0, typename T1, typename T2>
        RetType handleInternal(TMsg& msg, std::tuple<T0, T1, T2>& extraValues)
        {
            return
               m_nextLayerReader.read(
                    msg,
                    m_iter,
                    m_size,
                    std::get<0>(extraValues),
                    std::get<1>(extraValues),
                    std::get<2>(extraValues));
        }

        template <typename TMsg, typename T0, typename T1, typename T2, typename T3>
        RetType handleInternal(TMsg& msg, std::tuple<T0, T1, T2, T3>& extraValues)
        {
            return
               m_nextLayerReader.read(
                    msg,
                    m_iter,
                    m_size,
                    std::get<0>(extraValues),
                    std::get<1>(extraValues),
                    std::get<2>(extraValues),
                    std::get<3>(extraValues));
        }

        template <typename TMsg, typename T0, typename T1, typename T2, typename T3, typename T4>
        RetType handleInternal(TMsg& msg, std::tuple<T0, T1, T2, T3, T4>& extraValues)
        {
            return
               m_nextLayerReader.read(
                    msg,
                    m_iter,
                    m_size,
                    std::get<0>(extraValues),
                    std::get<1>(extraValues),
                    std::get<2>(extraValues),
                    std::get<3>(extraValues),
                    std::get<4>(extraValues));
        }

        template <typename TMsg, typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
        RetType handleInternal(TMsg& msg, std::tuple<T0, T1, T2, T3, T4, T5>& extraValues)
        {
            return
               m_nextLayerReader.read(
                    msg,
                    m_iter,
                    m_size,
                    std::get<0>(extraValues),
                    std::get<1>(extraValues),
                    std::get<2>(extraValues),
                    std::get<3>(extraValues),
                    std::get<4>(extraValues),
                    std::get<5>(extraValues));
        }

        TIter& m_iter;
        std::size_t m_size = 0U;
        TNextLayerReader&& m_nextLayerReader;
        std::tuple<TExtraValues...> m_extraValues;
    };

    template <typename TIter, typename TNextLayerReader, typename... TExtraValues>
    ReadRedirectionHandler<TIter, TNextLayerReader, TExtraValues...> makeReadRedirectionHandler(
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        TExtraValues... extraValues)
    {
        return ReadRedirectionHandler<TIter, TNextLayerReader, TExtraValues...>(iter, size, std::move(nextLayerReader), extraValues...);
    }

    template <typename TIter, typename TNextLayerWriter>
    class WriteRedirectionHandler
    {
    public:
        using RetType = comms::ErrorStatus;

        WriteRedirectionHandler(
            TIter& iter,
            std::size_t size,
            TNextLayerWriter&& nextLayerWriter)
          : m_iter(iter),
            m_size(size),
            m_nextLayerWriter(std::move(nextLayerWriter))
        {
        }

        template <typename TMsg>
        RetType handle(const TMsg& msg)
        {
            static_assert(comms::isMessageBase<TMsg>(), "Expected to be a valid message object");
            return m_nextLayerWriter.write(msg, m_iter, m_size);
        }
        
        RetType handle(const TMessage& msg)
        {
            static_cast<void>(msg);
            COMMS_ASSERT(!"Should not happen");
            return comms::ErrorStatus::InvalidMsgId;
        }   

    private:
        TIter& m_iter;
        std::size_t m_size = 0U;
        TNextLayerWriter&& m_nextLayerWriter;
    };

    template <typename TIter, typename TNextLayerWriter>
    WriteRedirectionHandler<TIter, TNextLayerWriter> makeWriteRedirectionHandler(
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter)
    {
        return WriteRedirectionHandler<TIter, TNextLayerWriter>(iter, size, std::move(nextLayerWriter));
    }

    template <typename TMsg>
    static MsgIdParamType getMsgId(const TMsg& msg, PolymorphicOpTag)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(comms::isMessage<MsgType>(),
            "The message class is expected to inherit from comms::Message");
        static_assert(MsgType::InterfaceOptions::HasMsgIdInfo,
            "The message interface class must expose polymorphic ID retrieval functionality, "
            "use comms::option::app::IdInfoInterface option to define it.");

        return msg.getId();
    }

    template <typename TMsg>
    static constexpr MsgIdParamType getMsgId(const TMsg& msg, DirectOpTag)
    {
        return msg.doGetId();
    }

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus doReadInternalDirect(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        TExtraValues... extraValues)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(details::protocolLayerHasDoGetId<MsgType>(),
                "Explicit message type is expected to expose compile type message ID by "
                "using \"StaticNumIdImpl\" option");

        auto id = static_cast<ExtendingClass*>(this)->getMsgIdFromField(field);
        BaseImpl::setMsgId(id, extraValues...);
        if (id != MsgType::doGetId()) {
            return ErrorStatus::InvalidMsgId;
        }

        static_cast<ExtendingClass*>(this)->beforeRead(field, msg);
        return nextLayerReader.read(msg, iter, size, extraValues...);
    }

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus doReadInternal(
        MsgIdParamType id,
        unsigned idx,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        PolymorphicOpTag,
        TExtraValues... extraValues)
    {
        static_cast<void>(id);
        static_cast<void>(idx);
        return nextLayerReader.read(msg, iter, size, extraValues...);
    }

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus doReadInternal(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        DirectOpTag,
        TExtraValues... extraValues)
    {
        return
            doReadInternalDirect(
                field,
                msg,
                iter,
                size,
                std::forward<TNextLayerReader>(nextLayerReader),
                extraValues...);
    }

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus doReadInternal(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        PointerOpTag,
        TExtraValues... extraValues)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(comms::details::hasElementType<MsgType>(),
            "Unsupported type of message object, expected to be either message itself or smart pointer");

        using MsgElementType = typename MsgType::element_type;

//        static_assert(std::has_virtual_destructor<MsgElementType>::value,
//            "Message object is (dynamically) allocated and held by the pointer to the base class. "
//            "However, there is no virtual desctructor to perform proper destruction.");

        using Tag = 
            typename std::conditional<
                MsgElementType::hasRead(),
                PolymorphicOpTag,
                StaticBinSearchOpTag
            >::type;

        const auto id = static_cast<ExtendingClass*>(this)->getMsgIdFromField(field);
        BaseImpl::setMsgId(id, extraValues...);

        auto es = comms::ErrorStatus::InvalidMsgId;
        unsigned idx = 0;
        CreateFailureReason failureReason = CreateFailureReason::None;
        while (true) {
            COMMS_ASSERT(!msg);
            msg = createMsgInternal(id, idx, &failureReason);
            if (!msg) {
                break;
            }

            using IterType = typename std::decay<decltype(iter)>::type;
            static_assert(std::is_same<typename std::iterator_traits<IterType>::iterator_category, std::random_access_iterator_tag>::value,
                "Iterator used for reading is expected to be random access one");
            IterType readStart = iter;                

            static_cast<ExtendingClass*>(this)->beforeRead(field, *msg);
            es = doReadInternal(id, idx, msg, iter, size, std::forward<TNextLayerReader>(nextLayerReader), Tag(), extraValues...);
            if (es == comms::ErrorStatus::Success) {
                BaseImpl::setMsgIndex(idx, extraValues...);
                return es;
            }

            msg.reset();
            iter = readStart;
            ++idx;
        }

        BaseImpl::setMsgIndex(idx, extraValues...);
        COMMS_ASSERT(!msg);
        if (failureReason == CreateFailureReason::AllocFailure) {
            return comms::ErrorStatus::MsgAllocFailure;
        }        
        
        COMMS_ASSERT(failureReason == CreateFailureReason::InvalidId);
        using GenericMsgTag = 
            typename std::conditional<
                Factory::ParsedOptions::HasSupportGenericMessage,
                HasGenericMsgTag,
                NoGenericMsgTag
            >::type;

        return createAndReadGenericMsgInternal(
            field, 
            idx,
            msg, 
            iter, 
            size, 
            std::forward<TNextLayerReader>(nextLayerReader), 
            es,
            GenericMsgTag(),
            extraValues...);
    }

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus doReadInternal(
        MsgIdParamType id,
        unsigned idx,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        StaticBinSearchOpTag,
        TExtraValues... extraValues)
    {
        auto handler =
            makeReadRedirectionHandler(
                iter,
                size,
                std::forward<TNextLayerReader>(nextLayerReader),
                extraValues...);
        return comms::dispatchMsgStaticBinSearch<AllMessages>(id, idx, *msg, handler);
    }

    template <typename TId>
    MsgPtr createMsgInternalTagged(TId&& id, unsigned idx, CreateFailureReason* reason, IdParamAsIsTag)
    {
        return createMsg(std::forward<TId>(id), idx, reason);
    }

    template <typename TId>
    MsgPtr createMsgInternalTagged(TId&& id, unsigned idx, CreateFailureReason* reason, IdParamCastTag)
    {
        return createMsg(static_cast<MsgIdType>(id), idx, reason);
    }

    template <typename TId>
    MsgPtr createMsgInternal(TId&& id, unsigned idx, CreateFailureReason* reason)
    {
        using IdType = typename std::decay<decltype(id)>::type;
        return createMsgInternalTagged(std::forward<TId>(id), idx, reason, IdParamTag<IdType>());
    }

    template <typename TId>
    MsgPtr createGenericMsgInternalTagged(TId&& id, unsigned idx, IdParamAsIsTag)
    {
        return factory_.createGenericMsg(std::forward<TId>(id), idx);
    }

    template <typename TId>
    MsgPtr createGenericMsgInternalTagged(TId&& id, unsigned idx, IdParamCastTag)
    {
        return factory_.createGenericMsg(static_cast<MsgIdType>(id), idx);
    }

    template <typename TId>
    MsgPtr createGenericMsgInternal(TId&& id, unsigned idx)
    {
        using IdType = typename std::decay<decltype(id)>::type;
        return createGenericMsgInternalTagged(std::forward<TId>(id), idx, IdParamTag<IdType>());
    }

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus createAndReadGenericMsgInternal(
        const Field& field,
        unsigned msgIdx,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        comms::ErrorStatus es,
        NoGenericMsgTag,
        TExtraValues...)
    {
        static_cast<void>(field);
        static_cast<void>(msgIdx);
        static_cast<void>(msg);
        static_cast<void>(iter);
        static_cast<void>(size);
        static_cast<void>(nextLayerReader);
        return es;
    }   

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus createAndReadGenericMsgInternal(
        const Field& field,
        unsigned msgIdx,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        comms::ErrorStatus es,
        HasGenericMsgTag,
        TExtraValues... extraValues)
    {
        using GenericMsgType = typename Factory::ParsedOptions::GenericMessage;

        auto id = static_cast<ExtendingClass*>(this)->getMsgIdFromField(field);
        msg = createGenericMsgInternal(id, msgIdx);
        if (!msg) {
            return es;
        }

        static_cast<ExtendingClass*>(this)->beforeRead(field, *msg);

        using Tag = 
            typename std::conditional<
                GenericMsgType::hasRead(),
                PolymorphicOpTag,
                DirectOpTag
            >::type;

        return
            readGenericMsg(
                msg,
                iter,
                size,
                std::forward<TNextLayerReader>(nextLayerReader),
                Tag(),
                extraValues...);
    }  

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus readGenericMsg(
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        PolymorphicOpTag,
        TExtraValues... extraValues)
    {
        return nextLayerReader.read(msg, iter, size, extraValues...);
    }  

    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus readGenericMsg(
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        DirectOpTag,
        TExtraValues... extraValues)
    {
        using GenericMsgType = typename Factory::ParsedOptions::GenericMessage;
        auto& castedMsgRef = static_cast<GenericMsgType&>(*msg);
        return nextLayerReader.read(castedMsgRef, iter, size, extraValues...);
    }               

    template <typename TId>
    std::size_t msgCountInternalTagged(TId&& id, IdParamAsIsTag)
    {
        return factory_.msgCount(std::forward<TId>(id));
    }

    template <typename TId>
    std::size_t msgCountInternalTagged(TId&& id, IdParamCastTag)
    {
        return factory_.msgCount(static_cast<MsgIdType>(id));
    }


    template <typename TId>
    std::size_t msgCountInternal(TId&& id)
    {
        using IdType = typename std::decay<decltype(id)>::type;
        return msgCountInternalTagged(std::forward<TId>(id), IdParamTag<IdType>());
    }

    template <typename TMsg, typename TIter, typename TNextLayerWriter>
    ErrorStatus writeInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter,
        DirectOpTag) const
    {
        static_cast<void>(field);
        return nextLayerWriter.write(msg, iter, size);
    }


    template <typename TMsg, typename TIter, typename TNextLayerWriter>
    ErrorStatus writeInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter,
        StaticBinSearchOpTag) const
    {
        auto handler = makeWriteRedirectionHandler(iter, size, std::forward<TNextLayerWriter>(nextLayerWriter));
        auto id = static_cast<const ExtendingClass*>(this)->getMsgIdFromField(field);
        return comms::dispatchMsgStaticBinSearch(id, msg, handler);
    }

    Factory factory_;

};


namespace details
{
template <typename T>
struct MsgIdLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField,
          typename TMessage,
          typename TAllMessages,
          typename TNextLayer,
          typename... TOptions>
struct MsgIdLayerCheckHelper<MsgIdLayer<TField, TMessage, TAllMessages, TNextLayer, TOptions...> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref MsgIdLayer
/// @related MsgIdLayer
template <typename T>
constexpr bool isMsgIdLayer()
{
    return details::MsgIdLayerCheckHelper<T>::Value;
}

}  // namespace protocol

}  // namespace comms

