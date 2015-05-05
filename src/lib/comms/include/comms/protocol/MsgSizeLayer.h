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

/// @file comms/protocol/MsgSizeLayer.h
/// This file contains "Message Size" protocol layer of the "comms" module.

#pragma once

#include <iterator>
#include <type_traits>
#include "comms/field/IntValue.h"
#include "ProtocolLayerBase.h"

namespace comms
{

namespace protocol
{

/// @ingroup comms
/// @brief Protocol layer that uses uses size field as a prefix to all the
///        subsequent data written by other (next) layers.
/// @details This layer is a mid level layer, expects other mid level layer or
///          MsgDataLayer to be its next one.
/// @tparam TTraits A traits class that must define:
///         @li Endianness type. Either embxx::comms::traits::endian::Big or
///             embxx::comms::traits::endian::Little
///         @li MsgSizeLen static integral constant of type std::size_t
///             specifying length of size field in bytes.
///         @li ExtraSizeValue static integral constant of type std::size_t
///             specifying extra value to be added to size field when
///             serialising
/// @tparam TNextLayer Next layer in the protocol stack
/// @headerfile embxx/comms/protocol/MsgSizeLayer.h
template <typename TField,
          typename TNextLayer>
class MsgSizeLayer : public
        ProtocolLayerBase<TField, TNextLayer, MsgSizeLayer<TField, TNextLayer> >
{
    typedef ProtocolLayerBase<TField, TNextLayer, MsgSizeLayer<TField, TNextLayer> > Base;

public:

    /// @brief Pointer to message object
    typedef typename Base::MsgPtr MsgPtr;

    typedef typename Base::Message Message;

    /// @brief Type of read iterator
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Type of write iterator
    typedef typename Base::WriteIterator WriteIterator;

    typedef typename Base::Field Field;

    static_assert(comms::field::isIntValue<Field>(),
        "Field must be of ComplexIntValue type");

    using Base::ProtocolLayerBase;

    /// @brief Destructor is default
    ~MsgSizeLayer() = default;

    /// @brief Copy assignment is default
    MsgSizeLayer& operator=(const MsgSizeLayer&) = default;

    constexpr std::size_t length() const
    {
        return Base::length();
    }

    template <typename TMsg>
    constexpr std::size_t length(const TMsg& msg) const
    {
        return lengthInternal(msg, LengthTag());
    }


    /// @brief Deserialise message from the input data sequence.
    /// @details Reads size of the subsequent data from the input data sequence
    ///          and calls read() member function of the next layer with
    ///          the size specified in the size field.The function will also
    ///          compare the provided size of the data with size of the
    ///          message read from the buffer. If the latter is greater than
    ///          former, embxx::comms::ErrorStatus::NotEnoughData will be returned.
    ///          However, if buffer contains enough data, but the next layer
    ///          reports its not enough (returns embxx::comms::ErrorStatus::NotEnoughData),
    ///          embxx::comms::ErrorStatus::ProtocolError will be returned.
    /// @param[in, out] msgPtr Reference to smart pointer that already holds or
    ///                 will hold allocated message object
    /// @param[in, out] iter Input iterator.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] missingSize If not nullptr and return value is
    ///             embxx::comms::ErrorStatus::NotEnoughData it will contain
    ///             minimal missing data length required for the successful
    ///             read attempt.
    /// @return Error status of the operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post missingSize output value is updated if and only if function
    ///       returns embxx::comms::ErrorStatus::NotEnoughData.
    /// @note Thread safety: Unsafe
    /// @note Exception guarantee: Basic
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
    /// @details The function will reserve space in the output data sequence
    ///          required to write size field, then forward the write() request
    ///          the the next layer in the protocol stack. After the latter
    ///          finishes its write, this protocol will evaluate the size of
    ///          the written data and update size field accordingly if such
    ///          update is possible (output iterator is random access one). If
    ///          update of the field is not possible (for example
    ///          std::back_insert_iterator was used), this function will return
    ///          embxx::comms::ErrorStatus::UpdateRequired. In this case
    ///          it is needed to call update() member function to finalise
    ///          the write operation.
    /// @param[in] msg Reference to message object
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Available space in data sequence.
    /// @return Status of the write operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       written. In case of an error, distance between original position
    ///       and advanced will pinpoint the location of the error.
    /// @note Thread safety: Unsafe
    /// @note Exception guarantee: Basic
    ErrorStatus write(
            const Message& msg,
            WriteIterator& iter,
            std::size_t size) const
    {
        Field field;
        return writeInternal(field, msg, iter, size, Base::createNextLayerWriter());
    }

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
        auto requiredRemainingSize = static_cast<std::size_t>(field.getValue());

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
        field.setValue(static_cast<FieldValueType>(Base::nextLayer().length(msg)));
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

