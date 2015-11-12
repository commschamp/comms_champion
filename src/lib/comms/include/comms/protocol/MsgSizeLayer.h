//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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
template <typename TField,
          typename TNextLayer>
class MsgSizeLayer : public ProtocolLayerBase<TField, TNextLayer>
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

    /// @brief Type of the field object used to read/write remaining size value.
    typedef typename Base::Field Field;

    static_assert(comms::field::isIntValue<Field>(),
        "Field must be of IntValue type");

    /// @brief Constructor of any number of arguments.
    /// @details All the arguments are passed to the constructor of the
    ///     ProtocolLayerBase (which is a base of this class), which it turn
    ///     forwards them to the constructor of the next layer.
    template <typename... TArgs>
    explicit MsgSizeLayer(TArgs&&... args)
       : Base(std::forward<TArgs>(args)...)
    {
    }

    /// @brief Copy constructor
    MsgSizeLayer(const MsgSizeLayer&) = default;

    /// @brief Move constructor
    MsgSizeLayer(MsgSizeLayer&&) = default;

    /// @brief Destructor.
    ~MsgSizeLayer() = default;

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
    template <typename TMsgPtr>
    ErrorStatus read(
        TMsgPtr& msgPtr,
        ReadIterator& iter,
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
    template <std::size_t TIdx, typename TAllFields, typename TMsgPtr>
    ErrorStatus readFieldsCached(
        TAllFields& allFields,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
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
    /// @param[in] msg Reference to message object
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Max number of bytes that can be written.
    /// @return Status of the write operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       written. In case of an error, distance between original position
    ///       and advanced will pinpoint the location of the error.
    ErrorStatus write(
            const Message& msg,
            WriteIterator& iter,
            std::size_t size) const
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
    template <std::size_t TIdx, typename TAllFields>
    ErrorStatus writeFieldsCached(
        TAllFields& allFields,
        const Message& msg,
        WriteIterator& iter,
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

    using FixedLengthTag = typename Base::FixedLengthTag;
    using VarLengthTag = typename Base::VarLengthTag;
    using LengthTag = typename Base::LengthTag;

    template <typename TMsgPtr, typename TReader>
    ErrorStatus readInternal(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
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
        return es;
    }

    template <typename TWriter>
    ErrorStatus writeInternal(
        Field& field,
        const Message& msg,
        WriteIterator& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        typedef typename Field::ValueType FieldValueType;
        field.value() =
            static_cast<FieldValueType>(Base::nextLayer().length(msg));
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field.length());
    }

    template <typename TMsg>
    constexpr std::size_t lengthInternal(const TMsg& msg, FixedLengthTag) const
    {
        return Base::length(msg);
    }

    template <typename TMsg>
    std::size_t lengthInternal(const TMsg& msg, VarLengthTag) const
    {
        typedef typename Field::ValueType FieldValueType;
        auto remSize = Base::nextLayer().length(msg);
        return Field(static_cast<FieldValueType>(remSize)).length() + remSize;
    }
};


}  // namespace protocol

}  // namespace comms

