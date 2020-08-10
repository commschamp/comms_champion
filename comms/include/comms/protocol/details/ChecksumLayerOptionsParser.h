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
class ChecksumLayerOptionsParser;

template <>
class ChecksumLayerOptionsParser<>
{
public:
    static const bool HasVerifyBeforeRead = false;
};

template <typename... TOptions>
class ChecksumLayerOptionsParser<comms::option::def::ChecksumLayerVerifyBeforeRead, TOptions...> :
        public ChecksumLayerOptionsParser<TOptions...>
{
public:
    static const bool HasVerifyBeforeRead = true;
};

template <typename... TOptions>
class ChecksumLayerOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public ChecksumLayerOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class ChecksumLayerOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public ChecksumLayerOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace protocol

} // namespace comms
