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

#include "comms/ErrorStatus.h"
#include "comms/util/Tuple.h"

template <typename TField, typename TNextLayer>
class ProtocolLayerBase
{
public:
    typedef TField Field;

    typedef TNextLayer NextLayer;

    typedef typename std::decay<
        decltype(
            std::tuple_cat(
                std::declval<std::tuple<Field> >(),
                std::declval<typename TNextLayer::AllFields>())
            )
        >::type AllFields;

    typedef typename NextLayer::MsgPtr MsgPtr;

    typedef typename NextLayer::ReadIterator ReadIterator;

    typedef typename NextLayer::WriteIterator WriteIterator;

    NextLayer& nextLayer()
    {
        return nextLayer_;
    }

    const NextLayer& nextLayer() const
    {
        return nextLayer_;
    }

    const Field& field() const
    {
        return field_;
    }

    template <typename TFields>
    void getAllFields(TFields& fields)
    {
        getAllFieldsFromIdx<0>(fields);
    }

    template <std::size_t TIdx, typename TFields>
    void getAllFieldsFromIdx(TFields& fields)
    {
        static_assert(TIdx < std::tuple_size<TFields>::value,
            "Tuple of fields is too small.");

        std::get<TIdx>(fields) = field_;
        nextLayer_.getAllFieldsFromIdx<TIdx + 1>(fields);
    }

    constexpr std::size_t length() const
    {
        return field_.length() + nextLayer_.length();
    }

    template <typename TMsg>
    constexpr std::size_t length(const TMsg& msg) const
    {
        return field_.length() + nextLayer_.length(msg);
    }

    template <typename TIter>
    comms::ErrorStatus update(TIter& iter, std::size_t size) const
    {
        auto length = field_.length();
        std::advance(iter, length);
        return nextLayer_.update(iter, size - length);
    }

private:

    static_assert (comms::util::IsTuple<AllFields>::Value, "Must be tuple");
    Field field_;
    NextLayer nextLayer_;
};

