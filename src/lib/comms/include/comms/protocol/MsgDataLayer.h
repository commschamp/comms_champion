//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include <tuple>
#include <iterator>
#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "comms/field/VarSizeArray.h"
#include "comms/field/BasicIntValue.h"

namespace comms
{

namespace protocol
{

template <
    typename TMessage,
    typename TField =
        comms::field::VarSizeArray<
            typename TMessage::Field,
            comms::field::BasicIntValue<typename TMessage::Field, std::uint8_t>
        >
>
class MsgDataLayer
{
public:

    typedef TField Field;
    typedef std::tuple<Field> AllFields;

    typedef void MsgPtr;

    typedef TMessage Message;
    typedef typename TMessage::ReadIterator ReadIterator;
    typedef typename TMessage::WriteIterator WriteIterator;

    MsgDataLayer() = default;
    MsgDataLayer(const MsgDataLayer&) = default;
    MsgDataLayer(MsgDataLayer&&) = default;
    ~MsgDataLayer() = default;
    MsgDataLayer& operator=(const MsgDataLayer&) = default;
    MsgDataLayer& operator=(MsgDataLayer&&) = default;

    template <typename TMsgPtr>
    static ErrorStatus read(
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        GASSERT(msgPtr);
        auto result = msgPtr->read(iter, size);
        if ((result == ErrorStatus::NotEnoughData) &&
            (missingSize != nullptr)) {
            if (size < msgPtr->length()) {
                *missingSize = msgPtr->length() - size;
            }
            else {
                *missingSize = 1;
            }
        }
        return result;
    }

    template <std::size_t TIdx, typename TAllFields, typename TMsgPtr>
    static ErrorStatus readFieldsCached(
        TAllFields& allFields,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize = nullptr)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be read when MsgDataLayer is reached");

        typedef typename std::iterator_traits<ReadIterator>::iterator_category IterType;
        auto& dataField = std::get<TIdx>(allFields);

        typedef typename std::decay<decltype(dataField)>::type FieldType;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");
        return readWithFieldCachedInternal(dataField, msgPtr, iter, size, missingSize, IterType());
    }

    static ErrorStatus write(
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size)
    {
        return msg.write(iter, size);
    }

    template <std::size_t TIdx, typename TAllFields>
    static ErrorStatus writeFieldsCached(
        TAllFields& allFields,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size)
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be written when MsgDataLayer is reached");

        typedef typename std::iterator_traits<WriteIterator>::iterator_category IterType;
        auto& dataField = std::get<TIdx>(allFields);

        typedef typename std::decay<decltype(dataField)>::type FieldType;
        static_assert(
            std::is_same<Field, FieldType>::value,
            "Field has wrong type");
        return writeWithFieldCachedInternal(dataField, msg, iter, size, IterType());
    }

    template <typename TIter>
    static comms::ErrorStatus update(TIter& iter, std::size_t size)
    {
        std::advance(iter, size);
        return comms::ErrorStatus::Success;
    }

    template <std::size_t TIdx, typename TAllFields, typename TUpdateIter>
    ErrorStatus updateFieldsCached(
        TAllFields& allFields,
        TUpdateIter& iter,
        std::size_t size) const
    {
        static_assert(comms::util::IsTuple<TAllFields>::Value,
                                        "Expected TAllFields to be tuple.");

        static_assert((TIdx + 1) == std::tuple_size<TAllFields>::value,
                    "All fields must be written when MsgDataLayer is reached");

        static_cast<void>(allFields);
        std::advance(iter, size);
        return comms::ErrorStatus::Success;
    }

    static constexpr std::size_t length()
    {
        return 0U;
    }

    static constexpr std::size_t length(const TMessage& msg)
    {
        return msg.length();
    }

private:
    template <typename TMsgPtr>
    static ErrorStatus readWithFieldCachedInternal(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize,
        std::random_access_iterator_tag)
    {
        return readWithFieldCachedRandomAccess(field, msgPtr, iter, size, missingSize);
    }

    template <typename TMsgPtr>
    static ErrorStatus readWithFieldCachedInternal(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize,
        std::input_iterator_tag)
    {
        static_cast<void>(field);
        GASSERT(!"Not supported yet");
        return read(msgPtr, iter, size, missingSize);
    }

    template <typename TMsgPtr>
    static ErrorStatus readWithFieldCachedRandomAccess(
        Field& field,
        TMsgPtr& msgPtr,
        ReadIterator& iter,
        std::size_t size,
        std::size_t* missingSize)
    {
        ReadIterator dataIter = iter;
        auto es = read(msgPtr, iter, size, missingSize);
        auto dataSize = static_cast<std::size_t>(std::distance(dataIter, iter));
        auto dataEs = field.read(dataIter, dataSize);
        GASSERT((!msgPtr) || (dataSize == msgPtr->length()));
        GASSERT(dataEs == comms::ErrorStatus::Success);
        return es;
    }

    static ErrorStatus writeWithFieldCachedInternal(
        Field& field,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size,
        std::random_access_iterator_tag)
    {
        return writeWithFieldCachedRandomAccess(field, msg, iter, size);
    }

    static ErrorStatus writeWithFieldCachedInternal(
        Field& field,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size,
        std::output_iterator_tag)
    {
        return writeWithFieldCachedOutput(field, msg, iter, size);
    }

    static ErrorStatus writeWithFieldCachedRandomAccess(
        Field& field,
        const TMessage& msg,
        WriteIterator& iter,
        std::size_t size)
    {
        auto dataReadIter = iter;
        auto es = write(msg, iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto dataEs = field.read(dataReadIter, msg.length());
        GASSERT(dataEs == comms::ErrorStatus::Success);
        static_cast<void>(dataEs);
        return comms::ErrorStatus::Success;
    }

    template <typename TCollection>
    static ErrorStatus writeWithFieldCachedOutput(
        Field& field,
        const TMessage& msg,
        std::back_insert_iterator<TCollection>& iter,
        std::size_t size)
    {
        auto es = write(msg, iter, size);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        TCollection col;
        auto dataWriteIter = std::back_inserter(col);
        auto dataWriteEs = write(msg, dataWriteIter, size);
        GASSERT(dataWriteEs == comms::ErrorStatus::Success);
        static_cast<void>(dataWriteEs);

        auto dataReadIter = col.cbegin();
        auto dataReadEs = field.read(dataReadIter, col.size());
        GASSERT(dataReadEs == comms::ErrorStatus::Success);
        static_cast<void>(dataReadEs);

        return comms::ErrorStatus::Success;
    }
};

}  // namespace protocol

}  // namespace comms



