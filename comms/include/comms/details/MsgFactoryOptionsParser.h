//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/options.h"

namespace comms
{

namespace details
{

template <typename... TOptions>
class MsgFactoryOptionsParser;

template <>
class MsgFactoryOptionsParser<>
{
public:
    static const bool HasInPlaceAllocation = false;
    static const bool HasSupportGenericMessage = false;
    static const bool HasForcedDispatch = false;
};

template <typename... TOptions>
class MsgFactoryOptionsParser<comms::option::app::InPlaceAllocation, TOptions...> :
        public MsgFactoryOptionsParser<TOptions...>
{
public:
    static const bool HasInPlaceAllocation = true;
};

template <typename TMsg, typename... TOptions>
class MsgFactoryOptionsParser<comms::option::app::SupportGenericMessage<TMsg>, TOptions...> :
        public MsgFactoryOptionsParser<TOptions...>
{
public:
    static const bool HasSupportGenericMessage = true;
    using GenericMessage = TMsg;
};

template <typename T, typename... TOptions>
class MsgFactoryOptionsParser<comms::option::app::ForceDispatch<T>, TOptions...> :
        public MsgFactoryOptionsParser<TOptions...>
{
public:
    static const bool HasForcedDispatch = true;
    using ForcedDispatch = T;
};


template <typename... TOptions>
class MsgFactoryOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public MsgFactoryOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class MsgFactoryOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public MsgFactoryOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace comms
