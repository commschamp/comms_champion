//
// Copyright 2015 - 2017 (C). Alex Robenko. All rights reserved.
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
///     field type definition must use options (comms::option::DefaultNumValue)
///     to specify its default value to be equal to the expected "sync" value.
/// @tparam TNextLayer Next transport layer in protocol stack.
template <typename TField, typename TNextLayer>
class SyncPrefixLayer : public ProtocolLayerBase<TField, TNextLayer>
{
    typedef ProtocolLayerBase<TField, TNextLayer> Base;

public:
    /// @brief Type of smart pointer that will hold allocated message object.
    typedef typename Base::MsgPtr MsgPtr;

    /// @brief Type of the message interface.
    /// @details Initially provided to MsgDataLayer and propagated through all
    ///     the layers in between to this class.
    typedef typename Base::Message Message;

    /// @brief Type of read iterator.
    /// @details Initially provided to comms::Message through options, then it finds
    ///     its way as a type in MsgDataLayer, and finally propagated through all
    ///     the layers in between to this class
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Type of write iterator
    /// @details Initially provided to comms::Message through options, then it finds
    ///     its way as a type in MsgDataLayer, and finally propagated through all
    ///     the layers in between to this class.
    typedef typename Base::WriteIterator WriteIterator;

    /// @brief Type of the field object used to read/write "sync" value.
    typedef typename Base::Field Field;

    /// @brief Default constructor
    SyncPrefixLayer() = default;

    /// @brief Copy constructor.
    SyncPrefixLayer(const SyncPrefixLayer&) = default;

    /// @brief Move constructor.
    SyncPrefixLayer(SyncPrefixLayer&&) = default;

    /// @brief Destructor
    ~SyncPrefixLayer() = default;

    /// @brief Deserialise message from the input data sequence.
    /// @details Reads the "sync" value from the input data. If the read value
    ///     is NOT as expected (doesn't equal to the default constructed
    ///     @ref Field), then comms::ErrorStatus::ProtocolError is returned.
    ////    If the read "sync" value as expected, the read() member function of
    ///     the next layer is called.
    /// @param[in, out] msgPtr Reference to smart pointer that already holds or
    ///     will hold allocated message object
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] missingSize If not nullptr and return value is
    ///     comms::ErrorStatus::NotEnoughData it will contain
    ///     minimal missing data length required for the successful
    ///     read attempt.
    /// @return Status of the read operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post missingSize output value is updated if and only if function
    ///       returns comms::ErrorStatus::NotEnoughData.
    template <typename TMsgPtr, typename TIter>
    ErrorStatus read(
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        Field field;
        return
            readInternal(
                field,
                msgPtr,
                iter,
                size,
                missingSize,
                Base::createNextLayerReader());
    }

    /// @brief Deserialise message from the input data sequence while caching
    ///     the read transport information fields.
    /// @details Very similar to read() member function, but adds "allFields"
    ///     parameter to store read transport information fields.
    /// @tparam TIdx Index of the message ID field in TAllFields tuple.
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in] msgPtr Reference to the smart pointer holding message object.
    /// @param[in, out] iter Iterator used for reading.
    /// @param[in] size Number of bytes available for reading.
    /// @param[out] missingSize If not nullptr and return value is
    ///             comms::ErrorStatus::NotEnoughData it will contain
    ///             minimal missing data length required for the successful
    ///             read attempt.
    /// @return Status of the operation.
    template <std::size_t TIdx, typename TAllFields, typename TMsgPtr, typename TIter>
    ErrorStatus readFieldsCached(
        TAllFields& allFields,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        auto& field = Base::template getField<TIdx>(allFields);

        return
            readInternal(
                field,
                msgPtr,
                iter,
                size,
                missingSize,
                Base::template createNextLayerCachedFieldsReader<TIdx>(allFields));
    }

    /// @brief Serialise message into the output data sequence.
    /// @details The function will write proper "sync" value to the output
    ///     buffer, then call the write() function of the next layer.
    /// @param[in] msg Reference to message object
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       written. In case of an error, distance between original position
    ///       and advanced will pinpoint the location of the error.
    template <typename TMsg, typename TIter>
    ErrorStatus write(const TMsg& msg, TIter& iter, std::size_t size) const
    {
        Field field;
        return writeInternal(field, msg, iter, size, Base::createNextLayerWriter());
    }

    /// @brief Serialise message into output data sequence while caching the written transport
    ///     information fields.
    /// @details Very similar to write() member function, but adds "allFields"
    ///     parameter to store raw data of the message.
    /// @tparam TIdx Index of the data field in TAllFields, expected to be last
    ///     element in the tuple.
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in] msg Reference to the message object that is being written,
    /// @param[in, out] iter Iterator used for writing.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    template <std::size_t TIdx, typename TAllFields, typename TMsg, typename TIter>
    ErrorStatus writeFieldsCached(
        TAllFields& allFields,
        const TMsg& msg,
        TIter& iter,
        std::size_t size) const
    {
        auto& field = Base::template getField<TIdx>(allFields);
        return
            writeInternal(
                field,
                msg,
                iter,
                size,
                Base::template createNextLayerCachedFieldsWriter<TIdx>(allFields));
    }

private:

    template <typename TMsgPtr, typename TIter, typename TReader>
    ErrorStatus readInternal(
        Field& field,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize,
        TReader&& reader)
    {
        auto es = field.read(iter, size);
        if (es == ErrorStatus::NotEnoughData) {
            Base::updateMissingSize(field, size, missingSize);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        if (field != Field()) {
            // doesn't match expected
            return ErrorStatus::ProtocolError;
        }

        return reader.read(msgPtr, iter, size - field.length(), missingSize);
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternal(
        const Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field.length());
    }
};

}  // namespace protocol

}  // namespace comms


