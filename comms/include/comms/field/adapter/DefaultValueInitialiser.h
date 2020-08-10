//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TInitialiser, typename TBase>
class DefaultValueInitialiser : public TBase
{
    using BaseImpl = TBase;
    using Initialiser = TInitialiser;
public:
    using ValueType = typename BaseImpl::ValueType;

    DefaultValueInitialiser()
    {
        Initialiser()(*this);
    }

    explicit DefaultValueInitialiser(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit DefaultValueInitialiser(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    DefaultValueInitialiser(const DefaultValueInitialiser&) = default;
    DefaultValueInitialiser(DefaultValueInitialiser&&) = default;
    DefaultValueInitialiser& operator=(const DefaultValueInitialiser&) = default;
    DefaultValueInitialiser& operator=(DefaultValueInitialiser&&) = default;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms



