//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file 
/// Contains definition of @ref comms::protocol::ChecksumPrefixLayer

#pragma once

#include <iterator>
#include <type_traits>
#include "comms/field/IntValue.h"
#include "comms/protocol/details/ProtocolLayerBase.h"
#include "comms/protocol/details/ChecksumLayerOptionsParser.h"
#include "comms/protocol/details/ProtocolLayerExtendingClassHelper.h"
#include "comms/util/type_traits.h"

namespace comms
{

namespace protocol
{
/// @brief Protocol layer that is responsible to calculate checksum on the
///     data written by all the wrapped internal layers and prepend it before
///     the written data. When reading, this layer is responsible to verify
///     the checksum reported at the beginning of the read data.
/// @details This protocol layer handles checksum value that usually precedes the
///     data. Some protocols use checksum value that follows the data. In this
///     case use @ref ChecksumLayer instead.
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
///         @b ChecksumPrefixLayer will invoke @b read operation of inner (wrapped) layers
///         and only if it is successful, it will calculate and verify the
///         checksum value. Usage of @ref comms::option::def::ChecksumLayerVerifyBeforeRead
///         modifies the default behaviour by forcing the checksum verification
///         prior to invocation of @b read operation in the wrapped layer(s).
///     @li  @ref comms::option::ExtendingClass - Use this option to provide a class
///         name of the extending class, which can be used to extend existing functionality.
///         See also @ref page_custom_checksum_layer tutorial page.
/// @headerfile comms/protocol/ChecksumPrefixLayer.h
template <typename TField, typename TCalc, typename TNextLayer, typename... TOptions>
class ChecksumPrefixLayer : public
        details::ProtocolLayerBase<
            TField,
            TNextLayer,
            details::ProtocolLayerExtendingClassT<
                ChecksumPrefixLayer<TField, TCalc, TNextLayer, TOptions...>,
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
                ChecksumPrefixLayer<TField, TCalc, TNextLayer, TOptions...>,
                details::ChecksumLayerOptionsParser<TOptions...>
            >,            
            comms::option::def::ProtocolLayerDisallowReadUntilDataSplit
        >;

public:
    /// @brief Parsed options
    using ParsedOptions = details::ChecksumLayerOptionsParser<TOptions...>;

    /// @brief Type of the field object used to read/write checksum value.
    using Field = typename BaseImpl::Field;

    /// @brief Default constructor.
    ChecksumPrefixLayer() = default;

    /// @brief Copy constructor
    ChecksumPrefixLayer(const ChecksumPrefixLayer&) = default;

    /// @brief Move constructor
    ChecksumPrefixLayer(ChecksumPrefixLayer&&) = default;

    /// @brief Destructor.
    ~ChecksumPrefixLayer() noexcept = default;

    /// @brief Copy assignment
    ChecksumPrefixLayer& operator=(const ChecksumPrefixLayer&) = default;

    /// @brief Move assignment
    ChecksumPrefixLayer& operator=(ChecksumPrefixLayer&&) = default;

    /// @brief Customized read functionality, invoked by @ref read().
    /// @details First, reads the expected checksum value field, then
    ///     executes the read() member function of the next layer.
    ///     If the call returns comms::ErrorStatus::Success, it calculated the
    ///     checksum of the read data and
    ///     compares it to the calculated. If checksums match,
    ///     comms::ErrorStatus::Success is returned, otherwise
    ///     function returns comms::ErrorStatus::ProtocolError.
    /// @tparam TMsg Type of the @b msg parameter.
    /// @tparam TIter Type of iterator used for reading.
    /// @tparam TNextLayerReader next layer reader object type.
    /// @param[out] field Field object to read.
    /// @param[in, out] msg Reference to smart pointer that already holds or
    ///     will hold allocated message object
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
    comms::ErrorStatus doRead(
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

        auto* msgPtr = BaseImpl::toMsgPtr(msg);
        auto& thisObj = BaseImpl::thisLayer();
        auto beforeFieldReadIter = iter;
        auto checksumEs = thisObj.readField(msgPtr, field, iter, size);        
        if (checksumEs == ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, extraValues...);
        }

        if (checksumEs != ErrorStatus::Success) {
            return checksumEs;
        }

        using VerifyTag = 
            typename comms::util::LazyShallowConditional<
                ParsedOptions::HasVerifyBeforeRead
            >::template Type<
                VerifyBeforeReadTag,
                VerifyAfterReadTag
            >;        

        auto fieldLen = static_cast<std::size_t>(std::distance(beforeFieldReadIter, iter));
        return
            readInternal(
                field,
                msg,
                iter,
                size - fieldLen,
                std::forward<TNextLayerReader>(nextLayerReader),
                VerifyTag(),
                extraValues...);
    }

    /// @brief Customized write functionality, invoked by @ref write().
    /// @details First, reserves the appropriate number of bytes in the
    ///     output buffer which are supposed to contain valid checksum
    ///     value, then executes the write() member function of the next layer.
    ///     If the call returns comms::ErrorStatus::Success and it is possible
    ///     to re-read what has been written (random access iterator is used
    ///     for writing), the real checksum value is calculated and updated in the
    ///     previously reserved area. In case non-random access iterator
    ///     type is used for writing (for example std::back_insert_iterator), then
    ///     this function returns
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
    comms::ErrorStatus doWrite(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TNextLayerWriter&& nextLayerWriter) const
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using Tag = typename std::iterator_traits<IterType>::iterator_category;

        field.value() = 0;
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
        auto checksumIter = iter;
        std::advance(iter, Field::maxLength());

        auto fromIter = iter;
        auto es = nextLayerUpdater.update(iter, size - Field::maxLength());
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto* msgPtr = static_cast<typename BaseImpl::MsgPtr::element_type*>(nullptr);
        return fieldUpdateInternal(msgPtr, checksumIter, fromIter, iter, size, field);
    }

    /// @brief Customized update functionality, invoked by @ref update().
    /// @details Should be called when @ref doWrite() returns comms::ErrorStatus::UpdateRequired.
    /// @tparam TMsg Type of recently written message.
    /// @tparam TIter Type of iterator used for updating.
    /// @tparam TNextLayerWriter next layer updater object type.
    /// @param[in] msg Reference to recently written message object.
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
        auto& thisObj = BaseImpl::thisLayer();
        auto checksumIter = iter;
        auto fieldLen = thisObj.doFieldLength(msg);
        std::advance(iter, fieldLen);

        auto fromIter = iter;
        auto es = nextLayerUpdater.update(msg, iter, size - fieldLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return fieldUpdateInternal(&msg, checksumIter, fromIter, iter, size, field);
    }

protected:
    /// @brief Read the checksum field.
    /// @details The default implementation invokes @b read() operation of the 
    ///     passed field object. The function can be overriden by the extending class.
    /// @param[in] msgPtr Pointer to message object (if available), can be nullptr.
    /// @param[out] field Field object value of which needs to be populated
    /// @param[in, out] iter Iterator used for reading, expected to be advanced
    /// @param[in] len Length of the input buffer
    /// @note May be non-static in the extending class
    template <typename TMsg, typename TIter>
    static comms::ErrorStatus readField(const TMsg* msgPtr, Field& field, TIter& iter, std::size_t len)
    {
        static_cast<void>(msgPtr);
        return field.read(iter, len);
    }

    /// @brief Write the checksum field.
    /// @details The default implementation invokes @b write() operation of the 
    ///     passed field object. The function can be overriden by the extending class.
    /// @param[in] msgPtr Pointer to message object (if available), can be nullptr.
    /// @param[out] field Field object value of which needs to be written
    /// @param[in, out] iter Iterator used for writing, expected to be advanced
    /// @param[in] len Length of the output buffer
    /// @note May be non-static in the extending class, but needs to be const.
    template <typename TMsg, typename TIter>
    static comms::ErrorStatus writeField(const TMsg* msgPtr, const Field& field, TIter& iter, std::size_t len)
    {
        static_cast<void>(msgPtr);
        return field.write(iter, len);
    }

    /// @brief Calculate checksum.
    /// @details The default implementation invokes @b operator() of provided
    ///     calculation algorithm (@b TCalc template parameter). 
    ///     The function can be overriden by the extending class.
    /// @param[in] msg Pointer to message object (if available), can be nullptr.
    /// @param[in, out] iter Iterator used for reading data, expected to be advanced
    /// @param[in] len Length of the output buffer
    /// @param[out] checksumValid Indication of whether the return checksum is valid, 
    ///     must be populated.
    /// @return The checksum value, can be of any type, but should field 
    ///     into @b Field::ValueType.
    /// @note May be non-static in the extending class, but needs to be const.
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
        auto& thisObj = BaseImpl::thisLayer();
        auto* msgPtr = BaseImpl::toMsgPtr(msg);

        bool checksumValid = false;
        auto checksum = 
            thisObj.calculateChecksum(
                msgPtr,
                fromIter,
                size,
                checksumValid);

        if (!checksumValid) {
            return comms::ErrorStatus::ProtocolError;
        }

        auto expectedValue = field.value();

        if (expectedValue != static_cast<decltype(expectedValue)>(checksum)) {
            BaseImpl::resetMsg(msg);
            return ErrorStatus::ProtocolError;
        }

        return nextLayerReader.read(msg, iter, size, extraValues...);
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

        auto es = nextLayerReader.read(msg, iter, size, extraValues...);
        if ((es == ErrorStatus::NotEnoughData) ||
            (es == ErrorStatus::ProtocolError)) {
            return es;
        }

        auto* msgPtr = BaseImpl::toMsgPtr(msg);
        auto& thisObj = BaseImpl::thisLayer();
        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        bool checksumValid = false;
        auto checksum = 
            thisObj.calculateChecksum(
                msgPtr,
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
        auto& thisObj = BaseImpl::thisLayer();
        auto checksumIter = iter;
        auto es = thisObj.writeField(&msg, field, iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto checksumLen =
            static_cast<std::size_t>(std::distance(checksumIter, iter));

        auto fromIter = iter;
        es = nextLayerWriter.write(msg, iter, size - checksumLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(fromIter <= iter);
        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));

        using FieldValueType = typename Field::ValueType;
        bool checksumValid = false;
        auto checksum = 
            thisObj.calculateChecksum(
                &msg,
                fromIter,
                len,
                checksumValid);

        if (!checksumValid) {
            return comms::ErrorStatus::ProtocolError;
        }

        field.value() = static_cast<FieldValueType>(checksum);
        auto checksumEs = thisObj.writeField(&msg, field, checksumIter, checksumLen);
        static_cast<void>(checksumEs);
        COMMS_ASSERT(checksumEs == comms::ErrorStatus::Success);
        return es;
    }

    template <typename TMsg, typename TIter, typename TWriter>
    ErrorStatus writeInternalOutput(
        Field& field,
        const TMsg& msg,
        TIter& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto& thisObj = BaseImpl::thisLayer();
        auto es = thisObj.writeField(&msg, field, iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto fieldLen = thisObj.doFieldLength(&msg);
        es = nextLayerWriter.write(msg, iter, size - fieldLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

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
    ErrorStatus fieldUpdateInternal(
        const TMsg* msgPtr, 
        TIter checksumIter, 
        TIter from, 
        TIter to, 
        std::size_t size, 
        Field& field) const
    {
        static_cast<void>(size);
        COMMS_ASSERT(from <= to);
        auto& thisObj = BaseImpl::thisLayer();
        auto len = static_cast<std::size_t>(std::distance(from, to));
        auto fieldLen = Field::maxLength();
        if (msgPtr != nullptr) {
            fieldLen = thisObj.doFieldLength(*msgPtr);
        }
        COMMS_ASSERT(len == (size - fieldLen));
        
        bool checksumValid = false;
        auto checksum = 
            thisObj.calculateChecksum(
                msgPtr,
                from,
                len,
                checksumValid);

        if (!checksumValid) {
            return comms::ErrorStatus::ProtocolError;
        }

        using FieldValueType = typename Field::ValueType;
        field.value() = static_cast<FieldValueType>(checksum);
        return field.write(checksumIter, fieldLen);
    }
};


namespace details
{
template <typename T>
struct ChecksumPrefixLayerCheckHelper
{
    static const bool Value = false;
};

template <typename TField, typename TCalc, typename TNextLayer, typename... TOptions>
struct ChecksumPrefixLayerCheckHelper<ChecksumPrefixLayer<TField, TCalc, TNextLayer, TOptions...> >
{
    static const bool Value = true;
};

} // namespace details

/// @brief Compile time check of whether the provided type is
///     a variant of @ref ChecksumPrefixLayer
/// @related ChecksumLayer
template <typename T>
constexpr bool isChecksumPrefixLayer()
{
    return details::ChecksumPrefixLayerCheckHelper<T>::Value;
}


}  // namespace protocol

}  // namespace comms


