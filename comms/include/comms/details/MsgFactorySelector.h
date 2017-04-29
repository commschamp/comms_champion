//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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

#include <type_traits>
#include "MsgFactoryDirect.h"
#include "MsgFactoryUniq.h"
#include "MsgFactoryGeneric.h"

namespace comms
{

namespace details
{

template <typename TAllMessages>
using MsgFactoryLastMessageType =
    typename std::tuple_element<std::tuple_size<TAllMessages>::value - 1, TAllMessages>::type;

template <typename TAllMessages>
constexpr bool msgFactoryCanDirectAccess()
{
    return static_cast<std::size_t>(MsgFactoryLastMessageType<TAllMessages>::ImplOptions::MsgId) < (std::tuple_size<TAllMessages>::value + 10);
}


template <bool TStrongSorted>
struct MsgFactoryStaticNumIdSelector;

template <>
struct MsgFactoryStaticNumIdSelector<true>
{
    template <typename TMsgBase, typename TAllMessages, typename... TOptions>
    using Type =
        typename std::conditional<
            msgFactoryCanDirectAccess<TAllMessages>(),
            MsgFactoryDirect<TMsgBase, TAllMessages, TOptions...>,
            MsgFactoryUniq<TMsgBase, TAllMessages, TOptions...>
        >::type;
};

template <>
struct MsgFactoryStaticNumIdSelector<false>
{
    template <typename TMsgBase, typename TAllMessages, typename... TOptions>
    using Type = MsgFactoryGeneric<TMsgBase, TAllMessages, TOptions...>;
};

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
using MsgFactoryStaticNumIdSelectorT =
    typename MsgFactoryStaticNumIdSelector<msgFactoryAreAllStrongSorted<TAllMessages>()>::template
            Type<TMsgBase, TAllMessages, TOptions...>;

template <typename TMsgBase, typename TAllMessages, typename... TOptions>
struct MsgFactorySelector
{
    using Type =
        typename std::conditional<
            msgFactoryAllHaveStaticNumId<TAllMessages>(),
            MsgFactoryStaticNumIdSelectorT<TMsgBase, TAllMessages, TOptions...>,
            MsgFactoryGeneric<TMsgBase, TAllMessages, TOptions...>
        >::type;
};

} // namespace details

} // namespace comms
