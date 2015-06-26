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

template <typename TField, typename TCalc, typename TNextLayer>
class ChecksumLayer : public
    ProtocolLayerBase<TField, TNextLayer, ChecksumLayer<TField, TCalc, TNextLayer> >
{
    typedef ProtocolLayerBase<TField, TNextLayer, ChecksumLayer<TField, TCalc, TNextLayer> > Base;
public:
    /// @brief Pointer to message object
    typedef typename Base::MsgPtr MsgPtr;

    typedef typename Base::Message Message;

    /// @brief Type of read iterator
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Type of write iterator
    typedef typename Base::WriteIterator WriteIterator;

    typedef typename Base::Field Field;

    using Base::ProtocolLayerBase;

    /// @brief Destructor is default
    ~ChecksumLayer() = default;

    /// @brief Copy assignment is default
    ChecksumLayer& operator=(const ChecksumLayer&) = default;

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

    ErrorStatus write(
            const Message& msg,
            WriteIterator& iter,
            std::size_t size) const
    {
        Field field;
        return writeInternal(field, msg, iter, size, Base::createNextLayerWriter(), WriteIteratorCategoryTag());
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
                Base::template createNextLayerCachedFieldsWriter<TIdx>(allFields),
                WriteIteratorCategoryTag());
    }

    template <typename TIter>
    comms::ErrorStatus update(TIter& iter, std::size_t size) const
    {
        Field field;
        return updateInternal(field, iter, size, Base::createNextLayerUpdater());
    }

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


