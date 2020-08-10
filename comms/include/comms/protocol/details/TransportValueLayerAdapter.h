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

template <typename TBase>
class TransportValueLayerPseudoBase : public TBase
{
    using BaseImpl = TBase;
    using FieldImpl = typename BaseImpl::Field;
public:
    FieldImpl& pseudoField()
    {
        return pseudoField_;
    }

    const FieldImpl& pseudoField() const
    {
        return pseudoField_;
    }

private:
    FieldImpl pseudoField_;
};

template <bool THasPseudoValue>
struct TransportValueLayerProcessPseudoBase;

template <>
struct TransportValueLayerProcessPseudoBase<true>
{
    template <typename TBase>
    using Type = TransportValueLayerPseudoBase<TBase>;
};

template <>
struct TransportValueLayerProcessPseudoBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using TransportValueLayerPseudoBaseT =
    typename TransportValueLayerProcessPseudoBase<TOpt::HasPseudoValue>::template Type<TBase>;

template <typename TBase, typename... TOptions>
class TransportValueLayerAdapter
{
    using Options = TransportValueLayerOptionsParser<TOptions...>;
    using PseudoBase = TransportValueLayerPseudoBaseT<TBase, Options>;
public:
    using Type = PseudoBase;
};

template <typename TBase, typename... TOptions>
using TransportValueLayerAdapterT =
    typename TransportValueLayerAdapter<TBase, TOptions...>::Type;

} // namespace details

} // namespace protocol

} // namespace comms
