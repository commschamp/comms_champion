//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
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

template <typename TValidator, typename TBase>
class CustomValidator : public TBase
{
    using BaseImpl = TBase;
    using Validator = TValidator;

public:

    using ValueType = typename BaseImpl::ValueType;

    CustomValidator() = default;

    explicit CustomValidator(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit CustomValidator(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    CustomValidator(const CustomValidator&) = default;
    CustomValidator(CustomValidator&&) = default;
    CustomValidator& operator=(const CustomValidator&) = default;
    CustomValidator& operator=(CustomValidator&&) = default;

    bool valid() const
    {
        return BaseImpl::valid() && (Validator()(*this));
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




