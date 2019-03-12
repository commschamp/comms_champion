//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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
#include "comms/protocol/ProtocolLayerBase.h"
#include "comms/protocol/details/MsgSizeLayerOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerExtendingClassHelper.h"

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
/// @tparam TOptions Default functionality extension options. Supported options are:
///     @li  @ref comms::option::ExtendingClass - Use this option to provide a class
///         name of the extending class, which can be used to extend existing functionality.
///         See also @ref page_custom_size_layer tutorial page.
/// @headerfile comms/protocol/MsgSizeLayer.h
template <typename TField, typename TNextLayer, typename... TOptions>
class MsgSizeLayer : public
        ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                MsgSizeLayer<TField, TNextLayer, TOptions...>, 
                details::MsgSizeLayerOptionsParser<TOptions...>
            >,
            comms::option::ProtocolLayerDisallowReadUntilDataSplit
        >
{
    using ExtendingClass = 
            details::ProtocolLayerExtendingClassT<
                MsgSizeLayer<TField, TNextLayer, TOptions...>, 
                details::MsgSizeLayerOptionsParser<TOptions...>
            >;

    using BaseImpl =
        ProtocolLayerBase<
            TField,
            TNextLayer,
            ExtendingClass,
            comms::option::ProtocolLayerDisallowReadUntilDataSplit
        >;

public:
    /// @brief Type of the field object used to read/write remaining size value.
    using Field = typename BaseImpl::Field;

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

    static constexpr std::size_t doFieldLength()
    {
        return BaseImpl::doFieldLength();
    }

    template <typename TMsg>
    constexpr std::size_t doFieldLength(const TMsg& msg) const
    {
        return fieldLengthInternal(msg, LengthTag());
    }
    /// @endcond

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details Reads size of the subsequent data from the input data sequence
    ///          and calls read() member function of the next layer with
    ///          the size specified in the size field. The function will also
    ///          compare the provided size of the data with value
    ///          read from the buffer. If the latter is greater than
    ///          former, comms::ErrorStatus::NotEnoughData will be returned.
    ///          However, if buffer contains enough data, but the next layer
    ///          reports it's not enough (returns comms::ErrorStatus::NotEnoughData),
    ///          comms::ErrorStatus::ProtocolError will be returned.
    /// @tparam TMsg Type of @b msg parameter.
    /// @tparam TIter Type of iterator used for reading.
    /// @tparam TNextLayerReader next layer reader object type.
    /// @param[out] field Field object to read.
    /// @param[in, out] msg Reference to smart pointer, that already holds or
    ///     will hold allocated message object, or reference to actual message
    ///     object (which extends @ref comms::MessageBase).
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
    template <typename TMsg, typename TIter, typename TNextLayerReader>
    comms::ErrorStatus doRead(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        std::size_t* missingSize,
        TNextLayerReader&& nextLayerReader)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(
            std::is_base_of<std::random_access_iterator_tag, IterTag>::value,
            "Current implementation of MsgSizeLayer requires iterator used for reading to be random-access one.");

        auto es = field.read(iter, size);
        if (es == ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, missingSize);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        auto fromIter = iter;
        std::size_t actualRemainingSize = (size - field.length());
        std::size_t requiredRemainingSize = 
            static_cast<ExtendingClass*>(this)->getRemainingSizeFromField(field);

        if (actualRemainingSize < requiredRemainingSize) {
            if (missingSize != nullptr) {
                *missingSize = requiredRemainingSize - actualRemainingSize;
            }
            return ErrorStatus::NotEnoughData;
        }

        using MsgType = typename std::decay<decltype(msg)>::type;
        using MsgPtrTag = MsgTypeTag<MsgType>;
        // not passing missingSize farther on purpose
        static_cast<ExtendingClass*>(this)->beforeRead(field, getPtrToMsgInternal(msg, MsgPtrTag()));
        es = nextLayerReader.read(msg, iter, requiredRemainingSize, nullptr);
        if (es == ErrorStatus::NotEnoughData) {
            BaseImpl::resetMsg(msg);
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


    /// @brief Customized write functionality, invoked by @ref write().
    /// @details The function will write number of bytes required to serialise
    ///     the message, then invoke the write() member function of the next
    ///     layer. The calculation of the required length is performed by invoking
    ///     "length(msg)".
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
    ErrorStatus doWrite(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter) const
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        return writeInternal(field, msg, iter, size, std::forward<TNextLayerWriter>(nextLayerWriter), MsgLengthTag<MsgType>());
    }

    /// @brief Customized update functionality, invoked by @ref update().
    /// @details Should be called when @ref doWrite() returns comms::ErrorStatus::UpdateRequired.
    /// @tparam TIter Type of iterator used for updating.
    /// @tparam TNextLayerWriter next layer updater object type.
    /// @param[out] field Field object to update.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @param[in] nextLayerUpdater Next layer updater object.
    /// @return Status of the update operation.
    template <typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus doUpdate(
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater) const
    {
        std::size_t lenValue = size - Field::maxLength();
        auto nullMsgPtr = static_cast<const void*>(nullptr);
        static_cast<const ExtendingClass*>(this)->prepareFieldForWrite(lenValue, nullMsgPtr, field);

        if (field.length() != Field::maxLength()) {
            lenValue = size - field.length();
            static_cast<const ExtendingClass*>(this)->prepareFieldForWrite(lenValue, nullMsgPtr, field);
        }

        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        return nextLayerUpdater.update(iter, size - field.length());
    }

protected:
    /// @brief Retrieve remaining size (length) from the field.
    /// @details May be overridden by the extending class
    /// @param[in] field Field for this layer.
    static std::size_t getRemainingSizeFromField(const Field& field)
    {
        static_assert(comms::field::isIntValue<Field>(),
            "Field must be of IntValue type");

        return static_cast<std::size_t>(field.value());
    }

    /// @brief Extra operation before read is forwarded to the next layer.
    /// @details Default implementation does nothing, may be overriden in the
    ///     derived class.
    /// @param[in] field Field of the layer that was successfully read.
    /// @param[in, out] msg Pointer to message object, either interface
    ///     class or message object itself (depending on how doRead() was invoked).
    ///     Can be @b nullptr in case message object hasn't been created yet
    template <typename TMsg>
    static void beforeRead(const Field& field, TMsg* msg)
    {
        static_cast<void>(field);
        static_cast<void>(msg);
    }

    /// @brief Prepare field for writing
    /// @details Must assign provided size (length) value. 
    ///     May be overridden by the extending class if some complex functionality is required.
    /// @param[in] size Size value to assign
    /// @param[in] msg Pointer to message object being written, maybe nullptr (in case invoked
    ///     from @ref comms::protocol::MsgSizeLayer::doUpdate "doUpdate()")
    /// @param[out] field Field, value of which needs to be populated
    template <typename TMsg>
    static void prepareFieldForWrite(std::size_t size, const TMsg* msg, Field& field)
    {
        static_assert(
            comms::field::isIntValue<Field>(),
            "Field must be of IntValue or EnumValue types");

        static_cast<void>(msg);
        field.value() = static_cast<typename Field::ValueType>(size);
    }

private:

    using FixedLengthTag = typename BaseImpl::FixedLengthTag;
    using VarLengthTag = typename BaseImpl::VarLengthTag;
    using LengthTag = typename BaseImpl::LengthTag;
    struct MsgHasLengthTag {};
    struct MsgNoLengthTag {};

    template<typename TMsg>
    using MsgLengthTag =
        typename std::conditional<
            details::ProtocolLayerHasFieldsImpl<TMsg>::Value || TMsg::InterfaceOptions::HasLength,
            MsgHasLengthTag,
            MsgNoLengthTag
        >::type;

    struct PtrToMsgTag {};
    struct DirectMsgTag {};

    template <typename TMsg>
    using MsgTypeTag =
        typename std::conditional<
            comms::isMessage<TMsg>(),
            DirectMsgTag,
            PtrToMsgTag
        >::type;


    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalHasLength(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        std::size_t lenValue = BaseImpl::nextLayer().length(msg);
        static_cast<const ExtendingClass*>(this)->prepareFieldForWrite(lenValue, &msg, field);
        auto es = field.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(field.length() <= size);
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

        field.value() = static_cast<typename Field::ValueType>(0);
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
        COMMS_ASSERT(field.length() == sizeLen);
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
        field.value() = static_cast<typename Field::ValueType>(0);
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
    constexpr std::size_t fieldLengthInternal(const TMsg& msg, FixedLengthTag) const
    {
        return BaseImpl::doFieldLength(msg);
    }

    template <typename TMsg>
    std::size_t fieldLengthInternal(const TMsg& msg, VarLengthTag) const
    {
        using FieldValueType = typename Field::ValueType;
        auto remSize = BaseImpl::nextLayer().length(msg);
        return Field(static_cast<FieldValueType>(remSize)).length();
    }

    template <typename TMsg>
    auto getPtrToMsgInternal(TMsg& msg, PtrToMsgTag) -> decltype (msg.get())
    {
        return msg.get();
    }

    template <typename TMsg>
    auto getPtrToMsgInternal(TMsg& msg, DirectMsgTag) -> decltype (&msg)
    {
        return &msg;
    }
};

namespace details
{
template <typename T>
struct MsgSizeLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField, typename TNextLayer>
struct MsgSizeLayerCheckHelper<MsgSizeLayer<TField, TNextLayer> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref MsgSizeLayer
/// @related MsgSizeLayer
template <typename T>
constexpr bool isMsgSizeLayer()
{
    return details::MsgSizeLayerCheckHelper<T>::Value;
}

}  // namespace protocol

}  // namespace comms

