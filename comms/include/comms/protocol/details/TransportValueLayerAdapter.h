//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "TransportValueLayerOptionsParser.h"

namespace comms
{

namespace protocol
{

namespace details
{

template <typename TBase, typename... TOptions>
using TransportValueLayerAdapterT =
    typename TransportValueLayerOptionsParser<
        TOptions...
    >::template BuildPseudoBase<
        TBase
    >;

} // namespace details

} // namespace protocol

} // namespace comms
