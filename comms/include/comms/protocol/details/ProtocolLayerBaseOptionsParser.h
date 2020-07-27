//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
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
class ProtocolLayerBaseOptionsParser;

template <>
class ProtocolLayerBaseOptionsParser<>
{
public:
    static const bool HasForceReadUntilDataSplit = false;
    static const bool HasDisallowReadUntilDataSplit = false;
};

template <typename... TOptions>
class ProtocolLayerBaseOptionsParser<
    comms::option::def::ProtocolLayerForceReadUntilDataSplit, TOptions...> :
        public ProtocolLayerBaseOptionsParser<TOptions...>
{
public:
    static const bool HasForceReadUntilDataSplit = true;
};

template <typename... TOptions>
class ProtocolLayerBaseOptionsParser<
    comms::option::def::ProtocolLayerDisallowReadUntilDataSplit, TOptions...> :
        public ProtocolLayerBaseOptionsParser<TOptions...>
{
public:
    static const bool HasDisallowReadUntilDataSplit = true;
};

template <typename... TOptions>
class ProtocolLayerBaseOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public ProtocolLayerBaseOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class ProtocolLayerBaseOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public ProtocolLayerBaseOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace protocol

} // namespace comms
