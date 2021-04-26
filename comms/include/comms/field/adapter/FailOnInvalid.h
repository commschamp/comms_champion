//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <comms::ErrorStatus TStatus, typename TBase>
class FailOnInvalid : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    FailOnInvalid() = default;

    explicit FailOnInvalid(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit FailOnInvalid(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    FailOnInvalid(const FailOnInvalid&) = default;
    FailOnInvalid(FailOnInvalid&&) = default;
    FailOnInvalid& operator=(const FailOnInvalid&) = default;
    FailOnInvalid& operator=(FailOnInvalid&&) = default;

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        BaseImpl tmp;
        auto es = tmp.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (!tmp.valid()) {
            return TStatus;
        }

        static_cast<BaseImpl&>(*this) = std::move(tmp);
        return comms::ErrorStatus::Success;
    }

    static constexpr bool hasReadNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

