//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/options.h"
#include "TransportValueLayerBases.h"
#include "ProtocolLayerDetails.h"

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
    static constexpr bool HasExtendingClass = false;

    template <typename TBase>
    using BuildPseudoBase = TBase;

    template <typename TLayer>
    using DefineExtendingClass = TLayer;

    template <typename TNextLayer>
    using ForceReadUntilDataSplitIfNeeded = 
        typename comms::util::Conditional<
            std::is_void<typename ProtocolLayerMsgPtr<TNextLayer>::Type>::value
        >::template Type <
            comms::option::app::EmptyOption,
            comms::option::def::ProtocolLayerForceReadUntilDataSplit
            // If the MsgPtr is not defined, then the MsgIdLayer is 
            // probably an outer layer, as the result the message 
            // object is properly allocated when transport value
            // read operation is reached.
        >;
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

template <typename T, typename... TOptions>
class TransportValueLayerOptionsParser<comms::option::def::ExtendingClass<T>, TOptions...> :
        public TransportValueLayerOptionsParser<TOptions...>
{
public:
    static constexpr bool HasExtendingClass = true;
    using ExtendingClass = T;

    template <typename TLayer>
    using DefineExtendingClass = ExtendingClass;    
};

template <typename... TOptions>
class TransportValueLayerOptionsParser<
    comms::option::def::ProtocolLayerSuppressReadUntilDataSplitForcing, 
    TOptions...> :
        public TransportValueLayerOptionsParser<TOptions...>
{
public:
    template <typename TNextLayer>
    using ForceReadUntilDataSplitIfNeeded = comms::option::app::EmptyOption;
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
