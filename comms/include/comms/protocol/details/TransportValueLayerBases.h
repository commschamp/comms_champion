//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
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

} // namespace details

} // namespace protocol

} // namespace comms
