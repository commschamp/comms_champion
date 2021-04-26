//
// Copyright 2014 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/protocol/ProtocolLayerBase.h"

namespace comms
{

namespace protocol
{

namespace details
{

template <
    typename TField,
    typename TNextLayer,
    typename TDerived,
    typename...  TOptions>
using ProtocolLayerBase = 
    comms::protocol::ProtocolLayerBase<TField, TNextLayer, TDerived, TOptions...>;

} // namespace details

} // namespace protocol

} // namespace comms