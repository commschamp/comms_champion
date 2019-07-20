//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include <iterator>
#include <type_traits>
#include "comms/field/IntValue.h"
#include "ProtocolLayerBase.h"
#include "details/ChecksumLayerOptionsParser.h"

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
/// @headerfile comms/protocol/ChecksumPrefixLayer.h
template <typename TField, typename TCalc, typename TNextLayer, typename... TOptions>
class ChecksumPrefixLayer : public
        ProtocolLayerBase<
            TField,
            TNextLayer,
            ChecksumPrefixLayer<TField, TCalc, TNextLayer, TOptions...>,
            comms::option::def::ProtocolLayerDisallowReadUntilDataSplit
        >
{
    using BaseImpl =
        ProtocolLayerBase<
            TField,
            TNextLayer,
            ChecksumPrefixLayer<TField, TCalc, TNextLayer, TOptions...>,
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

        if (size < Field::minLength()) {
            return ErrorStatus::NotEnoughData;
        }

        auto beforeFieldReadIter = iter;
        auto checksumEs = field.read(iter, Field::minLength());
        if (checksumEs == ErrorStatus::NotEnoughData) {
            BaseImpl::updateMissingSize(field, size, extraValues...);
        }

        if (checksumEs != ErrorStatus::Success) {
            return checksumEs;
        }

        std::size_t fieldLen = static_cast<std::size_t>(std::distance(beforeFieldReadIter, iter));
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

        COMMS_ASSERT(fromIter <= iter);
        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        COMMS_ASSERT(len == (size - Field::maxLength()));
        using FieldValueType = typename Field::ValueType;
        field.value() = static_cast<FieldValueType>(TCalc()(fromIter, len));
        es = field.write(checksumIter, Field::maxLength());
        return es;
    }

private:
    static_assert(comms::field::isIntValue<Field>(),
        "The checksum field is expected to be of IntValue type");

    static_assert(Field::minLength() == Field::maxLength(),
        "The checksum field is expected to be of fixed length");

    struct VerifyBeforeReadTag {};
    struct VerifyAfterReadTag {};

    using VerifyTag = typename
        std::conditional<
            ParsedOptions::HasVerifyBeforeRead,
            VerifyBeforeReadTag,
            VerifyAfterReadTag
        >::type;


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

        auto checksum = TCalc()(fromIter, size);
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

        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        auto checksum = TCalc()(fromIter, len);
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
        VerifyBeforeReadTag,
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
        VerifyAfterReadTag,
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
        auto checksumIter = iter;
        auto es = field.write(iter, size);
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
        auto checksum = TCalc()(fromIter, len);
        field.value() = static_cast<FieldValueType>(checksum);

        auto checksumEs = field.write(checksumIter, checksumLen);
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
        auto es = field.write(iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        es = nextLayerWriter.write(msg, iter, size - Field::maxLength());
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


