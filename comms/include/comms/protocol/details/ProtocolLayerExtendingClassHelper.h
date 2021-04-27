//
// Copyright 2019 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace comms
{

namespace protocol
{

namespace details    
{

template <bool THasExtendingClass>
struct ProtocolLayerExtendingClassHelper;

template <>
struct ProtocolLayerExtendingClassHelper<false>
{
    template <typename TLayer, typename TParsedOptions>
    using Type = TLayer;
};

template <>
struct ProtocolLayerExtendingClassHelper<true>
{
    template <typename TLayer, typename TParsedOptions>
    using Type = typename TParsedOptions::ExtendingClass;
};

template <typename TLayer, typename TParsedOptions>
using ProtocolLayerExtendingClassT = 
    typename ProtocolLayerExtendingClassHelper<TParsedOptions::HasExtendingClass>::
        template Type<TLayer, TParsedOptions>;

} // namespace details

} // namespace protocol

} // namespace comms
