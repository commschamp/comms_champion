//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

#include <limits>
#include <algorithm>

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class SequenceLengthForcing : public TBase
{
    using BaseImpl = TBase;
public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceLengthForcing() = default;

    explicit SequenceLengthForcing(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceLengthForcing(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceLengthForcing(const SequenceLengthForcing&) = default;
    SequenceLengthForcing(SequenceLengthForcing&&) = default;
    SequenceLengthForcing& operator=(const SequenceLengthForcing&) = default;
    SequenceLengthForcing& operator=(SequenceLengthForcing&&) = default;

    void forceReadLength(std::size_t val)
    {
        COMMS_ASSERT(val != Cleared);
        forced_ = val;
    }

    void clearReadLengthForcing()
    {
        forced_ = Cleared;
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        if (forced_ == Cleared) {
            return BaseImpl::read(iter, len);
        }

        if (len < forced_) {
            return comms::ErrorStatus::NotEnoughData;
        }

        return BaseImpl::read(iter, forced_);
    }

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len)
    {
        if (forced_ == Cleared) {
            return BaseImpl::read(iter, len);
        }

        if (len < forced_) {
            return comms::ErrorStatus::NotEnoughData;
        }

        return BaseImpl::readN(count, iter, forced_);
    }

    static constexpr bool hasReadNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    void readNoStatusN(std::size_t count, TIter& iter) = delete;


private:
    static const std::size_t Cleared = std::numeric_limits<std::size_t>::max();
    std::size_t forced_ = Cleared;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


