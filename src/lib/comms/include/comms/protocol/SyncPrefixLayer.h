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

#include "ProtocolLayerBase.h"

namespace comms
{

namespace protocol
{

template <typename TField,
          typename TNextLayer>
class SyncPrefixLayer : public
            ProtocolLayerBase<TField, TNextLayer, SyncPrefixLayer<TField, TNextLayer> >
{
    typedef ProtocolLayerBase<TField, TNextLayer, SyncPrefixLayer<TField, TNextLayer> > Base;

public:
    /// @brief Pointer to message object
    typedef typename Base::MsgPtr MsgPtr;

    typedef typename Base::Message Message;

    /// @brief Type of read iterator
    typedef typename Base::ReadIterator ReadIterator;

    /// @brief Type of write iterator
    typedef typename Base::WriteIterator WriteIterator;

    typedef typename Base::Field Field;

    // Inherit constructors
    SyncPrefixLayer() = default;

    template <typename... TArgs>
    explicit SyncPrefixLayer(TArgs&&... args)
      : Base(std::forward<TArgs>(args)...)
    {
    }

    SyncPrefixLayer(const SyncPrefixLayer&) = default;

    SyncPrefixLayer(SyncPrefixLayer&&) = default;

    ~SyncPrefixLayer() = default;

    template <typename TOtherField>
    void setExpectedPrefix(TOtherField&& field)
    {
        field_ = std::forward<TOtherField>(field);
    }

    const Field& getExpectedPrefix() const
    {
        return field_;
    }

    constexpr std::size_t fieldLength() const
    {
        return field_.length();
    }

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
        return writeInternal(msg, iter, size, Base::createNextLayerWriter());
    }

    template <std::size_t TIdx, typename TAllFields>
    ErrorStatus writeFieldsCached(
        TAllFields& allFields,
        const Message& msg,
        WriteIterator& iter,
        std::size_t size) const
    {
        auto& field = Base::template getField<TIdx>(allFields);
        field = field_;

        return
            writeInternal(
                msg,
                iter,
                size,
                Base::template createNextLayerCachedFieldsWriter<TIdx>(allFields));
    }

private:

    template <typename TMsgPtr, typename TReader>
    ErrorStatus readInternal(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize,
        TReader&& reader)
    {
        auto es = field.read(iter, size);
        if (es == ErrorStatus::NotEnoughData) {
            Base::updateMissingSize(field, size, missingSize);
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        if (field != field_) {
            // doesn't match expected
            return ErrorStatus::ProtocolError;
        }

        return reader.read(msgPtr, iter, size - field.length(), missingSize);
    }

    template <typename TWriter>
    ErrorStatus writeInternal(
        const Message& msg,
        WriteIterator& iter,
        std::size_t size,
        TWriter&& nextLayerWriter) const
    {
        auto es = field_.write(iter, size);
        if (es != ErrorStatus::Success) {
            return es;
        }

        GASSERT(field_.length() <= size);
        return nextLayerWriter.write(msg, iter, size - field_.length());
    }

    Field field_;
};

}  // namespace protocol

}  // namespace comms


