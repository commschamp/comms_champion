//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file 
/// Contains definition of @ref comms::protocol::SyncPrefixLayer

#pragma once

#include "comms/protocol/details/ProtocolLayerBase.h"
#include "comms/protocol/details/SyncPrefixLayerOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerExtendingClassHelper.h"

namespace comms
{

namespace protocol
{

/// @brief Protocol layer that uses "sync" field as a prefix to all the
///        subsequent data written by other (next) layers.
/// @details The main purpose of this layer is to provide a constant synchronisation
///     prefix to help identify the beginning of the serialised message.
///     This layer is a mid level layer, expects other mid level layer or
///     MsgDataLayer to be its next one.
/// @tparam TField Type of the field that is used as sync prefix. The "sync"
///     field type definition must use options (@ref comms::option::def::DefaultNumValue)
///     to specify its default value to be equal to the expected "sync" value.
/// @tparam TNextLayer Next transport layer in protocol stack.
/// @tparam TOptions Extending functionality options. Supported options are:
///     @li  @ref comms::option::ExtendingClass - Use this option to provide a class
///         name of the extending class, which can be used to extend existing functionality.
///         See also @ref page_custom_sync_prefix_layer.
/// @headerfile comms/protocol/SyncPrefixLayer.h
template <typename TField, typename TNextLayer, typename... TOptions>
class SyncPrefixLayer : public
        details::ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                SyncPrefixLayer<TField, TNextLayer, TOptions...>,
                details::SyncPrefixLayerOptionsParser<TOptions...>
            >            
        >
{
    using BaseImpl =
        details::ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                SyncPrefixLayer<TField, TNextLayer, TOptions...>,
                details::SyncPrefixLayerOptionsParser<TOptions...>
            >            
        >;

public:
    /// @brief Type of the field object used to read/write "sync" value.
    using Field = typename BaseImpl::Field;

    /// @brief Default constructor
    SyncPrefixLayer() = default;

    /// @brief Copy constructor.
    SyncPrefixLayer(const SyncPrefixLayer&) = default;

    /// @brief Move constructor.
    SyncPrefixLayer(SyncPrefixLayer&&) = default;

    /// @brief Destructor
    ~SyncPrefixLayer() noexcept = default;

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details Reads the "sync" value from the input data. If the read value
    ///     is NOT as expected (doesn't equal to the default constructed
    ///     @ref Field), then comms::ErrorStatus::ProtocolError is returned.
    ////    If the read "sync" value as expected, the read() member function of
    ///     the next layer is called.
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
        auto& thisObj = BaseImpl::thisLayer();
        auto* msgPtr = BaseImpl::toMsgPtr(msg);
        auto beforeReadIter = iter;

        auto es = thisObj.doReadField(msgPtr, field, iter, size);
        if (es == comms::ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, extraValues...);
        }

        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        bool verified = thisObj.verifyFieldValue(field);
        if (!verified) {
            return comms::ErrorStatus::ProtocolError;
        }

        auto fieldLen = static_cast<std::size_t>(std::distance(beforeReadIter, iter));
        return nextLayerReader.read(msg, iter, size - fieldLen, extraValues...);
    }

    /// @brief Customized write functionality, invoked by @ref write().
    /// @details The function will write proper "sync" value to the output
    ///     buffer, then call the write() function of the next layer.
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
        thisObj.prepareFieldForWrite(field);
        auto es = thisObj.doWriteField(&msg, field, iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(field.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field.length());
    }

protected:
    /// @brief Verify the validity of the field.
    /// @details Default implementation compares read field with default constructed Field type. @n
    ///     May be overridden by the extending class in case
    ///     more complex logic is required.
    /// @param[out] field Field that has been read.
    /// @note May be non-static in the extending class
    static bool verifyFieldValue(const Field& field)
    {
        return field == Field();
    }    

    /// @brief Prepare field for writing.
    /// @details Default implementation does nothing. @n
    ///     May be overridden by the extending class in case
    ///     more complex logic is required.
    /// @param[out] field Field, default value of which needs to be (re)populated
    /// @note May be non-static in the extending class, but must be const, use
    ///     mutable member data in case it needs to be updated.
    static void prepareFieldForWrite(Field& field)
    {
        static_cast<void>(field);
    }
};

namespace details
{
template <typename T>
struct SyncPrefixLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField, typename TNextLayer>
struct SyncPrefixLayerCheckHelper<SyncPrefixLayer<TField, TNextLayer> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref SyncPrefixLayer
/// @related SyncPrefixLayer
template <typename T>
constexpr bool isSyncPrefixLayer()
{
    return details::SyncPrefixLayerCheckHelper<T>::Value;
}

}  // namespace protocol

}  // namespace comms


