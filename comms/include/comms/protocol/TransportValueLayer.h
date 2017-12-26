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
/// @headerfile comms/protocol/TransportValueLayer.h
template <typename TField, std::size_t TIdx, typename TNextLayer>
class TransportValueLayer : public
        ProtocolLayerBase<
            TField,
            TNextLayer,
            TransportValueLayer<TField, TIdx, TNextLayer>,
            comms::option::ProtocolLayerForceReadUntilDataSplit
        >
{
    using BaseImpl =
        ProtocolLayerBase<
            TField,
            TNextLayer,
            TransportValueLayer<TField, TIdx, TNextLayer>,
            comms::option::ProtocolLayerForceReadUntilDataSplit
        >;

public:
    /// @brief Type of the field object used to read/write "sync" value.
    using Field = typename BaseImpl::Field;

    /// @brief Default constructor
    TransportValueLayer() = default;

    /// @brief Copy constructor.
    TransportValueLayer(const TransportValueLayer&) = default;

    /// @brief Move constructor.
    TransportValueLayer(TransportValueLayer&&) = default;

    /// @brief Destructor
    ~TransportValueLayer() noexcept = default;

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details Reads the "sync" value from the input data. If the read value
    ///     is NOT as expected (doesn't equal to the default constructed
    ///     @ref Field), then comms::ErrorStatus::ProtocolError is returned.
    ////    If the read "sync" value as expected, the read() member function of
    ///     the next layer is called.
    /// @tparam TMsgPtr Type of the smart pointer to the allocated message object.
    /// @tparam TIter Type of iterator used for reading.
    /// @tparam TNextLayerReader next layer reader object type.
    /// @param[out] field Field object to read.
    /// @param[in, out] msgPtr Reference to smart pointer that already holds or
    ///     will hold allocated message object
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] missingSize If not nullptr and return value is
    ///     comms::ErrorStatus::NotEnoughData it will contain
    ///     minimal missing data length required for the successful
    ///     read attempt.
    /// @param[in] nextLayerReader Next layer reader object.
    /// @return Status of the read operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post missingSize output value is updated if and only if function
    ///       returns comms::ErrorStatus::NotEnoughData.
    template <typename TMsgPtr, typename TIter, typename TNextLayerReader>
    comms::ErrorStatus doRead(
        Field& field,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize,
        TNextLayerReader&& nextLayerReader)
    {
        auto es = field.read(iter, size);
        if (es == comms::ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, missingSize);
        }

        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        es = nextLayerReader.read(msgPtr, iter, size - field.length(), missingSize);
        if (msgPtr) {
            using MsgPtrType = typename std::decay<decltype(msgPtr)>::type;
            using MessageInterfaceType = typename MsgPtrType::element_type;
            static_assert(MessageInterfaceType::hasTransportFields(),
                "Message interface class hasn't defined transport fields, "
                "use comms::option::ExtraTransportFields option.");
            static_assert(TIdx < std::tuple_size<typename MessageInterfaceType::TransportFields>::value,
                "TIdx is too big, exceeds the amount of transport fields defined in interface class");

            auto& transportField = std::get<TIdx>(msgPtr->transportFields());

            using FieldType = typename std::decay<decltype(transportField)>::type;
            using ValueType = typename FieldType::ValueType;

            transportField.value() = static_cast<ValueType>(field.value());
        }
        return es;
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
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasTransportFields(),
            "Message interface class hasn't defined transport fields, "
            "use comms::option::ExtraTransportFields option.");
        static_assert(TIdx < std::tuple_size<typename MsgType::TransportFields>::value,
            "TIdx is too big, exceeds the amount of transport fields defined in interface class");

        using ValueType = typename Field::ValueType;

        auto& transportField = std::get<TIdx>(msg.transportFields());
        field.value() = static_cast<ValueType>(transportField.value());

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


