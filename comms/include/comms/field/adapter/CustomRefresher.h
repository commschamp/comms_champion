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

template <typename TRefresher, typename TBase>
class CustomRefresher : public TBase
{
    using BaseImpl = TBase;
    using Refresher = TRefresher;

public:

    using ValueType = typename BaseImpl::ValueType;

    CustomRefresher() = default;

    explicit CustomRefresher(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit CustomRefresher(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    CustomRefresher(const CustomRefresher&) = default;
    CustomRefresher(CustomRefresher&&) = default;
    CustomRefresher& operator=(const CustomRefresher&) = default;
    CustomRefresher& operator=(CustomRefresher&&) = default;

    bool refresh()
    {
        return (Refresher()(*this));
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return true;
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




