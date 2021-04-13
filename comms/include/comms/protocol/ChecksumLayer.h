//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file 
/// Contains definition of @ref comms::protocol::ChecksumLayer

#pragma once

#include <iterator>
#include <type_traits>
#include "comms/field/IntValue.h"
#include "comms/protocol/details/ProtocolLayerBase.h"
#include "comms/protocol/details/ChecksumLayerOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerExtendingClassHelper.h"
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"

namespace comms
{

namespace protocol
{
/// @brief Protocol layer that is responsible to calculate checksum on the
///     data written by all the wrapped internal layers and append it to the end of
///     the written data. When reading, this layer is responsible to verify
///     the checksum reported at the end of the read data.
/// @tparam TField Type of the field that is used as to represent checksum value.
/// @tparam TCalc The checksum calculater class that is used to calculate
///     the checksum value on the provided buffer. It must have the operator()
///     defined with the following signature:
///     @code
///     template <typename TIter>
///     ResultType operator()(TIter& iter, std::size_t len) const;
///     @endcode
///     It is up to the checksum calculator to choose the "ResultType" it
///     returns. The falue is going to be casted to Field::ValueType before
///     assigning it as a value of the check field being read and/or written.
/// @tparam TNextLayer Next transport layer in protocol stack.
/// @tparam TOptions Extending functionality options. Supported options are:
///     @li @ref comms::option::def::ChecksumLayerVerifyBeforeRead - By default, the
///         @b ChecksumLayer will invoke @b read operation of inner (wrapped) layers
///         and only if it is successful, it will calculate and verify the
///         checksum value. Usage of @ref comms::option::def::ChecksumLayerVerifyBeforeRead
///         modifies the default behaviour by forcing the checksum verification
///         prior to invocation of @b read operation in the wrapped layer(s).
///     @li  @ref comms::option::ExtendingClass - Use this option to provide a class
///         name of the extending class, which can be used to extend existing functionality.
/// @headerfile comms/protocol/ChecksumLayer.h
template <typename TField, typename TCalc, typename TNextLayer, typename... TOptions>
class ChecksumLayer : public
        details::ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                ChecksumLayer<TField, TCalc, TNextLayer, TOptions...>,
                details::ChecksumLayerOptionsParser<TOptions...>
            >,            
            comms::option::def::ProtocolLayerDisallowReadUntilDataSplit
        >
{
    using BaseImpl =
        details::ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                ChecksumLayer<TField, TCalc, TNextLayer, TOptions...>,
                details::ChecksumLayerOptionsParser<TOptions...>
            >,            
            comms::option::def::ProtocolLayerDisallowReadUntilDataSplit
        >;

    using ExtendingClass =
        details::ProtocolLayerExtendingClassT<
            ChecksumLayer<TField, TCalc, TNextLayer, TOptions...>,
            details::ChecksumLayerOptionsParser<TOptions...>
        >;
public:
    /// @brief Parsed options
    using ParsedOptions = details::ChecksumLayerOptionsParser<TOptions...>;

    /// @brief Type of the field object used to read/write checksum value.
    using Field = typename BaseImpl::Field;

    /// @brief Provided checksum calculation algorithm
    using ChecksumCalc = TCalc;

    /// @brief Default constructor.
    ChecksumLayer() = default;

    /// @brief Copy constructor
    ChecksumLayer(const ChecksumLayer&) = default;

    /// @brief Move constructor
    ChecksumLayer(ChecksumLayer&&) = default;

    /// @brief Destructor.
    ~ChecksumLayer() noexcept = default;

    /// @brief Copy assignment
    ChecksumLayer& operator=(const ChecksumLayer&) = default;

    /// @brief Move assignment
    ChecksumLayer& operator=(ChecksumLayer&&) = default;

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details First, executes the read() member function of the next layer.
    ///     If the call returns comms::ErrorStatus::Success, it calculated the
    ///     checksum of the read data, reads the expected checksum value and
    ///     compares it to the calculated. If checksums match,
    ///     comms::ErrorStatus::Success is returned, otherwise
    ///     function returns comms::ErrorStatus::ProtocolError.
    /// @tparam TMsg Type of @b msg parameter.
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
    /// @pre Iterator must be "random access" one.
    /// @pre Iterator must be valid and can be dereferenced and incremented at
    ///      least "size" times;
    /// @post The iterator will be advanced by the number of bytes was actually
    ///       read. In case of an error, distance between original position and
    ///       advanced will pinpoint the location of the error.
    template <typename TMsg, typename TIter, typename TNextLayerReader, typename... TExtraValues>
    ErrorStatus doRead(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerReader&& nextLayerReader,
        TExtraValues... extraValues)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        static_assert(std::is_same<typename std::iterator_traits<IterType>::iterator_category, std::random_access_iterator_tag>::value,
            "The read operation is expected to use random access iterator");

        if (size < Field::minLength()) {
            return ErrorStatus::NotEnoughData;
        }

        using VerifyTag = 
            typename comms::util::LazyShallowConditional<
                ParsedOptions::HasVerifyBeforeRead
            >::template Type<
                VerifyBeforeReadTag,
                VerifyAfterReadTag
            >;        

        return
            readInternal(
                field,
                msg,
                iter,
                size,
                std::forward<TNextLayerReader>(nextLayerReader),
                VerifyTag(),
                extraValues...);
    }

    /// @brief Customized write functionality, invoked by @ref write().
    /// @details First, executes the write() member function of the next layer.
    ///     If the call returns comms::ErrorStatus::Success and it is possible
    ///     to re-read what has been written (random access iterator is used
    ///     for writing), the checksum is calculated and added to the output
    ///     buffer using the same iterator. In case non-random access iterator
    ///     type is used for writing (for example std::back_insert_iterator), then
    ///     this function writes a dummy value as checksum and returns
    ///     comms::ErrorStatus::UpdateRequired to indicate that call to
    ///     update() with random access iterator is required in order to be
    ///     able to update written checksum information.
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
        using IterType = typename std::decay<decltype(iter)>::type;
        using Tag = typename std::iterator_traits<IterType>::iterator_category;

        return writeInternal(field, msg, iter, size, std::forward<TNextLayerWriter>(nextLayerWriter), Tag());
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
        auto fromIter = iter;
        auto es = nextLayerUpdater.update(iter, size - Field::maxLength());
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return fieldUpdateInternal(static_cast<void*>(nullptr), fromIter, iter, size, field);
    }

    /// @brief Customized update functionality, invoked by @ref update().
    /// @details Similar to other @ref comms::protocol::ChecksumLayer::doUpdate() "doUpdate()",
    ///     but receiving reference to valid message object.
    /// @param[in] msg Reference to valid message object.
    /// @param[out] field Field object to update.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @param[in] nextLayerUpdater Next layer updater object.
    /// @return Status of the update operation.
    template <typename TMsg, typename TIter, typename TNextLayerUpdater>
    comms::ErrorStatus doUpdate(
        const TMsg& msg,
        Field& field,
        TIter& iter,
        std::size_t size,
        TNextLayerUpdater&& nextLayerUpdater) const
    {
        auto fromIter = iter;
        auto es = nextLayerUpdater.update(msg, iter, size - Field::maxLength());
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return fieldUpdateInternal(&msg, fromIter, iter, size, field);
    }

protected:
    template <typename TMsg, typename TIter>
    static auto calculateChecksum(const TMsg* msg, TIter& iter, std::size_t len, bool& checksumValid) -> decltype(TCalc()(iter, len))
    {
        static_cast<void>(msg);
        checksumValid = true;
        return TCalc()(iter, len);
    }    

private:
    static_assert(comms::field::isIntValue<Field>(),
        "The checksum field is expected to be of IntValue type");

    static_assert(Field::minLength() == Field::maxLength(),
        "The checksum field is expected to be of fixed length");

    template <typename... TParams>
    using VerifyBeforeReadTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using VerifyAfterReadTag = comms::details::tag::Tag2<>;

    template <typename TMsg, typename TIter, typename TReader, typename... TExtraValues>
    ErrorStatus verifyRead(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TReader&& nextLayerReader,
        TExtraValues... extraValues)
    {
        auto fromIter = iter;
        auto toIter = fromIter + (size - Field::minLength());
        auto len = static_cast<std::size_t>(std::distance(fromIter, toIter));

        auto checksumEs = field.read(toIter, Field::minLength());
        if (checksumEs != ErrorStatus::Success) {
            return checksumEs;
        }

        bool checksumValid = false;
        auto checksum = 
            static_cast<ExtendingClass*>(this)->calculateChecksum(
                BaseImpl::toMsgPtr(msg),
                fromIter,
                len,
                checksumValid);

        if (!checksumValid) {
            return comms::ErrorStatus::ProtocolError;
        }

        auto expectedValue = field.value();

        if (expectedValue != static_cast<decltype(expectedValue)>(checksum)) {
            BaseImpl::resetMsg(msg);
            return ErrorStatus::ProtocolError;
        }

        auto es = nextLayerReader.read(msg, iter, size - Field::minLength(), extraValues...);
        if (es == ErrorStatus::Success) {
            iter = toIter;
        }

        return es;
    }

    template <typename TMsg, typename TIter, typename TReader, typename... TExtraValues>
    ErrorStatus readVerify(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TReader&& nextLayerReader,
        TExtraValues... extraValues)
    {
        auto fromIter = iter;

        auto es = nextLayerReader.read(msg, iter, size - Field::minLength(), extraValues...);
        if ((es == ErrorStatus::NotEnoughData) ||
            (es == ErrorStatus::ProtocolError)) {
            return es;
        }

        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        COMMS_ASSERT(len <= size);
        auto remSize = size - len;
        auto checksumEs = field.read(iter, remSize);
        if (checksumEs == ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, remSize, extraValues...);
        }

        if (checksumEs != ErrorStatus::Success) {
            BaseImpl::resetMsg(msg);
            return checksumEs;
        }

        bool checksumValid = false;
        auto checksum = 
            static_cast<ExtendingClass*>(this)->calculateChecksum(
                BaseImpl::toMsgPtr(msg),
                fromIter,
                len,
                checksumValid);

        if (!checksumValid) {
            return comms::ErrorStatus::ProtocolError;
        }

        auto expectedValue = field.value();

        if (expectedValue != static_cast<decltype(expectedValue)>(checksum)) {
            BaseImpl::resetMsg(msg);
            return ErrorStatus::ProtocolError;
        }

        return es;
    }

    template <typename TMsg, typename TIter, typename TReader, typename... TExtraValues>
    ErrorStatus readInternal(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TReader&& nextLayerReader,
        VerifyBeforeReadTag<>,
        TExtraValues... extraValues)
    {
        return
            verifyRead(
                field,
                msg,
                iter,
                size,
                std::forward<TReader>(nextLayerReader),
                extraValues...);
    }

    template <typename TMsg, typename TIter, typename TReader, typename... TExtraValues>
    ErrorStatus readInternal(
        Field& field,
        TMsg& msg,
        TIter& iter,
        std::size_t size,
        TReader&& nextLayerReader,
        VerifyAfterReadTag<>,
        TExtraValues... extraValues)
    {
        return
            readVerify(
                field,
                msg,
                iter,
                size,
                std::forward<TReader>(nextLayerReader),
                extraValues...);
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalRandomAccess(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto fromIter = iter;
        auto es = nextLayerWriter.write(msg, iter, size);
        if ((es != comms::ErrorStatus::Success) &&
            (es != comms::ErrorStatus::UpdateRequired)) {
            return es;
        }

        COMMS_ASSERT(fromIter <= iter);
        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        auto remSize = size - len;

        if (remSize < Field::maxLength()) {
            return comms::ErrorStatus::BufferOverflow;
        }

        if (es == comms::ErrorStatus::UpdateRequired) {
            auto esTmp = field.write(iter, remSize);
            static_cast<void>(esTmp);
            COMMS_ASSERT(esTmp == comms::ErrorStatus::Success);
            return es;
        }

        bool checksumValid = false;
        auto checksum = 
            static_cast<const ExtendingClass*>(this)->calculateChecksum(
                &msg,
                fromIter,
                len,
                checksumValid);

        if (!checksumValid) {
            return comms::ErrorStatus::ProtocolError;
        }

        using FieldValueType = typename Field::ValueType;
        field.value() = static_cast<FieldValueType>(checksum);

        return field.write(iter, remSize);
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalOutput(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto es = nextLayerWriter.write(msg, iter, size - Field::maxLength());
        if ((es != comms::ErrorStatus::Success) &&
            (es != comms::ErrorStatus::UpdateRequired)) {
            return es;
        }

        auto esTmp = field.write(iter, Field::maxLength());
        static_cast<void>(esTmp);
        COMMS_ASSERT(esTmp == comms::ErrorStatus::Success);
        return comms::ErrorStatus::UpdateRequired;
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternal(
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
    ErrorStatus writeInternal(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        std::output_iterator_tag) const
    {
        return writeInternalOutput(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TMsg, typename TIter>
    ErrorStatus fieldUpdateInternal(const TMsg* msgPtr, TIter from, TIter to, std::size_t size, Field& field) const
    {
        COMMS_ASSERT(from <= to);
        auto len = static_cast<std::size_t>(std::distance(from, to));
        COMMS_ASSERT(len == (size - Field::maxLength()));
        auto remSize = size - len;

        bool checksumValid = false;
        auto checksum = 
            static_cast<const ExtendingClass*>(this)->calculateChecksum(
                msgPtr,
                from,
                len,
                checksumValid);

        if (!checksumValid) {
            return comms::ErrorStatus::ProtocolError;
        }

        using FieldValueType = typename Field::ValueType;
        field.value() = static_cast<FieldValueType>(checksum);
        return field.write(to, remSize);
    }
};

namespace details
{
template <typename T>
struct ChecksumLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField, typename TCalc, typename TNextLayer, typename... TOptions>
struct ChecksumLayerCheckHelper<ChecksumLayer<TField, TCalc, TNextLayer, TOptions...> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref ChecksumLayer
/// @related ChecksumLayer
template <typename T>
constexpr bool isChecksumLayer()
{
    return details::ChecksumLayerCheckHelper<T>::Value;
}

}  // namespace protocol

}  // namespace comms


