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
#include "comms/util/Tuple.h"

namespace comms
{

namespace protocol
{

template <typename TMessage>
class MsgDataLayer
{
public:

    typedef std::tuple<> AllFields;

    typedef void MsgPtr;

    typedef TMessage Message;
    typedef typename TMessage::ReadIterator ReadIterator;
    typedef typename TMessage::WriteIterator WriteIterator;

    template <std::size_t TIdx, typename TFields>
    static void getAllFieldsFromIdx(TFields& fields) {
        static_assert(TIdx == std::tuple_size<TFields>::value - 1,
            "Tuple of fields is too big.");
        static_cast<void>(fields);
    }

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

        static_assert(TIdx == std::tuple_size<TAllFields>::value,
                    "All fields must be read when MsgDataLayer is reached");

        static_cast<void>(allFields);
        return read(msgPtr, iter, size, missingSize);
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

        static_assert(TIdx == std::tuple_size<TAllFields>::value,
                    "All fields must be written when MsgDataLayer is reached");

        static_cast<void>(allFields);
        return write(msg, iter, size);
    }

    template <typename TIter>
    static comms::ErrorStatus update(TIter& iter, std::size_t size)
    {
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

};

}  // namespace protocol

}  // namespace comms



