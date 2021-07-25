//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file 
/// Contains definition of @ref comms::protocol::TransportValueLayer

#pragma once

#include "ProtocolLayerBase.h"
#include "comms/CompileControl.h"
#include "comms/cast.h"
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"
#include "comms/protocol/details/TransportValueLayerAdapter.h"
#include "comms/protocol/details/TransportValueLayerOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerExtendingClassHelper.h"

COMMS_MSVC_WARNING_PUSH
COMMS_MSVC_WARNING_DISABLE(4189) // Disable erroneous initialized but not referenced variable warning

namespace comms
{

namespace protocol
{

/// @brief Protocol layer that reads a value from transport wrapping and
///     reassigns it to appropriate "extra transport" data member of the
///     created message object.
/// @details Some protocols may put some values, which influence the way
///     of how message contents are read and/or how the message is handled.
///     For example protocol version information. This layer will read
///     the field's value and will re-assign it to specified message object's
///     "extra transport" data member field. This layer requires extra support
///     from the defined message interface object - there is a need to pass
///     @ref comms::option::def::ExtraTransportFields option to the interface definition
///     @ref comms::Message class.
///     This layer is a mid level layer, expects other mid level layer or
///     MsgDataLayer to be its next one.
/// @tparam TField Type of the field that is used read / write extra transport value.
/// @tparam TIdx Index of "extra transport" field that message object contains
///     (accessed via @ref comms::Message::transportFields()).
/// @tparam TNextLayer Next transport layer in protocol stack.
/// @tparam TOptions Extending functionality options. Supported options are:
///     @li @ref comms::option::def::PseudoValue - Mark the handled value to be "pseudo"
///         one, i.e. the field is not getting serialised.
///     @li  @ref comms::option::ExtendingClass - Use this option to provide a class
///         name of the extending class, which can be used to extend existing functionality.
///         See also @ref page_custom_transport_value_layer tutorial page.
/// @headerfile comms/protocol/TransportValueLayer.h
/// @extends ProtocolLayerBase
template <typename TField, std::size_t TIdx, typename TNextLayer, typename... TOptions>
class TransportValueLayer : public
        details::TransportValueLayerAdapterT<
            ProtocolLayerBase<
                TField,
                TNextLayer,
                details::ProtocolLayerExtendingClassT<
                    TransportValueLayer<TField, TIdx, TNextLayer, TOptions...>,
                    details::TransportValueLayerOptionsParser<TOptions...>
                >,
                typename details::TransportValueLayerOptionsParser<TOptions...>::
                    template ForceReadUntilDataSplitIfNeeded<TNextLayer>
            >,
            TOptions...
        >
{
    using ThisClass = TransportValueLayer<TField, TIdx, TNextLayer, TOptions...>;
    using BaseImpl =
        details::TransportValueLayerAdapterT<
            ProtocolLayerBase<
                TField,
                TNextLayer,
                details::ProtocolLayerExtendingClassT<
                    ThisClass,
                    details::TransportValueLayerOptionsParser<TOptions...>
                >,
                typename details::TransportValueLayerOptionsParser<TOptions...>::
                    template ForceReadUntilDataSplitIfNeeded<TNextLayer>
            >,
            TOptions...
        >;
public:
    /// @brief Type of the field object used to read/write "sync" value.
    using Field = typename BaseImpl::Field;

    /// @brief Parsed options
    using TransportParsedOptions = details::TransportValueLayerOptionsParser<TOptions...>;

    /// @brief Default constructor
    TransportValueLayer() = default;

    /// @brief Copy constructor.
    TransportValueLayer(const TransportValueLayer&) = default;

    /// @brief Move constructor.
    TransportValueLayer(TransportValueLayer&&) = default;

    /// @brief Destructor
    ~TransportValueLayer() noexcept = default;

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details Reads the value from the input data and assigns it to appropriate
    ///     extra transport field inside the message object (accessed via
    ///     comms::Message::transportFields()). @n
    ///     Note, that this operation works fine even if message object is created
    ///     after reading the transport value. There is "inner magic" that causes
    ///     read operation to proceed until @b DATA layer
    ///     (implemented by @ref comms::protocol::MsgDataLayer), assigns the
    ///     read value to message object, then proceeds to reading the message
    ///     contents, i.e. when @ref comms::Message::read() function is invoked
    ///     the message object already has the value of the transport field updated.
    /// @tparam TMsg Type of the @b msg parameter.
    /// @tparam TIter Type of iterator used for reading.
    /// @tparam TNextLayerReader next layer reader object type.
    /// @param[out] field Field object to read.
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[in] nextLayerReader Reader object, needs to be invoked to
    ///     forward read operation to the next layer.
    /// @param[out] extraValues Variadic extra output parameters passed to the
    ///     "read" operatation of the protocol stack (see
    ///     @ref comms::protocol::ProtocolLayerBase::read() "read()" and
    ///     @ref comms::protocol::ProtocolLayerBase::readFieldsCached() "readFieldsCached()").
    ///     Need to passed on as variadic arguments to the @b nextLayerReader.
    /// @return Status of the read operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    comms::ErrorStatus doRead(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        TExtraValues... extraValues)
    {
        auto es = readFieldInternal(field, msg, iter, size, ValueTag<>(), extraValues...);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        static constexpr bool ForcedReadUntilDataSplit = 
            BaseImpl::ParsedOptions::HasForceReadUntilDataSplit;

        if (ForcedReadUntilDataSplit) {
            es = nextLayerReader.read(msg, iter, size, extraValues...);

            if (es != comms::ErrorStatus::Success) {
                return es;
            }
        }

        auto& thisObj = BaseImpl::thisLayer();
        auto* msgPtr = BaseImpl::toMsgPtr(msg);
        bool success = thisObj.reassignFieldValueToMsg(field, msgPtr);
        if (!success) {
            return comms::ErrorStatus::ProtocolError;
        }

        if (!ForcedReadUntilDataSplit) {
            es = nextLayerReader.read(msg, iter, size, extraValues...);
        }

        return es;
    }

    /// @brief Customized write functionality, invoked by @ref write().
    /// @details The function will write the appriprate extra transport value
    ///     held by the message object being written.
    /// @tparam TMsg Type of message object.
    /// @tparam TIter Type of iterator used for writing.
    /// @tparam TNextLayerWriter next layer writer object type.
    /// @param[out] field Field object to update and write.
    /// @param[in] msg Reference to message object
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Max number of bytes that can be written.
    /// @param[in] nextLayerWriter Next layer writer object.
    /// @return Status of the write operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       written. In case of an error, distance between original position
    ///       and advanced will pinpoint the location of the error.
    template <typename TMsg, typename TIter, typename TNextLayerWriter>
    comms::ErrorStatus doWrite(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter) const
    {
        auto& thisObj = BaseImpl::thisLayer();
        thisObj.prepareFieldForWrite(msg, field);

        auto es = writeFieldInternal(field, msg, iter, size, ValueTag<>());
        if (es != ErrorStatus::Success) {
            return es;
        }

        return nextLayerWriter.write(msg, iter, size);
    }

    /// @brief Customising field length calculation
    /// @details If the layer is marked as "pseudo" (using @ref comms::option::def::PseudoValue)
    ///     option, then the report length is 0.
    static constexpr std::size_t doFieldLength()
    {
         return doFieldLengthInternal(ValueTag<>());
    }

    /// @brief Customising field length calculation
    /// @details If the layer is marked as "pseudo" (using @ref comms::option::def::PseudoValue)
    ///     option, then the report length is 0.
    template <typename TMsg>
    static std::size_t doFieldLength(const TMsg&)
    {
        return doFieldLength();
    }

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Access to pseudo field stored internally.
    /// @details The function exists only if @ref comms::option::def::PseudoValue
    ///     option has been used.
    Field& pseudoField();

    /// @brief Const access to pseudo field stored internally.
    /// @details The function exists only if @ref comms::option::def::PseudoValue
    ///     option has been used.
    const Field& pseudoField() const;
#endif

protected:
    /// @brief Re-assign the value from the input field to appropriate transport field
    ///     in the message object.
    /// @details Default implementation just assigns to the field accessed using
    ///     @b TIdx index passed as the template argument to the class definition.@n
    ///     May be overridden by the extending class if some complex functionality is required.
    /// @param[out] msg Reference to the created message object
    /// @param[in] field Field, value of which needs to be re-assigned
    /// @deprecated Use @ref comms::protocol::TransportValueLayer::reassignFieldValueToMsg() "reassignFieldValueToMsg()"
    ///     instead.
    template <typename TMsg>
    static void reassignFieldValue(TMsg& msg, const Field& field)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasTransportFields(),
            "Message interface class hasn't defined transport fields, "
            "use comms::option::def::ExtraTransportFields option.");
        static_assert(TIdx < std::tuple_size<typename MsgType::TransportFields>::value,
            "TIdx is too big, exceeds the amount of transport fields defined in interface class");

        auto& allTransportFields = msg.transportFields();
        auto& transportField = std::get<TIdx>(allTransportFields);

        using FieldType = typename std::decay<decltype(transportField)>::type;
        transportField = comms::field_cast<FieldType>(field);
    }

    /// @brief Re-assign the value from the input field to appropriate transport field
    ///     in the message object.
    /// @details Default implementation just assigns to the field accessed using
    ///     @b TIdx index passed as the template argument to the class definition.@n
    ///     May be overridden by the extending class if some complex functionality is required.
    /// @param[in] field Field, value of which needs to be re-assigned
    /// @param[in, out] msgPtr Pointer to the created message object
    /// @return @b true in case of successful operation, @b false othewise @n
    ///     In case @b false is returned, 
    ///     the @ref comms::protocol::TransportValueLayer::doRead() "doRead()"
    ///     member function will return @ref comms::ErrorStatus::ProtocolError.
    /// @note May be non-static in the extending class
    template <typename TMsg>
    bool reassignFieldValueToMsg(const Field& field, TMsg* msgPtr)
    {
        if (msgPtr == nullptr) {
            return false;
        }

        auto& thisObj = BaseImpl::thisLayer();
        thisObj.reassignFieldValue(*msgPtr, field);
        return true;
    }

    /// @brief Prepare field for writing.
    /// @details Copies the field value from the appropriate transport field of the 
    ///     message object and assigns it to the provided output field. @n
    ///     May be overridden by the extending class if some complex functionality is required.
    /// @param[in] msg Reference to message object being written
    /// @param[out] field Field, value of which needs to be populated
    /// @note May be non-static in the extending class
    template <typename TMsg>
    static void prepareFieldForWrite(const TMsg& msg, Field& field)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasTransportFields(),
            "Message interface class hasn't defined transport fields, "
            "use comms::option::def::ExtraTransportFields option.");        
        static_assert(TIdx < std::tuple_size<typename MsgType::TransportFields>::value,
            "TIdx is too big, exceeds the amount of transport fields defined in interface class");

        auto& transportField = std::get<TIdx>(msg.transportFields());
        field = comms::field_cast<Field>(transportField);
    }    

private:

    template <typename... TParams>
    using PseudoValueTag = comms::details::tag::Tag1<>;   

    template <typename... TParams>
    using NormalValueTag = comms::details::tag::Tag2<>;          

    template <typename...>
    using ValueTag =
        typename comms::util::LazyShallowConditional<
            TransportParsedOptions::HasPseudoValue
        >::template Type<
            PseudoValueTag,
            NormalValueTag
        >;


    template <typename... TParams>
    static constexpr std::size_t doFieldLengthInternal(PseudoValueTag<TParams...>)
    {
        return 0U;
    }

    template <typename... TParams>
    static constexpr std::size_t doFieldLengthInternal(NormalValueTag<TParams...>)
    {
        return BaseImpl::doFieldLength();
    }

    template <typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readFieldInternal(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t& len,
        PseudoValueTag<>,
        TExtraValues...)
    {
        static_cast<void>(msg);
        static_cast<void>(iter);
        static_cast<void>(len);
        field = BaseImpl::pseudoField();
        return comms::ErrorStatus::Success;
    }

    template <typename TMsg, typename TIter, typename... TExtraValues>
    comms::ErrorStatus readFieldInternal(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t& len,
        NormalValueTag<>,
        TExtraValues... extraValues)
    {
        auto& thisObj = BaseImpl::thisLayer();
        auto* msgPtr = BaseImpl::toMsgPtr(msg);
        auto beforeReadIter = iter;
        
        auto es = thisObj.doReadField(msgPtr, field, iter, len);
        if (es == comms::ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, len, extraValues...);
        }
        else {
            auto fieldLen = static_cast<std::size_t>(std::distance(beforeReadIter, iter));
            len -= fieldLen;
        }
        return es;
    }

    template <typename TMsg, typename TIter, typename... TParams>
    comms::ErrorStatus writeFieldInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t& len,
        PseudoValueTag<TParams...>) const
    {
        static_cast<void>(msg);
        static_cast<void>(iter);
        static_cast<void>(len);
        field = BaseImpl::pseudoField();
        return comms::ErrorStatus::Success;
    }

    template <typename TMsg, typename TIter, typename... TParams>
    comms::ErrorStatus writeFieldInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t& len,
        NormalValueTag<TParams...>) const
    {
        auto& thisObj = BaseImpl::thisLayer();
        auto es = thisObj.doWriteField(&msg, field, iter, len);
        if (es == comms::ErrorStatus::Success) {
            COMMS_ASSERT(field.length() <= len);
            len -= field.length();
        }
        return es;
    }
};

namespace details
{
template <typename T>
struct TransportValueLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField, std::size_t TIdx, typename TNextLayer>
struct TransportValueLayerCheckHelper<TransportValueLayer<TField, TIdx, TNextLayer> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref TransportValueLayer
/// @related TransportValueLayer
template <typename T>
constexpr bool isTransportValueLayer()
{
    return details::TransportValueLayerCheckHelper<T>::Value;
}

}  // namespace protocol

}  // namespace comms

COMMS_MSVC_WARNING_POP
