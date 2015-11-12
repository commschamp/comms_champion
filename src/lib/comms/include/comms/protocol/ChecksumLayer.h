//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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
template <typename TField, typename TCalc, typename TNextLayer>
class ChecksumLayer : public
    ProtocolLayerBase<TField, TNextLayer, ChecksumLayer<TField, TCalc, TNextLayer> >
{
    typedef ProtocolLayerBase<TField, TNextLayer, ChecksumLayer<TField, TCalc, TNextLayer> > Base;
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

    /// @brief Type of the field object used to read/write checksum value.
    typedef typename Base::Field Field;

    /// @brief Default constructor.
    ChecksumLayer() = default;

    /// @brief Constructor of any number of arguments.
    /// @details All the arguments are passed to the constructor of the
    ///     ProtocolLayerBase (which is a base of this class), which it turn
    ///     forwards them to the constructor of the next layer.
    template <typename... TArgs>
    explicit ChecksumLayer(TArgs&&... args)
      : Base(std::forward<TArgs>(args)...)
    {
    }

    /// @brief Copy constructor
    ChecksumLayer(const ChecksumLayer&) = default;

    /// @brief Move constructor
    ChecksumLayer(ChecksumLayer&&) = default;

    /// @brief Destructor.
    ~ChecksumLayer() = default;

    /// @brief Copy assignment
    ChecksumLayer& operator=(const ChecksumLayer&) = default;

    /// @brief Move assignment
    ChecksumLayer& operator=(ChecksumLayer&&) = default;

    /// @brief Deserialise message from the input data sequence.
    /// @details First, executes the read() member function of the next layer.
    ///     If the call returns comms::ErrorStatus::Success, it calculated the
    ///     checksum of the read data, reads the expected checksum value and
    ///     compares it to the calculated. If checksums match,
    ///     comms::ErrorStatus::Success is returned, otherwise
    ///     function returns comms::ErrorStatus::ProtocolError.
    /// @param[in, out] msgPtr Reference to smart pointer that already holds or
    ///     will hold allocated message object
    /// @param[in, out] iter Input iterator used for reading.
    /// @param[in] size Size of the data in the sequence
    /// @param[out] missingSize If not nullptr and return value is
    ///     comms::ErrorStatus::NotEnoughData it will contain
    ///     minimal missing data length required for the successful
    ///     read attempt.
    /// @return Status of the read operation.
    /// @pre Iterator must be "random access" one.
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
        return writeInternal(field, msg, iter, size, Base::createNextLayerWriter(), WriteIteratorCategoryTag());
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
                Base::template createNextLayerCachedFieldsWriter<TIdx>(allFields),
                WriteIteratorCategoryTag());
    }

    /// @brief Update written dummy checksum with proper value.
    /// @details Should be called when write() returns comms::ErrorStatus::UpdateRequired.
    /// @param[in, out] iter Any random access iterator.
    /// @param[in] size Number of bytes that have been written using write().
    /// @return Status of the update operation.
    template <typename TIter>
    comms::ErrorStatus update(TIter& iter, std::size_t size) const
    {
        Field field;
        return updateInternal(field, iter, size, Base::createNextLayerUpdater());
    }

    /// @cond SKIP_DOC
    template <std::size_t TIdx, typename TAllFields, typename TUpdateIter>
    ErrorStatus updateFieldsCached(
        TAllFields& allFields,
        TUpdateIter& iter,
        std::size_t size) const
    {
        auto& field = Base::template getField<TIdx>(allFields);
        return
            updateInternal(
                field,
                iter,
                size,
                Base::template createNextLayerCachedFieldsUpdater<TIdx>(allFields));
    }
    /// @endcond

private:
    typedef typename std::iterator_traits<WriteIterator>::iterator_category WriteIteratorCategoryTag;

    static_assert(comms::field::isIntValue<Field>(),
        "The checksum field is expected to be of IntValue type");

    static_assert(Field::minLength() == Field::maxLength(),
        "The checksum field is expected to be of fixed length");

    static_assert(std::is_same<typename std::iterator_traits<ReadIterator>::iterator_category, std::random_access_iterator_tag>::value,
        "The ReadIterator is expected to be a random access one");

    template <typename TMsgPtr, typename TReader>
    ErrorStatus readInternal(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize,
        TReader&& nextLayerReader)
    {
        if (size < Field::minLength()) {
            return ErrorStatus::NotEnoughData;
        }

        auto fromIter = iter;

        auto es = nextLayerReader.read(msgPtr, iter, size - Field::minLength(), missingSize);
        if (es != ErrorStatus::Success) {
            return es;
        }

        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        GASSERT(len <= size);
        auto remSize = size - len;
        es = field.read(iter, remSize);
        if (es == ErrorStatus::NotEnoughData) {
            Base::updateMissingSize(field, remSize, missingSize);
        }

        if (es != ErrorStatus::Success) {
            msgPtr.reset();
            return es;
        }

        auto checksum = TCalc()(fromIter, len);
        auto expectedValue = field.value();

        if (expectedValue != static_cast<decltype(expectedValue)>(checksum)) {
            msgPtr.reset();
            return ErrorStatus::ProtocolError;
        }

        return ErrorStatus::Success;
    }

    template <typename TWriter>
    ErrorStatus writeInternalRandomAccess(
        Field& field,
        const Message& msg,
        WriteIterator& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto fromIter = iter;
        auto es = nextLayerWriter.write(msg, iter, size);
        if ((es != comms::ErrorStatus::Success) &&
            (es != comms::ErrorStatus::UpdateRequired)) {
            return es;
        }

        GASSERT(fromIter <= iter);
        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        auto remSize = size - len;

        if (remSize < Field::maxLength()) {
            return comms::ErrorStatus::BufferOverflow;
        }

        if (es == comms::ErrorStatus::UpdateRequired) {
            auto esTmp = field.write(iter, remSize);
            static_cast<void>(esTmp);
            GASSERT(esTmp == comms::ErrorStatus::Success);
            return es;
        }

        typedef typename Field::ValueType FieldValueType;
        auto checksum = TCalc()(fromIter, len);
        field.value() = static_cast<FieldValueType>(checksum);

        return field.write(iter, size);
    }

    template <typename TWriter>
    ErrorStatus writeInternalOutput(
        Field& field,
        const Message& msg,
        WriteIterator& iter,
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
        GASSERT(esTmp == comms::ErrorStatus::Success);
        return comms::ErrorStatus::UpdateRequired;
    }

    template <typename TWriter>
    ErrorStatus writeInternal(
        Field& field,
        const Message& msg,
        WriteIterator& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        std::random_access_iterator_tag) const
    {
        return writeInternalRandomAccess(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TWriter>
    ErrorStatus writeInternal(
        Field& field,
        const Message& msg,
        WriteIterator& iter,
        std::size_t size,
        TWriter&& nextLayerWriter,
        std::output_iterator_tag) const
    {
        return writeInternalOutput(field, msg, iter, size, std::forward<TWriter>(nextLayerWriter));
    }

    template <typename TIter, typename TUpdater>
    ErrorStatus updateInternal(
        Field& field,
        TIter& iter,
        std::size_t size,
        TUpdater&& nextLayerUpdater) const
    {
        auto fromIter = iter;
        auto es = nextLayerUpdater.update(iter, size - Field::maxLength());
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        GASSERT(fromIter <= iter);
        auto len = static_cast<std::size_t>(std::distance(fromIter, iter));
        GASSERT(len == (size - Field::maxLength()));
        auto remSize = size - len;
        typedef typename Field::ValueType FieldValueType;
        field.value() = static_cast<FieldValueType>(TCalc()(fromIter, len));
        es = field.write(iter, remSize);
        return es;
    }
};

}  // namespace protocol

}  // namespace comms


