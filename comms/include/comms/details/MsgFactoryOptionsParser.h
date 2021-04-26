//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <tuple>

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
    static constexpr bool HasInPlaceAllocation = false;
    static constexpr bool HasSupportGenericMessage = false;
    static constexpr bool HasForcedDispatch = false;

    using GenericMessage = void;

    template <typename TAll>
    using AllMessages = TAll;
};

template <typename... TOptions>
class MsgFactoryOptionsParser<comms::option::app::InPlaceAllocation, TOptions...> :
        public MsgFactoryOptionsParser<TOptions...>
{
public:
    static constexpr bool HasInPlaceAllocation = true;
};

template <typename TMsg, typename... TOptions>
class MsgFactoryOptionsParser<comms::option::app::SupportGenericMessage<TMsg>, TOptions...> :
        public MsgFactoryOptionsParser<TOptions...>
{
public:
    static constexpr bool HasSupportGenericMessage = true;
    using GenericMessage = TMsg;

    template <typename TAll>
    using AllMessages = 
        typename std::decay<
            decltype(
                std::tuple_cat(
                    std::declval<TAll>(),
                    std::declval<std::tuple<GenericMessage> >()
                )
            )
        >::type;
};

template <typename T, typename... TOptions>
class MsgFactoryOptionsParser<comms::option::app::ForceDispatch<T>, TOptions...> :
        public MsgFactoryOptionsParser<TOptions...>
{
public:
    static constexpr bool HasForcedDispatch = true;
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
