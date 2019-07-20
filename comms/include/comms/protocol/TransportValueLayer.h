//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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
#include "details/TransportValueLayerAdapter.h"


namespace comms
{

namespace protocol
{

/// @brief Protocol layer that reads a value from transport wrapping and
///     reassigns it to appropriate "extra transport" data member of the
///     created message object.
/// @details Some protocols may put some values, which influence the way
///     of how message contents are read and/or how the message is handled.
///     For example protocol version information. This layer will read
///     the field's value and will re-assign it to specified message object's
///     "extra transport" data member field. This layer requires extra support
///     from the defined message interface object - there is a need to pass
///     @ref comms::option::def::ExtraTransportFields option to the interface definition
///     @ref comms::Message class.
///     This layer is a mid level layer, expects other mid level layer or
///     MsgDataLayer to be its next one.
/// @tparam TField Type of the field that is used read / write extra transport value.
/// @tparam TIdx Index of "extra transport" field that message object contains
///     (accessed via @ref comms::Message::transportFields()).
/// @tparam TNextLayer Next transport layer in protocol stack.
/// @tparam TOptions Extending functionality options. Supported options are:
///     @li @ref comms::option::def::PseudoValue - Mark the handled value to be "pseudo"
///         one, i.e. the field is not getting serialised.
/// @headerfile comms/protocol/TransportValueLayer.h
/// @extends ProtocolLayerBase
template <typename TField, std::size_t TIdx, typename TNextLayer, typename... TOptions>
class TransportValueLayer : public
        details::TransportValueLayerAdapterT<
            ProtocolLayerBase<
                TField,
                TNextLayer,
                TransportValueLayer<TField, TIdx, TNextLayer, TOptions...>,
                comms::option::def::ProtocolLayerForceReadUntilDataSplit
            >,
            TOptions...
        >
{
    using BaseImpl =
        details::TransportValueLayerAdapterT<
            ProtocolLayerBase<
                TField,
                TNextLayer,
                TransportValueLayer<TField, TIdx, TNextLayer, TOptions...>,
                comms::option::def::ProtocolLayerForceReadUntilDataSplit
            >,
            TOptions...
        >;

public:
    /// @brief Type of the field object used to read/write "sync" value.
    using Field = typename BaseImpl::Field;

    /// @brief Parsed options
    using TransportParsedOptions = details::TransportValueLayerOptionsParser<TOptions...>;

    /// @brief Default constructor
    TransportValueLayer() = default;

    /// @brief Copy constructor.
    TransportValueLayer(const TransportValueLayer&) = default;

    /// @brief Move constructor.
    TransportValueLayer(TransportValueLayer&&) = default;

    /// @brief Destructor
    ~TransportValueLayer() noexcept = default;

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details Reads the value from the input data and assigns it to appropriate
    ///     extra transport field inside the message object (accessed via
    ///     comms::Message::transportFields()). @n
    ///     Note, that this operation works fine even if message object is created
    ///     after reading the transport value. There is "inner magic" that causes
    ///     read operation to proceed until @b DATA layer
    ///     (implemented by @ref comms::protocol::MsgDataLayer), assigns the
    ///     read value to message object, then proceeds to reading the message
    ///     contents, i.e. when @ref comms::Message::read() function is invoked
    ///     the message object already has the value of the transport field updated.
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
        auto es = readFieldInternal(field, iter, size, ValueTag(), extraValues...);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        es = nextLayerReader.read(msg, iter, size, extraValues...);

        using Tag =
            typename std::conditional<
                BaseImpl::template isMessageObjRef<typename std::decay<decltype(msg)>::type>(),
                MsgObjTag,
                SmartPtrTag
            >::type;

        if (validMsg(msg, Tag())) {
            auto& allTransportFields = transportFields(msg, Tag());
            auto& transportField = std::get<TIdx>(allTransportFields);

            using FieldType = typename std::decay<decltype(transportField)>::type;
            using ValueType = typename FieldType::ValueType;

            transportField.value() = static_cast<ValueType>(field.value());
        }
        return es;
    }

    /// @brief Customized write functionality, invoked by @ref write().
    /// @details The function will write the appriprate extra transport value
    ///     held by the message object being written.
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
            "use comms::option::def::ExtraTransportFields option.");
        static_assert(TIdx < std::tuple_size<typename MsgType::TransportFields>::value,
            "TIdx is too big, exceeds the amount of transport fields defined in interface class");

        using ValueType = typename Field::ValueType;

        auto& transportField = std::get<TIdx>(msg.transportFields());
        field.value() = static_cast<ValueType>(transportField.value());

        auto es = writeFieldInternal(field, iter, size, ValueTag());
        if (es != ErrorStatus::Success) {
            return es;
        }

        return nextLayerWriter.write(msg, iter, size);
    }

    /// @brief Customising field length calculation
    /// @details If the layer is marked as "pseudo" (using @ref comms::option::def::PseudoValue)
    ///     option, then the report length is 0.
    static constexpr std::size_t doFieldLength()
    {
         return doFieldLengthInternal(ValueTag());
    }

    /// @brief Customising field length calculation
    /// @details If the layer is marked as "pseudo" (using @ref comms::option::def::PseudoValue)
    ///     option, then the report length is 0.
    template <typename TMsg>
    static std::size_t doFieldLength(const TMsg&)
    {
        return doFieldLength();
    }

#ifdef FOR_DOXYGEN_DOC_ONLY
    /// @brief Access to pseudo field stored internally.
    /// @details The function exists only if @ref comms::option::def::PseudoValue
    ///     option has been used.
    Field& pseudoField();

    /// @brief Const access to pseudo field stored internally.
    /// @details The function exists only if @ref comms::option::def::PseudoValue
    ///     option has been used.
    const Field& pseudoField() const;
#endif
private:
    struct SmartPtrTag {};
    struct MsgObjTag {};

    struct PseudoValueTag {};
    struct NormalValueTag {};

    using ValueTag =
        typename std::conditional<
            TransportParsedOptions::HasPseudoValue,
            PseudoValueTag,
            NormalValueTag
        >::type;

    template <typename TMsg>
    static bool validMsg(TMsg& msgPtr, SmartPtrTag)
    {
        using MsgPtrType = typename std::decay<decltype(msgPtr)>::type;
        using MessageInterfaceType = typename MsgPtrType::element_type;
        static_assert(MessageInterfaceType::hasTransportFields(),
            "Message interface class hasn't defined transport fields, "
            "use comms::option::def::ExtraTransportFields option.");
        static_assert(TIdx < std::tuple_size<typename MessageInterfaceType::TransportFields>::value,
            "TIdx is too big, exceeds the amount of transport fields defined in interface class");

        return static_cast<bool>(msgPtr);
    }

    template <typename TMsg>
    static bool validMsg(TMsg& msg, MsgObjTag)
    {
        using MsgType = typename std::decay<decltype(msg)>::type;
        static_assert(MsgType::hasTransportFields(),
            "Message interface class hasn't defined transport fields, "
            "use comms::option::def::ExtraTransportFields option.");
        static_assert(TIdx < std::tuple_size<typename MsgType::TransportFields>::value,
            "TIdx is too big, exceeds the amount of transport fields defined in interface class");

        return true;
    }

    template <typename TMsg>
    static auto transportFields(TMsg& msgPtr, SmartPtrTag) -> decltype(msgPtr->transportFields())
    {
        return msgPtr->transportFields();
    }

    template <typename TMsg>
    static auto transportFields(TMsg& msg, MsgObjTag) -> decltype(msg.transportFields())
    {
        return msg.transportFields();
    }

    static constexpr std::size_t doFieldLengthInternal(PseudoValueTag)
    {
        return 0U;
    }

    static constexpr std::size_t doFieldLengthInternal(NormalValueTag)
    {
        return BaseImpl::doFieldLength();
    }

    template <typename TIter, typename... TExtraValues>
    comms::ErrorStatus readFieldInternal(
        Field& field,
        TIter& iter,
        std::size_t& len,
        PseudoValueTag,
        TExtraValues...)
    {
        static_cast<void>(iter);
        static_cast<void>(len);
        field = BaseImpl::pseudoField();
        return comms::ErrorStatus::Success;
    }

    template <typename TIter, typename... TExtraValues>
    comms::ErrorStatus readFieldInternal(
        Field& field,
        TIter& iter,
        std::size_t& len,
        NormalValueTag,
        TExtraValues... extraValues)
    {
        auto beforeReadIter = iter;
        auto es = field.read(iter, len);
        if (es == comms::ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, len, extraValues...);
        }
        else {
            auto fieldLen = static_cast<std::size_t>(std::distance(beforeReadIter, iter));
            len -= fieldLen;
        }
        return es;
    }

    template <typename TIter>
    comms::ErrorStatus writeFieldInternal(
        Field& field,
        TIter& iter,
        std::size_t& len,
        PseudoValueTag) const
    {
        static_cast<void>(iter);
        static_cast<void>(len);
        field = BaseImpl::pseudoField();
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    comms::ErrorStatus writeFieldInternal(
        Field& field,
        TIter& iter,
        std::size_t& len,
        NormalValueTag) const
    {
        auto es = field.write(iter, len);
        if (es == comms::ErrorStatus::Success) {
            COMMS_ASSERT(field.length() <= len);
            len -= field.length();
        }
        return es;
    }
};

namespace details
{
template <typename T>
struct TransportValueLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField, std::size_t TIdx, typename TNextLayer>
struct TransportValueLayerCheckHelper<TransportValueLayer<TField, TIdx, TNextLayer> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref TransportValueLayer
/// @related TransportValueLayer
template <typename T>
constexpr bool isTransportValueLayer()
{
    return details::TransportValueLayerCheckHelper<T>::Value;
}

}  // namespace protocol

}  // namespace comms


