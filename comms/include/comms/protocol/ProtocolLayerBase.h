//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

#include <tuple>
#include <utility>
#include <algorithm>

#include "comms/ErrorStatus.h"
#include "comms/util/Tuple.h"
#include "comms/Assert.h"
#include "comms/options.h"

#include "comms/protocol/details/ProtocolLayerBaseOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerDetails.h"
#include "comms/details/protocol_layers_access.h"
#include "comms/details/detect.h"

namespace comms
{

namespace protocol
{

/// @brief Base class for all the middle (non @ref MsgDataLayer) protocol transport layers.
/// @details Provides all the default and/or common functionality for the
///     middle transport layer. The inheriting actual layer class may
///     use and/or override the provided functionality by redefining member
///     function with the same signature. The @ref NextLayer is stored as a private
///     data meber.
/// @tparam TField Every middle layer will have a field containing extra
///     information for this layer. This template parameter is a type of such
///     field.
/// @tparam TNextLayer Next layer this one wraps and forwards the calls to.
/// @tparam TDerived Actual protocol layer class that extends this one.
/// @tparam TOptions Extra options. Supported ones are:
///     @li @ref comms::option::def::ProtocolLayerForceReadUntilDataSplit
///     @li @ref comms::option::def::ProtocolLayerDisallowReadUntilDataSplit
/// @headerfile comms/protocol/ProtocolLayerBase.h
template <
    typename TField,
    typename TNextLayer,
    typename TDerived,
    typename...  TOptions>
class ProtocolLayerBase
{
public:
    /// @brief Type of the field used for this layer.
    using Field = TField;

    /// @brief Type of the next transport layer
    using NextLayer = TNextLayer;

    /// @brief Parsed options structure
    using ParsedOptions = details::ProtocolLayerBaseOptionsParser<TOptions...>;

    /// @brief Type of all the fields of all the transport layers
    ///     wrapped in std::tuple.
    /// @details The @ref Field type is prepended to the @ref AllFields type
    ///     of the @ref NextLayer and reported as @ref AllFields of this one.
    using AllFields = typename std::decay<
        decltype(
            std::tuple_cat(
                std::declval<std::tuple<Field> >(),
                std::declval<typename TNextLayer::AllFields>())
            )
        >::type;

    /// @brief All supported messages.
    /// @details Same as NextLayer::AllMessages or void if such doesn't exist.
    using AllMessages = details::ProtocolLayerAllMessagesType<NextLayer>;

    /// @brief Type of pointer to the message.
    /// @details Same as NextLayer::MsgPtr or void if such doesn't exist.
    using MsgPtr = typename details::ProtocolLayerMsgPtr<NextLayer>::Type;

    /// @brief Actual derived class
    using ThisLayer = TDerived;

    /// @brief Static constant indicating amount of transport layers used.
    static const std::size_t NumOfLayers = 1 + NextLayer::NumOfLayers;

    /// @brief Copy constructor
    ProtocolLayerBase(const ProtocolLayerBase&) = default;

    /// @brief Move constructor
    ProtocolLayerBase(ProtocolLayerBase&&) = default;

    /// @brief Constructor.
    /// @details Forwards all the parameters to the constructor of the embedded
    ///     @ref NextLayer object.
    /// @param args Arguments to be passed to the constructor of the next layer
    template <typename... TArgs>
    explicit ProtocolLayerBase(TArgs&&... args)
      : nextLayer_(std::forward<TArgs>(args)...)
    {
    }

    /// @brief Desctructor
    ~ProtocolLayerBase() noexcept = default;

    /// @brief Copy assignment
    ProtocolLayerBase& operator=(const ProtocolLayerBase&) = default;

    /// @brief Get access to the next layer object.
    NextLayer& nextLayer()
    {
        return nextLayer_;
    }

    /// @brief Get "const" access to the next layer object.
    const NextLayer& nextLayer() const
    {
        return nextLayer_;
    }

    /// @brief Get access to this layer object.
    ThisLayer& thisLayer()
    {
        return static_cast<ThisLayer&>(*this);
    }

    /// @brief Get "const" access to this layer object.
    const ThisLayer& thisLayer() const
    {
        return static_cast<const ThisLayer&>(*this);
    }

    /// @brief Compile time check whether split read "until" and "from" data
    ///     layer is allowed.
    static constexpr bool canSplitRead()
    {
        return (!ParsedOptions::HasDisallowReadUntilDataSplit) && NextLayer::canSplitRead();
    }

    /// @brief Deserialise message from the input data sequence.
    /// @details The function will invoke @b doRead() member function
    ///     provided by the derived class, which must have the following signature
    ///     and logic:
    ///     @code
    ///         template<typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    ///         comms::ErrorStatus doRead(
    ///             Field& field, // field object used to read required data
    ///             TMsg& msg, // Ref to smart pointer to message object, or message object itself
    ///             TIter& iter, // iterator used for reading
    ///             std::size_t size, // size of the remaining data
    ///             TNextLayerReader&& nextLayerReader, // next layer reader object
    ///             TExtraValues... extraValues)
    ///         {
    ///             // internal logic prior next layer read, such as reading the field value
    ///             auto es = field.read(iter, size);
    ///             ...
    ///             // request next layer to perform read operation
    ///             es = nextLayerReader.read(msg, iter, size - field.length(), extraValues...);
    ///             ... // internal logic after next layer read if applicable
    ///             return es;
    ///         };
    ///     @endcode
    ///     The signature of the @b nextLayerReader.read() function is
    ///     the same as the signature of this @b read() member function.
    ///     The implemented @b doRead() member function also may use the following
    ///     inherited protected member to set values of variadic parameters in
    ///     case they are provided.
    ///     @li @ref comms::protocol::ProtocolLayerBase::updateMissingSize() "updateMissingSize()" -
    ///     to calculate update missing size if such is requested.
    ///     @li @ref comms::protocol::ProtocolLayerBase::setMissingSize() "setMissingSize()" -
    ///     to set specific value as missing size if suce information is requested.
    ///     @li @ref comms::protocol::ProtocolLayerBase::setMsgId() "setMsgId()" -
    ///     to set the value of message id
    ///     @li @ref comms::protocol::ProtocolLayerBase::setMsgIndex() "setMsgIndex()" -
    ///     to set the value of message index.
    /// @tparam TMsg Type of @b msg parameter
    /// @tparam TIter Type of iterator used for reading.
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] extraValues Extra output parameters provided using one
    ///     of the following functions: @ref comms::protocol::missingSize(),
    ///     @ref comms::protocol::msgId(), @ref comms::protocol::msgIndex(),
    ///     @ref comms::protocol::msgPayload().
    /// @return Status of the operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post Returns comms::ErrorStatus::Success if and only if msg points
    ///       to a valid object.
    template <typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus read(
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TExtraValues... extraValues)
    {
        using Tag =
            typename std::conditional<
                ParsedOptions::HasForceReadUntilDataSplit,
                SplitReadTag,
                NormalReadTag
            >::type;

        static_assert(std::is_same<Tag, NormalReadTag>::value || canSplitRead(),
            "Read split is disallowed by at least one of the inner layers");
        return readInternal(msg, iter, size, Tag(), extraValues...);
    }

    /// @brief Perform read of data fields until data layer (message payload).
    /// @details Same as @b read by stops read operation when data layer is reached.
    ///     Expected to be followed by a call to @ref readFromData().
    /// @tparam TMsg Type of @b msg parameter.
    /// @tparam TIter Type of iterator used for reading.
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] extraValues Extra output parameters provided using one
    ///     of the following functions
    ///     @li @ref comms::protocol::missingSize()
    ///     @li @ref comms::protocol::msgId()
    ///     @li @ref comms::protocol::msgIndex()
    ///     @li @ref comms::protocol::msgPayload()
    /// @return Status of the operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post missingSize output value is updated if and only if function
    ///       returns comms::ErrorStatus::NotEnoughData.
    template <typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readUntilData(
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TExtraValues... extraValues)
    {

        Field field;
        auto& derivedObj = static_cast<TDerived&>(*this);
        return
            derivedObj.doRead(
                field,
                msg,
                iter,
                size,
                createNextLayerUntilDataReader(),
                extraValues...);
    }

    /// @brief Finalise the read operation by reading the message payload.
    /// @details Should be called to finalise the read operation started by
    ///     @ref readUntilData().
    /// @tparam TMsg Type of @b msg parameter
    /// @tparam TIter Type of iterator used for reading.
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] extraValues Extra output parameters provided using one
    ///     of the following functions
    ///     @li @ref comms::protocol::missingSize()
    ///     @li @ref comms::protocol::msgId()
    ///     @li @ref comms::protocol::msgIndex()
    ///     @li @ref comms::protocol::msgPayload()
    /// @return Status of the operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    template <typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readFromData(
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TExtraValues... extraValues)
    {
        return nextLayer().readFromData(msg, iter, size, extraValues...);
    }

    /// @brief Deserialise message from the input data sequence while caching
    ///     the read transport information fields.
    /// @details Very similar to @ref read() member function, but adds "allFields"
    ///     parameter to store read transport information fields.
    ///     The function will also invoke the same @b doRead() member function
    ///     provided by the derived class, as described with @ref read().
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TMsg Type of @b msg parameter.
    /// @tparam TIter Type of iterator used for reading.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Iterator used for reading.
    /// @param[in] size Number of bytes available for reading.
    /// @param[out] extraValues Extra output parameters provided using one
    ///     of the following functions
    ///     @li @ref comms::protocol::missingSize()
    ///     @li @ref comms::protocol::msgId()
    ///     @li @ref comms::protocol::msgIndex()
    ///     @li @ref comms::protocol::msgPayload()
    /// @return Status of the operation.
    template <typename TAllFields, typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readFieldsCached(
        TAllFields& allFields,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TExtraValues... extraValues)
    {
        using AllFieldsDecayed = typename std::decay<TAllFields>::type;
        static_assert(util::tupleIsTailOf<AllFields, AllFieldsDecayed>(), "Passed tuple is wrong.");
        static const std::size_t Idx =
            std::tuple_size<AllFieldsDecayed>::value -
                                std::tuple_size<AllFields>::value;
        auto& field = getField<Idx>(allFields);
        auto& derivedObj = static_cast<TDerived&>(*this);
        return
            derivedObj.doRead(
                field,
                msg,
                iter,
                size,
                createNextLayerCachedFieldsReader(allFields),
                extraValues...);
    }

    /// @brief Perform read of data fields until data layer (message payload) while caching
    ///     the read transport information fields.
    /// @details Very similar to @ref readUntilData() member function, but adds "allFields"
    ///     parameter to store read transport information fields.
    ///     The function will also invoke the same @b doRead() member function
    ///     provided by the derived class, as described with @ref read().
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TMsg Type of @b msg parameter
    /// @tparam TIter Type of iterator used for reading.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Iterator used for reading.
    /// @param[in] size Number of bytes available for reading.
    /// @param[out] extraValues Extra output parameters provided using one
    ///     of the following functions
    ///     @li @ref comms::protocol::missingSize()
    ///     @li @ref comms::protocol::msgId()
    ///     @li @ref comms::protocol::msgIndex()
    ///     @li @ref comms::protocol::msgPayload()
    /// @return Status of the operation.
    template <typename TAllFields, typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readUntilDataFieldsCached(
        TAllFields& allFields,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TExtraValues... extraValues)
    {
        using AllFieldsDecayed = typename std::decay<TAllFields>::type;
        static_assert(util::tupleIsTailOf<AllFields, AllFieldsDecayed>(), "Passed tuple is wrong.");
        static const std::size_t Idx =
            std::tuple_size<AllFieldsDecayed>::value -
                                std::tuple_size<AllFields>::value;

        auto& field = getField<Idx>(allFields);
        auto& derivedObj = static_cast<TDerived&>(*this);
        return
            derivedObj.doRead(
                field,
                msg,
                iter,
                size,
                createNextLayerCachedFieldsUntilDataReader(allFields),
                extraValues...);
    }

    /// @brief Finalise the read operation by reading the message payload while caching
    ///     the read transport information fields.
    /// @details Should be called to finalise the read operation started by
    ///     @ref readUntilDataFieldsCached().
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TMsg Type of @b msg parameter
    /// @tparam TIter Type of iterator used for reading.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Iterator used for reading.
    /// @param[in] size Number of bytes available for reading.
    /// @param[out] extraValues Extra output parameters provided using one
    ///     of the following functions
    ///     @li @ref comms::protocol::missingSize()
    ///     @li @ref comms::protocol::msgId()
    ///     @li @ref comms::protocol::msgIndex()
    ///     @li @ref comms::protocol::msgPayload()
    /// @return Status of the operation.
    template <typename TAllFields, typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readFromDataFieldsCached(
        TAllFields& allFields,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TExtraValues... extraValues)
    {
        return nextLayer().readFromDataFieldsCached(allFields, msg, iter, size, extraValues...);
    }

    /// @brief Serialise message into output data sequence.
    /// @details The function will invoke @b doWrite() member function
    ///     provided by the derived class, which must have the following signature
    ///     and logic:
    ///     @code
    ///         template<typename TMsg, typename TIter, typename TNextLayerWriter>
    ///         comms::ErrorStatus doWrite(
    ///             Field& field, // field object used to update and write required data
    ///             const TMsg& msg, // reference to ready to be sent message object
    ///             TIter& iter, // iterator used for writing
    ///             std::size_t size, // Max number of bytes that can be written.
    ///             TNextLayerWriter&& nextLayerWriter // next layer writer object
    ///             )
    ///         {
    ///             // internal logic prior next layer write, such as
    ///             // updating field's value and writing it.
    ///             field.value() = ...;
    ///             auto es = field.write(iter, size);
    ///             ...
    ///             // request next layer to perform write operation
    ///             es = nextLayerWriter.write(msg, iter, size - field.length());
    ///             ... // internal logic after next layer write if applicable
    ///             return es;
    ///         };
    ///     @endcode
    ///     The signature of the @b nextLayerWriter.write() function is
    ///     the same as the signature of this @b write() member function.
    /// @tparam TMsg Type of the message being written.
    /// @tparam TIter Type of iterator used for writing.
    /// @param[in] msg Reference to message object
    /// @param[in, out] iter Output iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       written. In case of an error, distance between original position
    ///       and advanced will pinpoint the location of the error.
    /// @return Status of the write operation.
    template <typename TMsg, typename TIter>
    comms::ErrorStatus write(
        const TMsg& msg,
        TIter& iter,
        std::size_t size) const
    {
        Field field;
        auto& derivedObj = static_cast<const TDerived&>(*this);
        return derivedObj.doWrite(field, msg, iter, size, createNextLayerWriter());
    }

    /// @brief Serialise message into output data sequence while caching the written transport
    ///     information fields.
    /// @details Very similar to @ref write() member function, but adds "allFields"
    ///     parameter to store raw data of the message.
    ///     The function will also invoke the same @b doWrite() member function
    ///     provided by the derived class, as described with @ref write().
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TMsg Type of the message being written.
    /// @tparam TIter Type of iterator used for writing.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in] msg Reference to the message object that is being written,
    /// @param[in, out] iter Iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    template <typename TAllFields, typename TMsg, typename TIter>
    comms::ErrorStatus writeFieldsCached(
        TAllFields& allFields,
        const TMsg& msg,
        TIter& iter,
        std::size_t size) const
    {
        using AllFieldsDecayed = typename std::decay<TAllFields>::type;
        static_assert(util::tupleIsTailOf<AllFields, AllFieldsDecayed>(), "Passed tuple is wrong.");
        static const std::size_t Idx =
            std::tuple_size<AllFieldsDecayed>::value -
                                std::tuple_size<AllFields>::value;

        auto& field = getField<Idx>(allFields);
        auto& derivedObj = static_cast<const TDerived&>(*this);
        return derivedObj.doWrite(field, msg, iter, size, createNextLayerCachedFieldsWriter(allFields));
    }

    /// @brief Get remaining length of wrapping transport information.
    /// @details The message data always get wrapped with transport information
    ///     to be successfully delivered to and unpacked on the other side.
    ///     This function return remaining length of the transport information.
    ///     It performs a call to @ref doFieldLength() member function to
    ///     get info about current field length. To update the default behaviour
    ///     just override the function in the derived class.
    /// @return length of the field + length reported by the next layer.
    constexpr std::size_t length() const
    {
        return static_cast<const ThisLayer&>(*this).doFieldLength() + nextLayer_.length();
    }

    /// @brief Get remaining length of wrapping transport information + length
    ///     of the provided message.
    /// @details This function usually gets called when there is a need to
    ///     identify the size of the buffer required to write provided message
    ///     wrapped in the transport information. This function is very similar
    ///     to length(), but adds also length of the message.
    ///     It performs a call to @ref doFieldLength() member function with message parameter to
    ///     get info about current field length. To update the default behaviour
    ///     just override the function in the derived class.
    /// @tparam TMsg Type of message object.
    /// @param[in] msg Message object
    /// @return length of the field + length reported by the next layer.
    template <typename TMsg>
    constexpr std::size_t length(const TMsg& msg) const
    {
        return static_cast<const ThisLayer&>(*this).doFieldLength(msg) + nextLayer_.length(msg);
    }

    /// @brief Update recently written (using write()) message contents data.
    /// @details Sometimes, when NON random access iterator is used for writing
    ///     (for example std::back_insert_iterator), some transport data cannot
    ///     be properly written. In this case, @ref write() function will return
    ///     comms::ErrorStatus::UpdateRequired. When such status is returned
    ///     it is necessary to call update() with random access iterator on
    ///     the written buffer to update written dummy information with
    ///     proper values.@n
    ///     The function will invoke @b doUpdate() member function
    ///     provided (or inherited) by the derived class, which must have the following signature
    ///     and logic:
    ///     @code
    ///         template<typename TIter, typename TNextLayerUpdater>
    ///         comms::ErrorStatus doUpdate(
    ///             Field& field, // field object to update and re-write if necessary
    ///             TIter& iter, // iterator used for updateing
    ///             std::size_t size, // Number of remaning bytes in the output buffer.
    ///             TNextLayerUpdater&& nextLayerUpdater // next layer updater object
    ///             )
    ///         {
    ///             // internal logic prior next layer update, such as
    ///             // updating field's value and re-writing it.
    ///             field.value() = ...;
    ///             auto es = field.write(iter, size);
    ///             ...
    ///             // request next layer to perform update operation
    ///             es = nextLayerUpdater.update(iter, size - field.length());
    ///             ... // internal logic after next layer write if applicable
    ///             return es;
    ///         };
    ///     @endcode
    ///     The signature of the @b nextLayerUpdater.update() function is
    ///     the same as the signature of this @b update() member function.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @return Status of the update operation.
    template <typename TIter>
    comms::ErrorStatus update(TIter& iter, std::size_t size) const
    {
        Field field;
        auto& derivedObj = static_cast<const TDerived&>(*this);
        return derivedObj.doUpdate(field, iter, size, createNextLayerUpdater());
    }

    /// @brief Update recently written (using write()) message contents data.
    /// @details Similar to other @ref comms::protocol::ProtocolLayerBase::update() "update()"
    ///     but also receiving recently written message object.
    /// @param[in] msg Reference to message object.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @return Status of the update operation.
    template <typename TMsg, typename TIter>
    comms::ErrorStatus update(const TMsg& msg, TIter& iter, std::size_t size) const
    {
        Field field;
        auto& derivedObj = static_cast<const TDerived&>(*this);
        return derivedObj.doUpdate(msg, field, iter, size, createNextLayerUpdater());
    }


    /// @brief Update recently written (using writeFieldsCached()) message data as
    ///     well as cached transport information fields.
    /// @details Very similar to @ref update() member function, but adds "allFields"
    ///     parameter to store raw data of the message.@n
    ///     The function will also invoke the same @b doUpdate() member function
    ///     provided by the derived class, as described with @ref write().
    /// @tparam TIdx Index of the data field in TAllFields.
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @tparam TIter Type of the random access iterator.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in, out] iter Random access iterator to the written data.
    /// @param[in] size Number of bytes that have been written using writeFieldsCached().
    /// @return Status of the update operation.
    template <typename TAllFields, typename TIter>
    ErrorStatus updateFieldsCached(
        TAllFields& allFields,
        TIter& iter,
        std::size_t size) const
    {
        using AllFieldsDecayed = typename std::decay<TAllFields>::type;
        static_assert(util::tupleIsTailOf<AllFields, AllFieldsDecayed>(), "Passed tuple is wrong.");
        static const std::size_t Idx =
            std::tuple_size<AllFieldsDecayed>::value -
                                std::tuple_size<AllFields>::value;

        auto& field = getField<Idx>(allFields);
        auto& derivedObj = static_cast<const TDerived&>(*this);
        return derivedObj.doUpdate(field, iter, size, createNextLayerCachedFieldsUpdater(allFields));
    }

    /// @brief Default implementation of the "update" functaionality.
    /// @details It will be invoked by @ref update() or @ref updateFieldsCached()
    ///     member function, unless the derived class provides its own @ref doUpdate()
    ///     member function to override the default behavior.@n
    ///     This function in this layer does nothing, just advances the iterator
    ///     by the length of the @ref Field.
    /// @tparam TIter Type of iterator used for updating.
    /// @tparam TNextLayerWriter next layer updater object type.
    /// @param[out] field Field that needs to be updated.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using @ref write().
    /// @param[in] nextLayerUpdater Next layer updater object.
    template <typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus doUpdate(
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater) const
    {
        return updateInternal(field, iter, size, std::forward<TNextLayerUpdater>(nextLayerUpdater), LengthTag());
    }

    /// @brief Default implementation of the "update" functaionality.
    /// @details Similar to other @ref comms::protocol::ProtocolLayerBase::doUpdate() "doUpdate()"
    ///     but also receiving recently written message object.
    /// @tparam TMsg Type of message object.
    /// @tparam TIter Type of iterator used for updating.
    /// @tparam TNextLayerWriter next layer updater object type.
    /// @param[in] msg Reference to message object.
    /// @param[out] field Field that needs to be updated.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using @ref write().
    /// @param[in] nextLayerUpdater Next layer updater object.
    template <typename TMsg, typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus doUpdate(
        TMsg& msg,
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater) const
    {
        static_cast<void>(msg);
        auto& derivedObj = static_cast<const TDerived&>(*this);
        return derivedObj.doUpdate(field, iter, size, std::forward<TNextLayerUpdater>(nextLayerUpdater));
    }


    /// @brief Default implementation of field length retrieval.
    static constexpr std::size_t doFieldLength()
    {
        return Field::minLength();
    }

    /// @brief Default implementation of field length retrieval when
    ///     message is known.
    template <typename TMsg>
    static constexpr std::size_t doFieldLength(const TMsg&)
    {
        return doFieldLength();
    }

    /// @brief Create message object given the ID.
    /// @details The default implementation is to forwards this call to the next
    ///     layer. One of the layers (usually comms::protocol::MsgIdLayer)
    ///     hides and overrides this implementation.
    /// @tparam TMsg Type of message ID.
    /// @param id ID of the message.
    /// @param idx Relative index of the message with the same ID.
    /// @return Smart pointer (variant of std::unique_ptr) to allocated message
    ///     object
    template <typename TId>
    MsgPtr createMsg(TId&& id, unsigned idx = 0)
    {
        return nextLayer().createMsg(std::forward<TId>(id), idx);
    }

    /// @brief Access appropriate field from "cached" bundle of all the
    ///     protocol stack fields.
    /// @param allFields All fields of the protocol stack
    /// @return Reference to requested field.
    template <typename TAllFields>
    static auto accessCachedField(TAllFields& allFields) ->
        decltype(std::get<std::tuple_size<typename std::decay<TAllFields>::type>::value - std::tuple_size<AllFields>::value>(allFields))
    {
        using AllFieldsDecayed = typename std::decay<TAllFields>::type;
        static_assert(util::tupleIsTailOf<AllFields, AllFieldsDecayed>(), "Passed tuple is wrong.");
        static const std::size_t Idx =
            std::tuple_size<AllFieldsDecayed>::value -
                                std::tuple_size<AllFields>::value;

        return std::get<Idx>(allFields);
    }

protected:
    /// @brief Detect whether type is actual message object
    /// @tparam T Type of the object
    /// @return @b true if @b T type is extending @b comms::MessageBase,
    ///     @b false otherwise.
    template <typename T>
    static constexpr bool isMessageObjRef()
    {
        return comms::details::hasImplOptions<T>();
    }

    /// @brief Reset msg in case it is a smart pointer (@ref MsgPtr).
    /// @details Does nothing if passed parameter is actual message object.
    /// @see @ref isMessageObjRef().
    template <typename TMsg>
    static void resetMsg(TMsg& msg)
    {
        using Tag =
            typename std::conditional<
                isMessageObjRef<typename std::decay<decltype(msg)>::type>(),
                MessageObjTag,
                SmartPtrTag
            >::type;
        resetMsgInternal(msg, Tag());
    }

    /// @brief Update the missing size information if such is requested.
    /// @details Calculates the minimal required length to be yet read.
    ///     Updates the value reference to which was passed to the
    ///     "read" operation using @ref comms::protocol::missingSize().
    /// @param[in] size Remaining number of bytes in the input buffer.
    /// @param[out] extraValues Variadic parameters passed to the
    ///     "read" function such as @ref comms::protocol::ProtocolLayerBase::read() "read()"
    ///     or @ref comms::protocol::ProtocolLayerBase::readFieldsCached() "readFieldsCached()"
    template <typename... TExtraValues>
    void updateMissingSize(std::size_t size, TExtraValues... extraValues) const
    {
        return updateMissingSizeInternal(size, extraValues...);
    }

    /// @brief Update the missing size information if such is requested.
    /// @details Calculates the minimal required length to be yet read.
    ///     Updates the value reference to which was passed to the
    ///     "read" operation using @ref comms::protocol::missingSize().
    /// @param[in] field Field read operation of which has failed.
    /// @param[in] size Remaining number of bytes in the input buffer.
    /// @param[out] extraValues Variadic parameters passed to the
    ///     "read" function such as @ref comms::protocol::ProtocolLayerBase::read() "read()"
    ///     or @ref comms::protocol::ProtocolLayerBase::readFieldsCached() "readFieldsCached()"
    template <typename... TExtraValues>
    void updateMissingSize(
        const Field& field,
        std::size_t size,
        TExtraValues... extraValues) const
    {
        return updateMissingSizeInternal(field, size, extraValues...);
    }

    /// @brief Set the missing size information if such is requested.
    /// @details Updates the value reference to which was passed to the
    ///     "read" operation using @ref comms::protocol::missingSize().
    /// @param[in] val Value to assign.
    /// @param[out] extraValues Variadic parameters passed to the
    ///     "read" function such as @ref comms::protocol::ProtocolLayerBase::read() "read()"
    ///     or @ref comms::protocol::ProtocolLayerBase::readFieldsCached() "readFieldsCached()"
    template <typename... TExtraValues>
    void setMissingSize(
        std::size_t val,
        TExtraValues... extraValues) const
    {
        return setMissingSizeInternal(val, extraValues...);
    }

    /// @brief Set the message ID information if such is requested.
    /// @details Updates the value reference to which was passed to the
    ///     "read" operation using @ref comms::protocol::msgId().
    /// @param[in] val Value to assign.
    /// @param[out] extraValues Variadic parameters passed to the
    ///     "read" function such as @ref comms::protocol::ProtocolLayerBase::read() "read()"
    ///     or @ref comms::protocol::ProtocolLayerBase::readFieldsCached() "readFieldsCached()"
    template <typename TId, typename... TExtraValues>
    void setMsgId(
        TId val,
        TExtraValues... extraValues) const
    {
        return setMsgIdInternal(val, extraValues...);
    }

    /// @brief Set the message index information if such is requested.
    /// @details Updates the value reference to which was passed to the
    ///     "read" operation using @ref comms::protocol::msgIndex().
    /// @param[in] val Value to assign.
    /// @param[out] extraValues Variadic parameters passed to the
    ///     "read" function such as @ref comms::protocol::ProtocolLayerBase::read() "read()"
    ///     or @ref comms::protocol::ProtocolLayerBase::readFieldsCached() "readFieldsCached()"
    template <typename... TExtraValues>
    void setMsgIndex(
        std::size_t val,
        TExtraValues... extraValues) const
    {
        return setMsgIndexInternal(val, extraValues...);
    }

    /// @brief Retrieve reference to a layer specific field out of
    ///     all fields.
    /// @tparam TIdx Index of the field in tuple
    template <std::size_t TIdx, typename TAllFields>
    static Field& getField(TAllFields& allFields)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be a tuple");
        static_assert(TIdx < std::tuple_size<TAllFields>::value,
                                        "Invalid tuple access index");

        auto& field = std::get<TIdx>(allFields);

        using FieldType = typename std::decay<decltype(field)>::type;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");

        return field;
    }

    /// @cond SKIP_DOC
    struct FixedLengthTag {};
    struct VarLengthTag {};
    using LengthTag = typename std::conditional<
        (Field::minLength() == Field::maxLength()),
        FixedLengthTag,
        VarLengthTag
    >::type;

    class NextLayerReader
    {
    public:
        explicit NextLayerReader(NextLayer& nextLayer)
          : nextLayer_(nextLayer)
        {
        }

        template <typename TMsgPtr, typename TIter, typename... TExtraValues>
        ErrorStatus read(
            TMsgPtr& msg,
            TIter& iter,
            std::size_t size,
            TExtraValues... extraValues)
        {
            return nextLayer_.read(msg, iter, size, extraValues...);
        }
    private:
        NextLayer& nextLayer_;
    };

    class NextLayerUntilDataReader
    {
    public:
        explicit NextLayerUntilDataReader(NextLayer& nextLayer)
          : nextLayer_(nextLayer)
        {
        }

        template <typename TMsgPtr, typename TIter, typename... TExtraValues>
        ErrorStatus read(
            TMsgPtr& msg,
            TIter& iter,
            std::size_t size,
            TExtraValues... extraValues)
        {
            return nextLayer_.readUntilData(msg, iter, size, extraValues...);
        }
    private:
        NextLayer& nextLayer_;
    };

    template <typename TAllFields>
    class NextLayerCachedFieldsReader
    {
    public:
        NextLayerCachedFieldsReader(
            NextLayer& nextLayer,
            TAllFields& allFields)
          : nextLayer_(nextLayer),
            allFields_(allFields)
        {
        }

        template<typename TMsgPtr, typename TIter, typename... TExtraValues>
        ErrorStatus read(
            TMsgPtr& msg,
            TIter& iter,
            std::size_t size,
            TExtraValues... extraValues)
        {
            return nextLayer_.readFieldsCached(allFields_, msg, iter, size, extraValues...);
        }

    private:
        NextLayer& nextLayer_;
        TAllFields& allFields_;
    };

    template <typename TAllFields>
    class NextLayerCachedFieldsUntilDataReader
    {
    public:
        NextLayerCachedFieldsUntilDataReader(
            NextLayer& nextLayer,
            TAllFields& allFields)
          : nextLayer_(nextLayer),
            allFields_(allFields)
        {
        }

        template<typename TMsgPtr, typename TIter, typename... TExtraValues>
        ErrorStatus read(
            TMsgPtr& msg,
            TIter& iter,
            std::size_t size,
            TExtraValues... extraValues)
        {
            return nextLayer_.readUntilDataFieldsCache(allFields_, msg, iter, size, extraValues...);
        }

    private:
        NextLayer& nextLayer_;
        TAllFields& allFields_;
    };

    class NextLayerWriter
    {
    public:

        explicit NextLayerWriter(const NextLayer& nextLayer)
          : nextLayer_(nextLayer)
        {
        }

        template <typename TMsg, typename TIter>
        ErrorStatus write(const TMsg& msg, TIter& iter, std::size_t size) const
        {
            return nextLayer_.write(msg, iter, size);
        }

    private:
        const NextLayer& nextLayer_;
    };

    template <typename TAllFields>
    class NextLayerCachedFieldsWriter
    {
    public:
        NextLayerCachedFieldsWriter(
            const NextLayer nextLayer,
            TAllFields& allFields)
          : nextLayer_(nextLayer),
            allFields_(allFields)
        {
        }

        template <typename TMsg, typename TIter>
        ErrorStatus write(const TMsg& msg, TIter& iter, std::size_t size) const
        {
            return nextLayer_.writeFieldsCached(allFields_, msg, iter, size);
        }

    private:
        const NextLayer& nextLayer_;
        TAllFields& allFields_;
    };

    class NextLayerUpdater
    {
    public:

        explicit NextLayerUpdater(const NextLayer& nextLayer)
          : nextLayer_(nextLayer)
        {
        }

        template <typename TIter>
        ErrorStatus update(TIter& iter, std::size_t size) const
        {
            return nextLayer_.update(iter, size);
        }

    private:
        const NextLayer& nextLayer_;
    };

    template <typename TAllFields>
    class NextLayerCachedFieldsUpdater
    {
    public:
        NextLayerCachedFieldsUpdater(
            const NextLayer nextLayer,
            TAllFields& allFields)
          : nextLayer_(nextLayer),
            allFields_(allFields)
        {
        }

        template <typename TIter>
        ErrorStatus update(TIter& iter, std::size_t size) const
        {
            return nextLayer_.updateFieldsCached(allFields_, iter, size);
        }

    private:
        const NextLayer& nextLayer_;
        TAllFields& allFields_;
    };

    NextLayerReader createNextLayerReader()
    {
        return NextLayerReader(nextLayer_);
    }

    NextLayerUntilDataReader createNextLayerUntilDataReader()
    {
        return NextLayerUntilDataReader(nextLayer_);
    }

    template <typename TAllFields>
    NextLayerCachedFieldsReader<TAllFields>
    createNextLayerCachedFieldsReader(TAllFields& fields)
    {
        return NextLayerCachedFieldsReader<TAllFields>(nextLayer_, fields);
    }

    template <typename TAllFields>
    NextLayerCachedFieldsUntilDataReader<TAllFields>
    createNextLayerCachedFieldsUntilDataReader(TAllFields& fields)
    {
        return NextLayerCachedFieldsUntilDataReader<TAllFields>(nextLayer_, fields);
    }

    NextLayerWriter createNextLayerWriter() const
    {
        return NextLayerWriter(nextLayer_);
    }

    template <typename TAllFields>
    NextLayerCachedFieldsWriter<TAllFields>
    createNextLayerCachedFieldsWriter(TAllFields& fields) const
    {
        return NextLayerCachedFieldsWriter<TAllFields>(nextLayer_, fields);
    }

    NextLayerUpdater createNextLayerUpdater() const
    {
        return NextLayerUpdater(nextLayer_);
    }

    template <typename TAllFields>
    NextLayerCachedFieldsUpdater<TAllFields>
    createNextLayerCachedFieldsUpdater(TAllFields& fields) const
    {
        return NextLayerCachedFieldsUpdater<TAllFields>(nextLayer_, fields);
    }

    /// @endcond
private:

    struct NormalReadTag {};
    struct SplitReadTag {};
    struct MessageObjTag {};
    struct SmartPtrTag {};

    template <typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readInternal(
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        NormalReadTag,
        TExtraValues... extraValues)
    {
        Field field;
        auto& derivedObj = static_cast<TDerived&>(*this);
        return derivedObj.doRead(field, msg, iter, size, createNextLayerReader(), extraValues...);
    }

    template <typename TMsgPtr, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readInternal(
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        SplitReadTag,
        TExtraValues... extraValues)
    {
        auto fromIter = iter;
        auto es = readUntilData(msgPtr, iter, size, extraValues...);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto consumed = static_cast<std::size_t>(std::distance(fromIter, iter));
        COMMS_ASSERT(consumed <= size);
        return readFromData(msgPtr, iter, size - consumed, extraValues...);
    }

    template <typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus updateInternal(
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater,
        FixedLengthTag) const
    {
        auto len = field.length();
        COMMS_ASSERT(len <= size);
        std::advance(iter, len);
        return nextLayerUpdater.update(iter, size - len);
    }

    template <typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus updateInternal(
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater,
        VarLengthTag) const
    {
        auto es = field.read(iter, size);
        if (es == comms::ErrorStatus::Success) {
            es = nextLayerUpdater.update(iter, size - field.length());
        }
        return es;
    }

    template <typename TMsg>
    static void resetMsgInternal(TMsg&, MessageObjTag)
    {
        // Do nothing
    }

    template <typename TMsg>
    static void resetMsgInternal(TMsg& msg, SmartPtrTag)
    {
        msg.reset();
    }

    static void updateMissingSizeInternal(std::size_t size)
    {
        static_cast<void>(size);
    }

    static void updateMissingSizeInternal(const Field& field, std::size_t size)
    {
        static_cast<void>(field);
        static_cast<void>(size);
    }

    template <typename... TExtraValues>
    void updateMissingSizeInternal(
        std::size_t size,
        details::MissingSizeRetriever retriever,
        TExtraValues... extraValues) const
    {
        COMMS_ASSERT(size <= length());
        retriever.setValue(std::max(std::size_t(1U), length() - size));
        updateMissingSizeInternal(size, extraValues...);
    }

    template <typename... TExtraValues>
    void updateMissingSizeInternal(
        const Field& field,
        std::size_t size,
        details::MissingSizeRetriever retriever,
        TExtraValues... extraValues) const
    {
        static_assert(
            details::isMissingSizeRetriever<typename std::decay<decltype(retriever)>::type>(),
            "Must be missing size retriever");
        auto totalLen = field.length() + nextLayer_.length();
        COMMS_ASSERT(size <= totalLen);
        retriever.setValue(std::max(std::size_t(1U), totalLen - size));
        updateMissingSizeInternal(size, extraValues...);
    }

    template <typename T, typename... TExtraValues>
    void updateMissingSizeInternal(
        std::size_t size,
        T retriever,
        TExtraValues... extraValues) const
    {
        static_cast<void>(retriever);
        static_assert(
            !details::isMissingSizeRetriever<typename std::decay<decltype(retriever)>::type>(),
            "Mustn't be missing size retriever");
        updateMissingSizeInternal(size, extraValues...);
    }

    template <typename T, typename... TExtraValues>
    void updateMissingSizeInternal(
        const Field& field,
        std::size_t size,
        T retriever,
        TExtraValues... extraValues) const
    {
        static_assert(
            !details::isMissingSizeRetriever<typename std::decay<decltype(retriever)>::type>(),
            "Mustn't be missing size retriever");
        updateMissingSizeInternal(field, size, extraValues...);
    }


    static void setMissingSizeInternal(std::size_t val)
    {
        static_cast<void>(val);
    }

    template <typename... TExtraValues>
    static void setMissingSizeInternal(
        std::size_t val,
        details::MissingSizeRetriever retriever,
        TExtraValues... extraValues)
    {
        retriever.setValue(val);
        setMissingSizeInternal(val, extraValues...);
    }

    template <typename T, typename... TExtraValues>
    static void setMissingSizeInternal(
        std::size_t val,
        T retriever,
        TExtraValues... extraValues)
    {
        static_cast<void>(retriever);
        static_assert(
            !details::isMissingSizeRetriever<typename std::decay<decltype(retriever)>::type>(),
            "Mustn't be missing size retriever");
        setMissingSizeInternal(val, extraValues...);
    }

    template <typename TId>
    static void setMsgIdInternal(TId val)
    {
        static_cast<void>(val);
    }

    template <typename TId, typename U, typename... TExtraValues>
    static void setMsgIdInternal(
        TId val,
        details::MsgIdRetriever<U> retriever,
        TExtraValues... extraValues)
    {
        retriever.setValue(val);
        setMsgIdInternal(val, extraValues...);
    }

    template <typename TId, typename T, typename... TExtraValues>
    static void setMsgIdInternal(
        TId val,
        T retriever,
        TExtraValues... extraValues)
    {
        static_cast<void>(retriever);
        static_assert(
            !details::isMsgIdRetriever<typename std::decay<decltype(retriever)>::type>(),
            "Mustn't be message id retriever");
        setMsgIdInternal(val, extraValues...);
    }

    static void setMsgIndexInternal(std::size_t val)
    {
        static_cast<void>(val);
    }

    template <typename... TExtraValues>
    static void setMsgIndexInternal(
        std::size_t val,
        details::MsgIndexRetriever retriever,
        TExtraValues... extraValues)
    {
        retriever.setValue(val);
        setMsgIndexInternal(val, extraValues...);
    }

    template <typename T, typename... TExtraValues>
    static void setMsgIndexInternal(
        std::size_t val,
        T retriever,
        TExtraValues... extraValues)
    {
        static_cast<void>(retriever);
        static_assert(
            !details::isMsgIndexRetriever<typename std::decay<decltype(retriever)>::type>(),
            "Mustn't be missing size retriever");
        setMsgIndexInternal(val, extraValues...);
    }

    static_assert (comms::util::IsTuple<AllFields>::Value, "Must be tuple");
    NextLayer nextLayer_;
};

/// @brief Upcast protocol layer in order to have
///     access to its internal types.
template <
    typename TField,
    typename TNextLayer,
    typename TDerived,
    typename...  TOptions>
ProtocolLayerBase<TField, TNextLayer, TDerived, TOptions...>&
toProtocolLayerBase(ProtocolLayerBase<TField, TNextLayer, TDerived, TOptions...>& layer)
{
    return layer;
}

/// @brief Upcast protocol layer in order to have
///     access to its internal types.
template <
    typename TField,
    typename TNextLayer,
    typename TDerived,
    typename...  TOptions>
constexpr
const ProtocolLayerBase<TField, TNextLayer, TDerived, TOptions...>&
toProtocolLayerBase(const ProtocolLayerBase<TField, TNextLayer, TDerived, TOptions...>& layer)
{
    return layer;
}

/// @brief Add "missing size" output parameter to protocol stack's (frame's)
///     "read" operation.
/// @details Can be passed as variadic parameters to "read" functions
///     of protocol stack (see @ref comms::protocol::ProtocolLayerBase::read()
///     and @ref  comms::protocol::ProtocolLayerBase::readFieldsCached()).
///     It can be used to retrieve missing length information in
///     case "read" of protocol stack operation returns @ref comms::ErrorStatus::NotEnoughData.
///     @code
///     using ProtocolStack = ...
///     ProtocolStack stack;
///     ProtocolStack::MsgPtr msg;
///     std::size_t missingSize = 0U;
///     auto es = stack.read(msg, readIter, size, comms::protocol::missingSize(missingSize));
///     if (es == comms::ErrorStatus::NotEnoughData) {
///         ... // missingSize will hold a minimal number of bytes that are required to be yet read
///     }
///     @endcode
/// @param[out] val Missing size value to be returned.
/// @return Implementation dependent object accepted by "read" functions.
/// @see @ref comms::protocol::ProtocolLayerBase::read()
/// @see @ref comms::protocol::ProtocolLayerBase::readFieldsCached()
inline
details::MissingSizeRetriever missingSize(std::size_t& val)
{
    return details::MissingSizeRetriever(val);
}

/// @brief Add "message ID" output parameter to protocol stack's (frame's)
///     "read" operation.
/// @details Can be passed as variadic parameters to "read" functions
///     of protocol stack (see @ref comms::protocol::ProtocolLayerBase::read()
///     and @ref  comms::protocol::ProtocolLayerBase::readFieldsCached()).
///     It can be used to retrieve numeric message ID value.
///     @code
///     using ProtocolStack = ...
///     ProtocolStack stack;
///     ProtocolStack::MsgPtr msg;
///     my_prot::MsgId msgId = my_prot::MsgId();
///     auto es = stack.read(msg, readIter, size, comms::protocol::msgId(msgId));
///     if (es == comms::ErrorStatus::Success) {
///         assert(msg); // Message object must be created
///         ... // use msgId value
///     }
///     @endcode
/// @param[out] val Numeric message ID value to be returned.
/// @return Implementation dependent object accepted by "read" functions.
/// @see @ref comms::protocol::ProtocolLayerBase::read()
/// @see @ref comms::protocol::ProtocolLayerBase::readFieldsCached()
template <typename TId>
details::MsgIdRetriever<TId> msgId(TId& val)
{
    return details::MsgIdRetriever<TId>(val);
}

/// @brief Add "message index" output parameter to protocol stack's (frame's)
///     "read" operation.
/// @details In case the expected input message types tuple contains multiple
///     @b different message classes but having the
///     @b same numeric ID, it may be beneficial to know the reletive index
///     (starting from the first message type having the same ID) of the
///     detected message. This function can be used to retrieve such
///     information. Can be passed as variadic parameters to "read" functions
///     of protocol stack (see @ref comms::protocol::ProtocolLayerBase::read()
///     and @ref  comms::protocol::ProtocolLayerBase::readFieldsCached()).
///     @code
///     using ProtocolStack = ...
///     ProtocolStack stack;
///     ProtocolStack::MsgPtr msg;
///     my_prot::MsgId msgId = my_prot::MsgId();
///     std::size_t msgIndex = 0U;
///     auto es =
///        stack.read(
///            msg,
///            readIter,
///            size,
///            comms::protocol::msgId(msgId),
///            comms::protocol::msgIndex(msgIndex));
///     if (es == comms::ErrorStatus::Success) {
///         assert(msg); // Message object must be created
///         ... // use msgId and msgIndex values
///     }
///     @endcode
/// @param[out] val Message index value to be returned.
/// @return Implementation dependent object accepted by "read" functions.
/// @see @ref comms::protocol::ProtocolLayerBase::read()
/// @see @ref comms::protocol::ProtocolLayerBase::readFieldsCached()
inline
details::MsgIndexRetriever msgIndex(std::size_t& val)
{
    return details::MsgIndexRetriever(val);
}

/// @brief Add "payload start" and "payload size" output parameters to
///     protocol stack's (frame's) "read" operation.
/// @details Can be passed as variadic parameters to "read" functions
///     of protocol stack (see @ref comms::protocol::ProtocolLayerBase::read()
///     and @ref  comms::protocol::ProtocolLayerBase::readFieldsCached()).
///     It can be used to retrieve information on payload start location
///     as well as its size.
///     @code
///     using ProtocolStack = ...
///     ProtocolStack stack;
///     ProtocolStack::MsgPtr msg;
///     decltype(readIter) payloadIter;
///     std::size_t payloadSize = 0U;
///     auto es = stack.read(msg, readIter, size, comms::protocol::msgPayload(payloadIter, payloadSize));
///     ... // use payload location and size information
///     @endcode
/// @param[out] iter Iterator pointing to the begining of the message payload in the
///     input buffer.
/// @param[out] len Number of bytes in the detected payload.
/// @return Implementation dependent object accepted by "read" functions.
/// @see @ref comms::protocol::ProtocolLayerBase::read()
/// @see @ref comms::protocol::ProtocolLayerBase::readFieldsCached()
template <typename TIter>
details::MsgPayloadRetriever<TIter> msgPayload(TIter& iter, std::size_t& len)
{
    return details::MsgPayloadRetriever<TIter>(iter, len);
}

}  // namespace protocol

}  // namespace comms

/// @brief Provide names and convenience access functions to protocol
///     layers.
/// @details The first argument is a name for innermost layer
///     (@ref comms::protocol::MsgDataLayer), while the last one
///     is the name for the outermost one.
/// @related comms::protocol::ProtocolLayerBase
#define COMMS_PROTOCOL_LAYERS_ACCESS(...) \
    COMMS_DO_ACCESS_LAYER_ACC_FUNC(__VA_ARGS__)

/// @brief Same as @ref COMMS_PROTOCOL_LAYERS_ACCESS()
/// @related comms::protocol::ProtocolLayerBase
#define COMMS_PROTOCOL_LAYERS_ACCESS_INNER(...) \
    COMMS_PROTOCOL_LAYERS_ACCESS(__VA_ARGS__)

/// @brief Provide names and convenience access functions to protocol
///     layers.
/// @details Similar to @ref COMMS_PROTOCOL_LAYERS_ACCESS(), but
///     the arguments are expected to be in reverse order, i.e.
///     the first argument is the name of the outermost layer, while
///     the last one is the name for the innermost one
///     (@ref comms::protocol::MsgDataLayer)
/// @related comms::protocol::ProtocolLayerBase
#define COMMS_PROTOCOL_LAYERS_ACCESS_OUTER(...) \
    COMMS_PROTOCOL_LAYERS_ACCESS(COMMS_EXPAND(COMMS_REVERSE_MACRO_ARGS(__VA_ARGS__)))
