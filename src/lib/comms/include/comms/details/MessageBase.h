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

#include <cstdint>
#include <tuple>

#include "comms/traits.h"
#include "comms/options.h"
#include "comms/EmptyHandler.h"
#include "comms/util/Tuple.h"

namespace comms
{

namespace details
{

template <typename... TOptions>
class MessageBase;

template <>
class MessageBase<>
{
protected:
    typedef unsigned MsgIdType;
    typedef comms::traits::endian::Big Endian;
    typedef const std::uint8_t* ReadIterator;
    typedef std::uint8_t* WriteIterator;
    typedef comms::EmptyHandler Handler;

    typedef std::tuple<> AllOptions;
};

template <typename T, typename... TOptions>
class MessageBase<comms::option::MsgIdType<T>, TOptions...>
                                            : public MessageBase<TOptions...>
{
    typedef comms::option::MsgIdType<T> Option;
    typedef MessageBase<TOptions...> Base;
protected:
    typedef typename Option::Type MsgIdType;
    typedef comms::util::TupleCatT<std::tuple<Option>, typename Base::AllOptions> AllOptions;

};

template <typename TEndian, typename... TOptions>
class MessageBase<comms::option::Endian<TEndian>, TOptions...>
                                            : public MessageBase<TOptions...>
{
    typedef comms::option::Endian<TEndian> Option;
    typedef MessageBase<TOptions...> Base;
protected:
    typedef typename Option::Type Endian;
    typedef comms::util::TupleCatT<std::tuple<Option>, typename Base::AllOptions> AllOptions;
};


template <typename TIter, typename... TOptions>
class MessageBase<comms::option::ReadIterator<TIter>, TOptions...>
                                            : public MessageBase<TOptions...>
{
    typedef comms::option::ReadIterator<TIter> Option;
    typedef MessageBase<TOptions...> Base;
protected:
    typedef typename Option::Type ReadIterator;
    typedef comms::util::TupleCatT<std::tuple<Option>, typename Base::AllOptions> AllOptions;
};

template <typename TIter, typename... TOptions>
class MessageBase<comms::option::WriteIterator<TIter>, TOptions...>
                                            : public MessageBase<TOptions...>
{
    typedef comms::option::WriteIterator<TIter> Option;
    typedef MessageBase<TOptions...> Base;
protected:
    typedef typename Option::Type WriteIterator;
    typedef comms::util::TupleCatT<std::tuple<Option>, typename Base::AllOptions> AllOptions;
};

template <typename T, typename... TOptions>
class MessageBase<comms::option::Handler<T>, TOptions...>
                                            : public MessageBase<TOptions...>
{
    typedef comms::option::Handler<T> Option;
    typedef MessageBase<TOptions...> Base;
protected:
    typedef typename Option::Type Handler;
    typedef comms::util::TupleCatT<std::tuple<Option>, typename Base::AllOptions> AllOptions;
};

template <typename... TBundledOptions, typename... TOptions>
class MessageBase<std::tuple<TBundledOptions...>, TOptions...>
                            : public MessageBase<TBundledOptions..., TOptions...>
{

};

}  // namespace details

}  // namespace comms


