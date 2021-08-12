//
// Copyright 2014 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file 
/// Contains definition of @ref comms::protocol::MsgSizeLayer

#pragma once

#include <iterator>
#include <type_traits>

#include "comms/CompileControl.h"
#include "comms/field/IntValue.h"
#include "comms/protocol/details/ProtocolLayerBase.h"
#include "comms/protocol/details/MsgSizeLayerOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerExtendingClassHelper.h"
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"

COMMS_MSVC_WARNING_PUSH
COMMS_MSVC_WARNING_DISABLE(4189) // Disable erroneous initialized but not referenced variable warning

namespace comms
{

namespace protocol
{

namespace details
{
template <bool TValidPtr>
struct MsgSizeLayerConstNullPtrCastHelper
{
    template <typename TPtr>
    using Type = const typename TPtr::element_type*;
};

template <>
struct MsgSizeLayerConstNullPtrCastHelper<false>
{
    template <typename TPtr>
    using Type = const void*;
};

} // namespace details    

/// @brief Protocol layer that uses size field as a prefix to all the
///        subsequent data written by other (next) layers.
/// @details The main purpose of this layer is to provide information about
///     the remaining size of the serialised message. This layer is a mid level
///     layer, expects other mid level layer or MsgDataLayer to be its next one.
/// @tparam TField Type of the field that describes the "size" field.
/// @tparam TNextLayer Next transport layer in protocol stack.
/// @tparam TOptions Default functionality extension options. Supported options are:
///     @li  @ref comms::option::ExtendingClass - Use this option to provide a class
///         name of the extending class, which can be used to extend existing functionality.
///         See also @ref page_custom_size_layer tutorial page.
/// @headerfile comms/protocol/MsgSizeLayer.h
template <typename TField, typename TNextLayer, typename... TOptions>
class MsgSizeLayer : public
        details::ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                MsgSizeLayer<TField, TNextLayer, TOptions...>, 
                details::MsgSizeLayerOptionsParser<TOptions...>
            >,
            comms::option::ProtocolLayerDisallowReadUntilDataSplit
        >
{
    using BaseImpl =
        details::ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                MsgSizeLayer<TField, TNextLayer, TOptions...>, 
                details::MsgSizeLayerOptionsParser<TOptions...>
            >,
            comms::option::ProtocolLayerDisallowReadUntilDataSplit
        >;
public:
    /// @brief Type of the field object used to read/write remaining size value.
    using Field = typename BaseImpl::Field;

    /// @brief Default constructor
    explicit MsgSizeLayer() = default;

    /// @brief Copy constructor
    MsgSizeLayer(const MsgSizeLayer&) = default;

    /// @brief Move constructor
    MsgSizeLayer(MsgSizeLayer&&) = default;

    /// @brief Destructor.
    ~MsgSizeLayer() noexcept = default;

    /// @brief Copy assignment.
    MsgSizeLayer& operator=(const MsgSizeLayer&) = default;

    /// @brief Move assignment.
    MsgSizeLayer& operator=(MsgSizeLayer&&) = default;

    /// @cond SKIP_DOC

    static constexpr std::size_t doFieldLength()
    {
        return BaseImpl::doFieldLength();
    }

    template <typename TMsg>
    constexpr std::size_t doFieldLength(const TMsg& msg) const
    {
        return fieldLengthInternal(msg, LengthTag<>());
    }
    /// @endcond

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details Reads size of the subsequent data from the input data sequence
    ///          and calls read() member function of the next layer with
    ///          the size specified in the size field. The function will also
    ///          compare the provided size of the data with value
    ///          read from the buffer. If the latter is greater than
    ///          former, comms::ErrorStatus::NotEnoughData will be returned.
    ///          However, if buffer contains enough data, but the next layer
    ///          reports it's not enough (returns comms::ErrorStatus::NotEnoughData),
    ///          comms::ErrorStatus::ProtocolError will be returned.
    /// @tparam TMsg Type of @b msg parameter.
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
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(
            std::is_base_of<std::random_access_iterator_tag, IterTag>::value,
            "Current implementation of MsgSizeLayer requires iterator used for reading to be random-access one.");

        auto begIter = iter;
        auto* msgPtr = BaseImpl::toMsgPtr(msg);
        auto& thisObj = BaseImpl::thisLayer();
        auto es = thisObj.doReadField(msgPtr, field, iter, size);        
        if (es == ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, extraValues...);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        auto fromIter = iter;
        auto readFieldLength = static_cast<std::size_t>(std::distance(begIter, iter));
        std::size_t actualRemainingSize = (size - readFieldLength);
        std::size_t requiredRemainingSize = thisObj.getRemainingSizeFromField(field);

        if (actualRemainingSize < requiredRemainingSize) {
            BaseImpl::setMissingSize(requiredRemainingSize - actualRemainingSize, extraValues...);
            return ErrorStatus::NotEnoughData;
        }

        thisObj.beforeRead(field, msgPtr);
        es = nextLayerReader.read(msg, iter, requiredRemainingSize, extraValues...);
        if (es == ErrorStatus::NotEnoughData) {
            BaseImpl::resetMsg(msg);
            return ErrorStatus::ProtocolError;
        }

        if (es != ErrorStatus::ProtocolError) {
            iter = fromIter;
            std::advance(iter, requiredRemainingSize);
        }

        auto consumed =
            static_cast<std::size_t>(std::distance(fromIter, iter));
        if (consumed < requiredRemainingSize) {
            auto diff = requiredRemainingSize - consumed;
            std::advance(iter, diff);
        }
        return es;
    }


    /// @brief Customized write functionality, invoked by @ref write().
    /// @details The function will write number of bytes required to serialise
    ///     the message, then invoke the write() member function of the next
    ///     layer. The calculation of the required length is performed by invoking
    ///     "length(msg)".
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
    ErrorStatus doWrite(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter) const
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        return writeInternal(field, msg, iter, size, std::forward<TNextLayerWriter>(nextLayerWriter), MsgLengthTag<MsgType>());
    }

    /// @brief Customized update functionality, invoked by @ref update().
    /// @details Should be called when @ref doWrite() returns comms::ErrorStatus::UpdateRequired.
    /// @tparam TIter Type of iterator used for updating.
    /// @tparam TNextLayerWriter next layer updater object type.
    /// @param[out] field Field object to update.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @param[in] nextLayerUpdater Next layer updater object.
    /// @return Status of the update operation.
    template <typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus doUpdate(
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater) const
    {
        using LocalMsgPtr = typename BaseImpl::MsgPtr;
        using ConstNullptrType = 
            typename details::MsgSizeLayerConstNullPtrCastHelper<
                !std::is_void<LocalMsgPtr>::value
            >::template Type<LocalMsgPtr>;
        auto noMsgPtr = static_cast<ConstNullptrType>(nullptr);
        return doUpdateInternal(noMsgPtr, field, iter, size, std::forward<TNextLayerUpdater>(nextLayerUpdater), NoMsgTypeTag<>());
    }

    /// @brief Customized update functionality, invoked by @ref update().
    /// @details Similar to other @ref comms::protocol::MsgSizeLayer::doUpdate() "doUpdate()",
    ///     but receiving reference to valid message object.
    /// @param[in] msg Reference to valid message object.
    /// @param[out] field Field object to update.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @param[in] nextLayerUpdater Next layer updater object.
    /// @return Status of the update operation.
    template <typename TMsg, typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus doUpdate(
        const TMsg& msg,
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater) const
    {
        return doUpdateInternal(&msg, field, iter, size, std::forward<TNextLayerUpdater>(nextLayerUpdater), ValidMsgTypeTag<>());
    }

protected:
    /// @brief Retrieve remaining size (length) from the field.
    /// @details May be overridden by the extending class
    /// @param[in] field Field for this layer.
    static std::size_t getRemainingSizeFromField(const Field& field)
    {
        static_assert(comms::field::isIntValue<Field>(),
            "Field must be of IntValue type");

        return static_cast<std::size_t>(field.value());
    }

    /// @brief Extra operation before read is forwarded to the next layer.
    /// @details Default implementation does nothing, may be overriden in the
    ///     derived class.
    /// @param[in] field Field of the layer that was successfully read.
    /// @param[in, out] msg Pointer to message object, either interface
    ///     class or message object itself (depending on how doRead() was invoked).
    ///     Can be @b nullptr in case message object hasn't been created yet
    /// @note May be non-static in the extending class
    template <typename TMsg>
    static void beforeRead(const Field& field, TMsg* msg)
    {
        static_cast<void>(field);
        static_cast<void>(msg);
    }

    /// @brief Prepare field for writing
    /// @details Must assign provided size (length) value. 
    ///     May be overridden by the extending class if some complex functionality is required.
    /// @param[in] size Size value to assign
    /// @param[in] msg Pointer to message object being written, maybe nullptr (in case invoked
    ///     from @ref comms::protocol::MsgSizeLayer::doUpdate "doUpdate()")
    /// @param[out] field Field, value of which needs to be populated
    /// @note May be non-static in the extending class
    template <typename TMsg>
    static void prepareFieldForWrite(std::size_t size, const TMsg* msg, Field& field)
    {
        static_assert(
            comms::field::isIntValue<Field>(),
            "Field must be of IntValue or EnumValue types");

        static_cast<void>(msg);
        field.value() = static_cast<typename Field::ValueType>(size);
    }

private:
    template <typename... TParams>
    using FixedLengthTag = typename BaseImpl::template FixedLengthTag<TParams...>;

    template <typename...TParams>
    using VarLengthTag = typename BaseImpl::template VarLengthTag<TParams...>;

    template <typename... TParams>
    using LengthTag = typename BaseImpl::template LengthTag<TParams...>;

    template <typename... TParams>
    using MsgHasLengthTag = comms::details::tag::Tag3<>;  

    template <typename... TParams>
    using MsgNoLengthTag = comms::details::tag::Tag4<>;   

    template <typename... TParams>
    using ValidMsgTypeTag = comms::details::tag::Tag5<>;     

    template <typename... TParams>
    using NoMsgTypeTag = comms::details::tag::Tag6<>;         

    template<typename TMsg>
    using MsgLengthTag =
        typename comms::util::LazyShallowConditional<
            details::ProtocolLayerHasFieldsImpl<TMsg>::Value || TMsg::InterfaceOptions::HasLength
        >::template Type<
            MsgHasLengthTag,
            MsgNoLengthTag
        >;

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalHasLength(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        std::size_t lenValue = BaseImpl::nextLayer().length(msg);
        auto& thisObj = BaseImpl::thisLayer();

        thisObj.prepareFieldForWrite(lenValue, &msg, field);
        auto es = thisObj.doWriteField(&msg, field, iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(field.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field.length());
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalRandomAccess(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto valueIter = iter;
        auto& thisObj = BaseImpl::thisLayer();
        thisObj.prepareFieldForWrite(0U, &msg, field);
        auto es = thisObj.doWriteField(&msg, field, iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        auto dataIter = iter;

        auto sizeLen = field.length();
        es = nextLayerWriter.write(msg, iter, size - sizeLen);
        if (es != ErrorStatus::Success) {
            return es;
        }

        auto dist = static_cast<std::size_t>(std::distance(dataIter, iter));
        thisObj.prepareFieldForWrite(dist, &msg, field);
        COMMS_ASSERT(field.length() == sizeLen);
        return thisObj.doWriteField(&msg, field, valueIter, sizeLen);
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalOutput(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto& thisObj = BaseImpl::thisLayer();
        thisObj.prepareFieldForWrite(0U, &msg, field);
        auto es = thisObj.doWriteField(&msg, field, iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        es = nextLayerWriter.write(msg, iter, size - field.length());
        if ((es != ErrorStatus::Success) &&
            (es != ErrorStatus::UpdateRequired)) {
            return es;
        }

        return ErrorStatus::UpdateRequired;
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalNoLengthTagged(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        std::random_access_iterator_tag) const
    {
        return writeInternalRandomAccess(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalNoLengthTagged(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        std::output_iterator_tag) const
    {
        return writeInternalOutput(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalNoLength(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        static_assert(
            (Field::minLength() == Field::maxLength()) ||
            (comms::isMessageBase<typename std::decay<decltype(msg)>::type>()),
                "Unable to perform write with size field having variable length and "
                "no polymorphic length calculation available.");
        using IterType = typename std::decay<decltype(iter)>::type;
        using Tag = typename std::iterator_traits<IterType>::iterator_category;
        return writeInternalNoLengthTagged(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter), Tag());
    }

    template <typename TMsg, typename TIter, typename TWriter, typename... TParams>
    ErrorStatus writeInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        MsgHasLengthTag<TParams...>) const
    {
        return writeInternalHasLength(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TMsg, typename TIter, typename TWriter, typename... TParams>
    ErrorStatus writeInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        MsgNoLengthTag<TParams...>) const
    {
        return writeInternalNoLength(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TMsg, typename... TParams>
    constexpr std::size_t fieldLengthInternal(const TMsg& msg, FixedLengthTag<TParams...>) const
    {
        return BaseImpl::doFieldLength(msg);
    }

    template <typename TMsg, typename... TParams>
    std::size_t fieldLengthInternal(const TMsg& msg, VarLengthTag<TParams...>) const
    {
        auto& thisObj = BaseImpl::thisLayer();
        auto remSize = BaseImpl::nextLayer().length(msg);
        Field fieldTmp;
        thisObj.prepareFieldForWrite(remSize, &msg, fieldTmp);
        return fieldTmp.length();
    }

    template <typename TMsg, typename TIter, typename TNextLayerUpdater, typename TForwardTag>
    comms::ErrorStatus doUpdateInternal(
        const TMsg* msg,
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater,
        TForwardTag&& tag) const
    {
        std::size_t lenValue = size - Field::maxLength();
        auto& thisObj = BaseImpl::thisLayer();
        thisObj.prepareFieldForWrite(lenValue, msg, field);

        if (field.length() != Field::maxLength()) {
            lenValue = size - field.length();
            thisObj.prepareFieldForWrite(lenValue, msg, field);
        }

        auto es = thisObj.doWriteField(msg, field, iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        return doUpdateForward(msg, iter, size - field.length(), std::forward<TNextLayerUpdater>(nextLayerUpdater), tag);
    }

    template <typename TMsg, typename TIter, typename TNextLayerUpdater, typename... TParams>
    comms::ErrorStatus doUpdateForward(
        const TMsg* msg,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater,
        NoMsgTypeTag<TParams...>) const
    {
        static_cast<void>(msg);
        return nextLayerUpdater.update(iter, size);
    } 

    template <typename TMsg, typename TIter, typename TNextLayerUpdater, typename... TParams>
    comms::ErrorStatus doUpdateForward(
        const TMsg* msg,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater,
        ValidMsgTypeTag<TParams...>) const
    {
        COMMS_ASSERT(msg != nullptr);
        return nextLayerUpdater.update(*msg, iter, size);
    }        
};

namespace details
{
template <typename T>
struct MsgSizeLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField, typename TNextLayer>
struct MsgSizeLayerCheckHelper<MsgSizeLayer<TField, TNextLayer> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref MsgSizeLayer
/// @related MsgSizeLayer
template <typename T>
constexpr bool isMsgSizeLayer()
{
    return details::MsgSizeLayerCheckHelper<T>::Value;
}

}  // namespace protocol

}  // namespace comms

COMMS_MSVC_WARNING_POP