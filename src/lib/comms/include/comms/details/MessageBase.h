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
};

template <typename T, typename... TOptions>
class MessageBase<comms::option::SetMsgIdType<T>, TOptions...>
                                            : public MessageBase<TOptions...>
{
protected:
    typedef typename comms::option::SetMsgIdType<T>::MsgIdType MsgIdType;
};

template <typename TEndian, typename... TOptions>
class MessageBase<comms::option::UseEndian<TEndian>, TOptions...>
                                            : public MessageBase<TOptions...>
{
protected:
    typedef typename comms::option::UseEndian<TEndian>::Endian Endian;
};


template <typename TIter, typename... TOptions>
class MessageBase<comms::option::SetReadIterator<TIter>, TOptions...>
                                            : public MessageBase<TOptions...>
{
protected:
    typedef typename comms::option::SetReadIterator<TIter>::ReadIterator ReadIterator;
};

template <typename TIter, typename... TOptions>
class MessageBase<comms::option::SetWriteIterator<TIter>, TOptions...>
                                            : public MessageBase<TOptions...>
{
protected:
    typedef typename comms::option::SetWriteIterator<TIter>::WriteIterator WriteIterator;
};

template <typename T, typename... TOptions>
class MessageBase<comms::option::SetHandler<T>, TOptions...>
                                            : public MessageBase<TOptions...>
{
protected:
    typedef typename comms::option::SetHandler<T>::Handler Handler;
};

template <typename... TBundledOptions, typename... TOptions>
class MessageBase<std::tuple<TBundledOptions...>, TOptions...>
                            : public MessageBase<TBundledOptions..., TOptions...>
{

};

}  // namespace details

}  // namespace comms


