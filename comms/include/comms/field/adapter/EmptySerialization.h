//
// Copyright    2017 - 2020 (C). Alex Robenko. All rights reserved.
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
class EmptySerialization : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    EmptySerialization() = default;

    explicit EmptySerialization(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit EmptySerialization(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    EmptySerialization(const EmptySerialization&) = default;
    EmptySerialization(EmptySerialization&&) = default;
    EmptySerialization& operator=(const EmptySerialization&) = default;
    EmptySerialization& operator=(EmptySerialization&&) = default;

    static constexpr std::size_t length()
    {
        return 0U;
    }

    static constexpr std::size_t minLength()
    {
        return length();
    }

    static constexpr std::size_t maxLength()
    {
        return length();
    }

    template <typename TIter>
    static comms::ErrorStatus read(TIter&, std::size_t)
    {
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    static void readNoStatus(TIter&)
    {
    }

    static constexpr bool canWrite()
    {
        return true;
    }

    template <typename TIter>
    static comms::ErrorStatus write(TIter&, std::size_t)
    {
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    static void writeNoStatus(TIter&)
    {
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

