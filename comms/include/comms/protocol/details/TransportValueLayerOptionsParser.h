//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/options.h"
#include "TransportValueLayerBases.h"

namespace comms
{

namespace protocol
{

namespace details
{


template <typename... TOptions>
class TransportValueLayerOptionsParser;

template <>
class TransportValueLayerOptionsParser<>
{
public:
    static const bool HasPseudoValue = false;

    template <typename TBase>
    using BuildPseudoBase = TBase;
};

template <typename... TOptions>
class TransportValueLayerOptionsParser<comms::option::def::PseudoValue, TOptions...> :
        public TransportValueLayerOptionsParser<TOptions...>
{
public:
    static const bool HasPseudoValue = true;

    template <typename TBase>
    using BuildPseudoBase = TransportValueLayerPseudoBase<TBase>;    
};

template <typename... TOptions>
class TransportValueLayerOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public TransportValueLayerOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class TransportValueLayerOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public TransportValueLayerOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace protocol

} // namespace comms
