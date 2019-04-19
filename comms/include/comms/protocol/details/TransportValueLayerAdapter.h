//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
