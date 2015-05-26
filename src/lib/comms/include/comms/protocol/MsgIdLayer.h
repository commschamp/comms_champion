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

/// @file comms/protocol/MsgIdLayer.h
/// This file contains "Message ID" protocol layer of the "comms" module.

#pragma once

#include <array>
#include <tuple>
#include <algorithm>
#include <utility>
#include <tuple>

#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "ProtocolLayerBase.h"
#include "comms/fields.h"
#include "comms/MsgFactory.h"

namespace comms
{

namespace protocol
{

template <typename TField,
          typename TAllMessages,
          typename TNextLayer,
          typename... TOptions>
class MsgIdLayer :
    public comms::protocol::ProtocolLayerBase<TField, TNextLayer, MsgIdLayer<TField, TAllMessages, TNextLayer, TOptions...> >
{
    static_assert(util::IsTuple<TAllMessages>::Value,
        "TAllMessages must be of std::tuple type");
    typedef comms::protocol::ProtocolLayerBase<TField, TNextLayer, MsgIdLayer<TField, TAllMessages, TNextLayer, TOptions...> > Base;

    typedef comms::MsgFactory<typename Base::Message, TAllMessages, TOptions...> Factory;

public:

    /// @brief Definition of all messages type. Must be std::tuple
    typedef typename Factory::AllMessages AllMessages;

    typedef typename Factory::MsgPtr MsgPtr;

    typedef typename Base::Message Message;

    /// @brief Type of message ID
    typedef typename Message::MsgIdParamType MsgIdParamType;

    typedef typename Message::MsgIdType MsgIdType;

    /// @brief Type of read iterator
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Type of write iterator
    typedef typename Base::WriteIterator WriteIterator;

    typedef TField Field;

    typedef typename Base::NextLayer NextLayer;

    static_assert(
        comms::field::isIntValue<Field>() || comms::field::isEnumValue<Field>(),
        "Field must be of IntValue or EnumValue types");

    template <typename... TArgs>
    explicit MsgIdLayer(TArgs&&... args)
       : Base(std::forward<TArgs>(args)...)
    {
    }

    /// @brief Copy constructor is default
    MsgIdLayer(const MsgIdLayer&) = default;

    /// @brief Move constructor is default
    MsgIdLayer(MsgIdLayer&&) = default;

    /// @brief Copy assignment is default
    MsgIdLayer& operator=(const MsgIdLayer&) = default;

    /// @brief Move assignment is default.
    MsgIdLayer& operator=(MsgIdLayer&&) = default;

    /// @brief Destructor
    ~MsgIdLayer() = default;

    /// @brief Deserialise message from the input data sequence.
    /// @details The function will read message ID from the data sequence first,
    ///          generate appropriate message object based on the read ID and
    ///          forward the request to the next layer.
    /// @param[in, out] msgPtr Reference to smart pointer that will hold
    ///                 allocated message object
    /// @param[in, out] iter Input iterator
    /// @param[in] size Size of the data in the sequence
    /// @param[out] missingSize If not nullptr and return value is
    ///             embxx::comms::ErrorStatus::NotEnoughData it will contain
    ///             minimal missing data length required for the successful
    ///             read attempt.
    /// @return Error status of the operation.
    /// @pre msgPtr doesn't point to any object:
    ///      @code assert(!msgPtr); @endcode
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    /// @post Returns embxx::comms::ErrorStatus::Success if and only if msgPtr points
    ///       to a valid object.
    /// @post missingSize output value is updated if and only if function
    ///       returns embxx::comms::ErrorStatus::NotEnoughData.
    /// @note Thread safety: Safe on distinct MsgIdLayer object and distinct
    ///       buffers, unsafe otherwise.
    /// @note Exception guarantee: Basic
    ErrorStatus read(
        MsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        Field field;
        return readInternal(field, msgPtr, iter, size, missingSize, Base::createNextLayerReader());
    }

    template <std::size_t TIdx, typename TAllFields>
    ErrorStatus readFieldsCached(
        TAllFields& allFields,
        MsgPtr& msgPtr,
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

    /// @brief Serialise message into output data sequence.
    /// @details The function will write ID of the message to the data
    ///          sequence, then call write() member function of the next
    ///          protocol layer.
    /// @param[in] msg Reference to message object
    /// @param[in, out] iter Output iterator.
    /// @param[in] size Available space in data sequence.
    /// @return Status of the write operation.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       written. In case of an error, distance between original position
    ///       and advanced will pinpoint the location of the error.
    /// @note Thread safety: Safe on distinct stream buffers, unsafe otherwise.
    /// @note Exception guarantee: Basic
    ErrorStatus write(
        const Message& msg,
        WriteIterator& iter,
        std::size_t size) const
    {
        Field field(msg.getId());
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

        field.setValue(msg.getId());
        return
            writeInternal(
                field,
                msg,
                iter,
                size,
                Base::template createNextLayerCachedFieldsWriter<TIdx>(allFields));
    }

    MsgPtr createMsg(MsgIdParamType id)
    {
        return factory_.createMsg(id);
    }

private:

    template <typename TReader>
    ErrorStatus readInternal(
        Field& field,
        MsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize,
        TReader&& reader)
    {
        GASSERT(!msgPtr);
        auto es = field.read(iter, size);
        if (es == ErrorStatus::NotEnoughData) {
            Base::updateMissingSize(field, size, missingSize);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        auto id = field.getValue();

        msgPtr = createMsg(id);

        if (!msgPtr) {
            if (!factory_.msgRegistered(id)) {
                return ErrorStatus::InvalidMsgId;
            }

            return ErrorStatus::MsgAllocFaulure;
        }

        es = reader.read(msgPtr, iter, size - field.length(), missingSize);
        if (es != ErrorStatus::Success) {
            msgPtr.reset();
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
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field.length());
    }

    Factory factory_;
};


}  // namespace protocol

}  // namespace comms

