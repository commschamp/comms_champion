//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "ProtocolLayerBase.h"

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
/// @headerfile comms/protocol/SyncPrefixLayer.h
template <typename TField, typename TNextLayer>
class SyncPrefixLayer : public
        ProtocolLayerBase<
            TField,
            TNextLayer,
            SyncPrefixLayer<TField, TNextLayer>
        >
{
    using BaseImpl =
        ProtocolLayerBase<
            TField,
            TNextLayer,
            SyncPrefixLayer<TField, TNextLayer>
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
        auto beforeReadIter = iter;
        auto es = field.read(iter, size);
        if (es == comms::ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, extraValues...);
        }

        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (field != Field()) {
            // doesn't match expected
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
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(field.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field.length());
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


