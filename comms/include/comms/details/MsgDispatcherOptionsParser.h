//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/options.h"

namespace comms
{

namespace details
{

template <typename... TOptions>
class MsgDispatcherOptionsParser;

template <>
class MsgDispatcherOptionsParser<>
{
public:
    static const bool HasForcedDispatch = false;
};

template <typename T, typename... TOptions>
class MsgDispatcherOptionsParser<comms::option::app::ForceDispatch<T>, TOptions...> :
        public MsgDispatcherOptionsParser<TOptions...>
{
public:
    static const bool HasForcedDispatch = true;
    using ForcedDispatch = T;
};


template <typename... TOptions>
class MsgDispatcherOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public MsgDispatcherOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class MsgDispatcherOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public MsgDispatcherOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace comms
