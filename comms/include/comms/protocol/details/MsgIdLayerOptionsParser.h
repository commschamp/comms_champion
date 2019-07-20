//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
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
#include "comms/options.h"

namespace comms
{

namespace protocol
{

namespace details
{


template <typename... TOptions>
class MsgIdLayerOptionsParser;

template <>
class MsgIdLayerOptionsParser<>
{
public:
    static const bool HasExtendingClass = false;
    using FactoryOptions = std::tuple<>;
};

template <typename T, typename... TOptions>
class MsgIdLayerOptionsParser<comms::option::def::ExtendingClass<T>, TOptions...> :
        public MsgIdLayerOptionsParser<TOptions...>
{
public:
    static const bool HasExtendingClass = true;
    using ExtendingClass = T;
};

template <typename... TOptions>
class MsgIdLayerOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public MsgIdLayerOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class MsgIdLayerOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public MsgIdLayerOptionsParser<TBundledOptions..., TOptions...>
{
};

template <typename T, typename... TOptions>
class MsgIdLayerOptionsParser<T, TOptions...> : public MsgIdLayerOptionsParser<TOptions...>
{
    using BaseImpl = MsgIdLayerOptionsParser<TOptions...>;
public:    
    using FactoryOptions = 
        typename std::decay<
            decltype(
                std::tuple_cat(
                    std::declval<std::tuple<T> >(),
                    std::declval<typename BaseImpl::FactoryOptions>()
                )
            )
        >::type;
};

} // namespace details

} // namespace protocol

} // namespace comms
