//
// Copyright 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/options.h"

namespace comms
{

namespace protocol
{

namespace details
{


template <typename... TOptions>
class SyncPrefixLayerOptionsParser;

template <>
class SyncPrefixLayerOptionsParser<>
{
public:
    static constexpr bool HasExtendingClass = false;

    template <typename TLayer>
    using DefineExtendingClass = TLayer;
};

template <typename T, typename... TOptions>
class SyncPrefixLayerOptionsParser<comms::option::def::ExtendingClass<T>, TOptions...> :
        public SyncPrefixLayerOptionsParser<TOptions...>
{
public:
    static constexpr bool HasExtendingClass = true;
    using ExtendingClass = T;

    template <typename TLayer>
    using DefineExtendingClass = ExtendingClass;    
};

template <typename... TOptions>
class SyncPrefixLayerOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public SyncPrefixLayerOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class SyncPrefixLayerOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public SyncPrefixLayerOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace protocol

} // namespace comms
