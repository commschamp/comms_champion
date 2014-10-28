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
class MsgSizeLayer : public ProtocolLayerBase<TField, TNextLayer>
{
    typedef ProtocolLayerBase<TField, TNextLayer> Base;

public:

    /// @brief Pointer to message object
    typedef typename Base::MsgPtr MsgPtr;

    typedef typename Base::Message Message;

    /// @brief Type of read iterator
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Type of write iterator
    typedef typename Base::WriteIterator WriteIterator;

    typedef typename Base::Field Field;

    static_assert(comms::field::isBasicIntValue<Field>(),
        "Field must be of BasicIntValue type");

    using Base::ProtocolLayerBase;

    /// @brief Destructor is default
    ~MsgSizeLayer() = default;

    /// @brief Copy assignment is default
    MsgSizeLayer& operator=(const MsgSizeLayer&) = default;


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
        auto es = field.read(iter, size);
        if (es == ErrorStatus::NotEnoughData) {
            Base::updateMissingSize(size, missingSize);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        static const auto Offset = static_cast<std::size_t>(Field::Offset);
        auto serialisedValue = static_cast<std::size_t>(field.getSerialisedValue());
        if ((0 < Offset) && (serialisedValue < Field::Offset)) {
            return ErrorStatus::ProtocolError;
        }

        auto actualRemainingSize = (size - Field::length());
        auto requiredRemainingSize = static_cast<std::size_t>(field.getValue());

        if (actualRemainingSize < requiredRemainingSize) {
            if (missingSize != nullptr) {
                *missingSize = requiredRemainingSize - actualRemainingSize;
            }
            return ErrorStatus::NotEnoughData;
        }

        // not passing missingSize farther on purpose
        es = Base::nextLayer().read(msgPtr, iter, requiredRemainingSize);
        if (es == ErrorStatus::NotEnoughData) {
            return ErrorStatus::ProtocolError;
        }
        return es;
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
        typedef typename std::iterator_traits<WriteIterator>::iterator_category IterType;
        return write(msg, iter, size, IterType());
    }

    /// @brief Update the recently written output data sequence.
    /// @copydetails MsgIdLayer::update
    template <typename TUpdateIter>
    ErrorStatus update(
        TUpdateIter& iter,
        std::size_t size) const
    {
        if (size < Field::length()) {
            return ErrorStatus::BufferOverflow;
        }

        typedef typename Field::ValueType ValueType;
        Field field(static_cast<ValueType>(size - Field::length()));
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        return Base::nextLayer().update(iter, size - Field::length());
    }

private:

    ErrorStatus write(
                const Message& msg,
                WriteIterator& iter,
                std::size_t size,
                const std::random_access_iterator_tag& tag) const
    {
        return writeRandomAccessIter(msg, iter, size);
    }

    ErrorStatus write(
                    const Message& msg,
                    WriteIterator& iter,
                    std::size_t size,
                    const std::output_iterator_tag& tag) const
    {
        return writeOutputIter(msg, iter, size);
    }

    ErrorStatus writeRandomAccessIter(
                const Message& msg,
                WriteIterator& iter,
                std::size_t size) const
    {
        WriteIterator firstIter(iter);

        typedef typename Field::ValueType ValueType;
        Field field(static_cast<ValueType>(0));
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field.length() <= size);
        es = Base::nextLayer().write(msg, iter, size - field.length());
        if (es == ErrorStatus::Success)
        {
            field.setValue(
                static_cast<ValueType>(
                    std::distance(firstIter, iter) - field.length()));

            es = field.write(firstIter, field.length());
        }

        return es;
    }

    ErrorStatus writeOutputIter(
                    const Message& msg,
                    WriteIterator& iter,
                    std::size_t size,
                    const std::output_iterator_tag& tag) const
    {
        typedef typename Field::ValueType ValueType;
        Field field(static_cast<ValueType>(0));
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field.length() <= size);
        es = Base::nextLayer().write(msg, iter, size - field.length());
        if (es != ErrorStatus::Success)
        {
            return es;
        }

        return ErrorStatus::UpdateRequired;
    }


};

// Implementation



}  // namespace protocol

}  // namespace comms

