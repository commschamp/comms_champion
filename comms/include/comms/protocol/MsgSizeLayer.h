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

#pragma once

#include <iterator>
#include <type_traits>
#include "comms/field/IntValue.h"
#include "ProtocolLayerBase.h"

namespace comms
{

namespace protocol
{

/// @brief Protocol layer that uses size field as a prefix to all the
///        subsequent data written by other (next) layers.
/// @details The main purpose of this layer is to provide information about
///     the remaining size of the serialised message. This layer is a mid level
///     layer, expects other mid level layer or MsgDataLayer to be its next one.
/// @tparam TField Type of the field that describes the "size" field.
/// @tparam TNextLayer Next transport layer in protocol stack.
/// @headerfile comms/protocol/MsgSizeLayer.h
template <typename TField,
          typename TNextLayer>
class MsgSizeLayer : public ProtocolLayerBase<TField, TNextLayer>
{
    using Base = ProtocolLayerBase<TField, TNextLayer>;

public:
    /// @brief Type of the field object used to read/write remaining size value.
    using Field = typename Base::Field;

    static_assert(comms::field::isIntValue<Field>(),
        "Field must be of IntValue type");

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
    constexpr std::size_t length() const
    {
        return Base::length();
    }

    template <typename TMsg>
    constexpr std::size_t length(const TMsg& msg) const
    {
        return lengthInternal(msg, LengthTag());
    }
    /// @endcond

    /// @brief Deserialise message from the input data sequence.
    /// @details Reads size of the subsequent data from the input data sequence
    ///          and calls read() member function of the next layer with
    ///          the size specified in the size field.The function will also
    ///          compare the provided size of the data with size of the
    ///          message read from the buffer. If the latter is greater than
    ///          former, comms::ErrorStatus::NotEnoughData will be returned.
    ///          However, if buffer contains enough data, but the next layer
    ///          reports it's not enough (returns comms::ErrorStatus::NotEnoughData),
    ///          comms::ErrorStatus::ProtocolError will be returned.
    /// @tparam TMsgPtr Type of smart pointer that holds message object.
    /// @tparam TIter Type of iterator used for reading.
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
    /// @tparam TMsgPtr Type of smart pointer that holds message object.
    /// @tparam TIter Type of iterator used for reading.
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
    /// @details The function will write number of bytes required to serialise
    ///     the message, then invoke the write() member function of the next
    ///     layer. The calculation of the required length is performed by invoking
    ///     "length(msg)".
    /// @tparam TMsg Type of message object.
    /// @tparam TIter Type of iterator used for writing.
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
        using MsgType = typename std::decay<decltype(msg)>::type;
        Field field;
        return writeInternal(field, msg, iter, size, Base::createNextLayerWriter(), MsgLengthTag<MsgType>());
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
    /// @tparam TMsg Type of message object.
    /// @tparam TIter Type of iterator used for writing.
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
        using MsgType = typename std::decay<decltype(msg)>::type;
        auto& field = Base::template getField<TIdx>(allFields);
        return
            writeInternal(
                field,
                msg,
                iter,
                size,
                Base::template createNextLayerCachedFieldsWriter<TIdx>(allFields),
                MsgLengthTag<MsgType>());
    }

    /// @brief Update written dummy size with proper value.
    /// @details Should be called when write() returns comms::ErrorStatus::UpdateRequired.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @return Status of the update operation.
    template <typename TIter>
    comms::ErrorStatus update(TIter& iter, std::size_t size) const
    {
        Field field;
        return updateInternal(field, iter, size, Base::createNextLayerUpdater());
    }

    /// @brief Update written dummy size with proper value while caching the written transport
    ///     information fields.
    /// @details Very similar to update() member function, but adds "allFields"
    ///     parameter to store raw data of the message.
    /// @tparam TIdx Index of the data field in TAllFields, expected to be last
    ///     element in the tuple.
    /// @tparam TAllFields std::tuple of all the transport fields, must be
    ///     @ref AllFields type defined in the last layer class that defines
    ///     protocol stack.
    /// @param[out] allFields Reference to the std::tuple object that wraps all
    ///     transport fields (@ref AllFields type of the last protocol layer class).
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the update operation.
    template <std::size_t TIdx, typename TAllFields, typename TIter>
    ErrorStatus updateFieldsCached(
        TAllFields& allFields,
        TIter& iter,
        std::size_t size) const
    {
        auto& field = Base::template getField<TIdx>(allFields);
        return
            updateInternal(
                field,
                iter,
                size,
                Base::template createNextLayerCachedFieldsUpdater<TIdx>(allFields));
    }

private:

    using FixedLengthTag = typename Base::FixedLengthTag;
    using VarLengthTag = typename Base::VarLengthTag;
    using LengthTag = typename Base::LengthTag;
    struct MsgHasLengthTag {};
    struct MsgNoLengthTag {};

    template<typename TMsg>
    using MsgLengthTag =
        typename std::conditional<
            details::ProtocolLayerHasFieldsImpl<TMsg>::Value || TMsg::InterfaceOptions::HasLength,
            MsgHasLengthTag,
            MsgNoLengthTag
        >::type;

    template <typename TMsgPtr, typename TIter, typename TReader>
    ErrorStatus readInternal(
        Field& field,
        TMsgPtr& msgPtr,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize,
        TReader&& reader)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(
            std::is_base_of<std::random_access_iterator_tag, IterTag>::value,
            "Current implementation of MsgSizeLayer requires iterator used for reading to be random-access one.");

        auto es = field.read(iter, size);
        if (es == ErrorStatus::NotEnoughData) {
            Base::updateMissingSize(field, size, missingSize);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        auto fromIter = iter;
        auto actualRemainingSize = (size - field.length());
        auto requiredRemainingSize = static_cast<std::size_t>(field.value());

        if (actualRemainingSize < requiredRemainingSize) {
            if (missingSize != nullptr) {
                *missingSize = requiredRemainingSize - actualRemainingSize;
            }
            return ErrorStatus::NotEnoughData;
        }

        // not passing missingSize farther on purpose
        es = reader.read(msgPtr, iter, requiredRemainingSize, nullptr);
        if (es == ErrorStatus::NotEnoughData) {
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

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalHasLength(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        using FieldValueType = typename Field::ValueType;
        field.value() =
            static_cast<FieldValueType>(Base::nextLayer().length(msg));
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field.length() <= size);
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

        field.value() = 0;
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        auto dataIter = iter;

        auto sizeLen = field.length();
        es = nextLayerWriter.write(msg, iter, size - sizeLen);
        if (es != ErrorStatus::Success) {
            return es;
        }

        field.value() = static_cast<typename Field::ValueType>(std::distance(dataIter, iter));
        GASSERT(field.length() == sizeLen);
        return field.write(valueIter, sizeLen);
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalOutput(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        field.value() = 0;
        auto es = field.write(iter, size);
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
        using IterType = typename std::decay<decltype(iter)>::type;
        using Tag = typename std::iterator_traits<IterType>::iterator_category;
        return writeInternalNoLengthTagged(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter), Tag());
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        MsgHasLengthTag) const
    {
        return writeInternalHasLength(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        MsgNoLengthTag) const
    {
        return writeInternalNoLength(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TMsg>
    constexpr std::size_t lengthInternal(const TMsg& msg, FixedLengthTag) const
    {
        return Base::length(msg);
    }

    template <typename TMsg>
    std::size_t lengthInternal(const TMsg& msg, VarLengthTag) const
    {
        using FieldValueType = typename Field::ValueType;
        auto remSize = Base::nextLayer().length(msg);
        return Field(static_cast<FieldValueType>(remSize)).length() + remSize;
    }

    template <typename TIter, typename TUpdater>
    ErrorStatus updateInternal(
        Field& field,
        TIter& iter,
        std::size_t size,
        TUpdater&& nextLayerUpdater) const
    {
        field.value() = size - Field::maxLength();
        if (field.length() != Field::maxLength()) {
            field.value() = size - field.length();
        }

        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        return nextLayerUpdater.update(iter, size - field.length());
    }
};


}  // namespace protocol

}  // namespace comms

