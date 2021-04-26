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

template <typename TBase>
class IgnoreInvalid : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    IgnoreInvalid() = default;

    explicit IgnoreInvalid(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit IgnoreInvalid(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    IgnoreInvalid(const IgnoreInvalid&) = default;
    IgnoreInvalid(IgnoreInvalid&&) = default;
    IgnoreInvalid& operator=(const IgnoreInvalid&) = default;
    IgnoreInvalid& operator=(IgnoreInvalid&&) = default;

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        BaseImpl tmp;
        auto es = tmp.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        if (tmp.valid()) {
            static_cast<BaseImpl&>(*this) = std::move(tmp);
        }

        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        BaseImpl tmp;
        tmp.readNoStatus(iter);

        if (tmp.valid()) {
            static_cast<BaseImpl&>(*this) = std::move(tmp);
        }
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

