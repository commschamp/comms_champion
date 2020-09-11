//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
class MsgSizeLayerOptionsParser;

template <>
class MsgSizeLayerOptionsParser<>
{
public:
    static constexpr bool HasExtendingClass = false;

    template <typename TLayer>
    using DefineExtendingClass = TLayer;
};

template <typename T, typename... TOptions>
class MsgSizeLayerOptionsParser<comms::option::def::ExtendingClass<T>, TOptions...> :
        public MsgSizeLayerOptionsParser<TOptions...>
{
public:
    static constexpr bool HasExtendingClass = true;
    using ExtendingClass = T;

    template <typename TLayer>
    using DefineExtendingClass = ExtendingClass;    
};

template <typename... TOptions>
class MsgSizeLayerOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public MsgSizeLayerOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class MsgSizeLayerOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public MsgSizeLayerOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace protocol

} // namespace comms
