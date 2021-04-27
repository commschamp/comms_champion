//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class InvalidByDefault : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    InvalidByDefault() = default;

    explicit InvalidByDefault(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit InvalidByDefault(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    InvalidByDefault(const InvalidByDefault&) = default;
    InvalidByDefault(InvalidByDefault&&) = default;
    InvalidByDefault& operator=(const InvalidByDefault&) = default;
    InvalidByDefault& operator=(InvalidByDefault&&) = default;

    static constexpr bool valid()
    {
        return false;
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

