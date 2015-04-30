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
#include "details/MsgIdLayerBase.h"
#include "comms/fields.h"

namespace comms
{

namespace protocol
{

namespace details
{

template <std::size_t TEndIdx, typename TAllMessages>
struct AreMessagesSorted
{
    typedef typename std::tuple_element<TEndIdx - 2, TAllMessages>::type FirstElemType;
    typedef typename std::tuple_element<TEndIdx - 1, TAllMessages>::type SecondElemType;

    static const bool Value =
        ((FirstElemType::MsgId < SecondElemType::MsgId) &&
         (AreMessagesSorted<TEndIdx - 1, TAllMessages>::Value));
};

template <typename TAllMessages>
struct AreMessagesSorted<1, TAllMessages>
{
    static const bool Value = true;
};

template <typename TAllMessages>
struct AreMessagesSorted<0, TAllMessages>
{
    static const bool Value = true;
};

template <std::size_t TSize, bool TIsNumeric>
struct MsgFactoryCreator
{
    template <typename TAllMessages,
              template <class> class TFactory,
              typename TFactories>
    static void create(TFactories& factories)
    {
        static const std::size_t Idx = TSize - 1;
        MsgFactoryCreator<Idx, TIsNumeric>::template
                            create<TAllMessages, TFactory>(factories);

        typedef typename std::tuple_element<Idx, TAllMessages>::type Message;
        static TFactory<Message> factory;
        factories[Idx] = &factory;
    }
};

template <>
struct MsgFactoryCreator<0, true>
{
    template <typename TAllMessages,
              template <class> class TFactory,
              typename TFactories>
    static void create(TFactories& factories)
    {
        static_cast<void>(factories);
        static const std::size_t NumOfMsgs = std::tuple_size<TAllMessages>::value;
        static_assert(AreMessagesSorted<NumOfMsgs, TAllMessages>::Value,
            "All the message types in the bundle must be sorted in ascending order "
            "based on their MsgId");

    }
};

template <>
struct MsgFactoryCreator<0, false>
{
    template <typename TAllMessages,
              template <class> class TFactory,
              typename TFactories>
    static void create(TFactories& factories)
    {
        GASSERT(std::is_sorted(factories.begin(), factories.end()));
    }
};

template <typename TOption>
struct IsNumIdImplOpt
{
    static const bool Value = false;
};

template <long long int TId>
struct IsNumIdImplOpt<comms::option::StaticNumIdImpl<TId> >
{
    static const bool Value = true;
};

template <typename TOptions>
struct HasNumIdImplOpt;

template <typename TFirst, typename... TRest>
struct HasNumIdImplOpt<std::tuple<TFirst, TRest...> >
{
    static const bool Value =
        IsNumIdImplOpt<TFirst>::Value ||
        HasNumIdImplOpt<std::tuple<TRest...> >::Value;
};

template <>
struct HasNumIdImplOpt<std::tuple<> >
{
    static const bool Value = false;
};

template <typename TMessage>
struct HasStaticId
{
    static const bool Value = HasNumIdImplOpt<typename TMessage::AllOptions>::Value;
};

}  // namespace details

/// @ingroup comms
/// @brief Protocol layer that uses message ID to differentiate between messages.
/// @details This layers is a "must have" one, it contains allocator to allocate
///          message object.
/// @tparam TAllMessages A tuple (std::tuple) of all the custom message types
///         this protocol layer must support. The messages in the tuple must
///         be sorted in ascending order based on their MsgId
/// @tparam TAllocator The allocator class, will be used to allocate message
///         objects in read() member function.
///         The requirements for the allocator are:
///         @li Must have a default (no arguments) constructor.
///         @li Must provide allocation function to allocate message
///             objects. The signature must be as following:
///             @code template <typename TObj, typename... TArgs> std::unique_ptr<TObj, Deleter> alloc(TArgs&&... args); @endcode
///             The Deleter maybe either default "std::default_delete<T>" or
///             custom one. All the allocators defined in "util" module
///             (header: "embxx/util/Allocators.h") satisfy these requirements.
///             See also embxx::comms::DynMemMsgAllocator and
///             embxx::comms::InPlaceMsgAllocator
/// @tparam TTraits A traits class that must define:
///         @li Endianness type. Either embxx::comms::traits::endian::Big or
///             embxx::comms::traits::endian::Little
///         @li MsgIdLen static integral constant specifying length of
///             message ID field in bytes.
/// @pre TAllMessages must be any variation of std::tuple
/// @pre All message types in TAllMessages must be in ascending order based on
///      their MsgId value
/// @headerfile embxx/comms/protocol/MsgIdLayer.h
template <typename TField,
          typename TAllMessages,
          typename TNextLayer,
          typename... TOptions>
class MsgIdLayer :
    public details::MsgIdLayerBase<TField, TAllMessages, TNextLayer, TOptions...>
{
    static_assert(util::IsTuple<TAllMessages>::Value,
        "TAllMessages must be of std::tuple type");
    typedef details::MsgIdLayerBase<TField, TAllMessages, TNextLayer, TOptions...> Base;

public:

    /// @brief Definition of all messages type. Must be std::tuple
    typedef typename Base::AllMessages AllMessages;

    typedef typename Base::MsgPtr MsgPtr;

    typedef typename Base::Message Message;

    /// @brief Type of message ID
    typedef typename Message::MsgIdParamType MsgIdParamType;

    typedef typename Message::MsgIdType MsgIdType;

    /// @brief Type of read iterator
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Type of write iterator
    typedef typename Base::WriteIterator WriteIterator;

    typedef typename Base::Field Field;

    typedef typename Base::NextLayer NextLayer;

    static_assert(
        comms::field::isComplexIntValue<Field>() || comms::field::isBasicEnumValue<Field>(),
        "Field must be of ComplexIntValue or BasicEnumValue types");

    template <typename... TArgs>
    explicit MsgIdLayer(TArgs&&... args)
       : Base(std::forward<TArgs>(args)...)
    {
        static const std::size_t NumOfMsgs = std::tuple_size<AllMessages>::value;
        static const bool Numeric =
            std::is_arithmetic<MsgIdType>::value ||
            std::is_enum<MsgIdType>::value;

        details::MsgFactoryCreator<NumOfMsgs, Numeric>::template
                        create<AllMessages, MsgFactory>(factories_);
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
        auto factoryIter = findFactory(id);

        if ((factoryIter == factories_.end()) || ((*factoryIter)->getId() != id)) {
            return MsgPtr();
        }

        return (*factoryIter)->create(*this);
    }

private:

    /// @cond DOCUMENT_MSG_ID_PROTOCOL_LAYER_FACTORY
    class Factory
    {
    public:

        virtual ~Factory() {};

        MsgIdParamType getId() const
        {
            return this->getIdImpl();
        }

        MsgPtr create(MsgIdLayer& layer) const
        {
            return this->createImpl(layer);
        }

    protected:
        virtual MsgIdParamType getIdImpl() const = 0;
        virtual MsgPtr createImpl(MsgIdLayer& layer) const = 0;
    };

    template <typename TMessage>
    class NumIdMsgFactory : public Factory
    {
    public:
        typedef TMessage Message;
        static const auto MsgId = Message::MsgId;
    protected:
        virtual MsgIdParamType getIdImpl() const
        {
            return static_cast<MsgIdParamType>(MsgId);
        }

        virtual MsgPtr createImpl(MsgIdLayer& layer) const
        {
            return layer.template allocMsg<Message>();
        }
    };

    template <typename TMessage>
    friend class comms::protocol::MsgIdLayer<
        TField, TAllMessages, TNextLayer, TOptions...>::NumIdMsgFactory;

    template <typename TMessage>
    class GenericMsgFactory : public Factory
    {
    public:
        typedef TMessage Message;

        GenericMsgFactory() : id_(Message().getId()) {}

    protected:

        virtual MsgIdParamType getIdImpl() const
        {
            return id_;
        }

        virtual MsgPtr createImpl(MsgIdLayer& layer) const
        {
            return layer.template allocMsg<Message>();
        }
    private:
        typename Message::MsgIdType id_;
    };

    template <typename TMessage>
    friend class comms::protocol::MsgIdLayer<
        TField, TAllMessages, TNextLayer, TOptions...>::GenericMsgFactory;

    template <typename TMessage>
    using MsgFactory =
        typename std::conditional<
            details::HasStaticId<TMessage>::Value,
            NumIdMsgFactory<TMessage>,
            GenericMsgFactory<TMessage>
        >::type;

    /// @endcond

    typedef std::array<Factory*, std::tuple_size<AllMessages>::value> Factories;

    typename Factories::iterator findFactory(MsgIdParamType id)
    {
        return std::lower_bound(factories_.begin(), factories_.end(), id,
            [](Factory* factory, MsgIdParamType id) -> bool
            {
                return factory->getId() < id;
            });
    }

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

        auto factoryIter = findFactory(id);
        if ((factoryIter == factories_.end()) || ((*factoryIter)->getId() != id)) {
            return ErrorStatus::InvalidMsgId;
        }

        msgPtr = (*factoryIter)->create(*this);

        if (!msgPtr) {
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

    Factories factories_;
};

// Implementation


}  // namespace protocol

}  // namespace comms

