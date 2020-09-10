//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <limits>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class SequenceSizeForcing : public TBase
{
    using BaseImpl = TBase;
public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceSizeForcing() = default;

    explicit SequenceSizeForcing(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceSizeForcing(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceSizeForcing(const SequenceSizeForcing&) = default;
    SequenceSizeForcing(SequenceSizeForcing&&) = default;
    SequenceSizeForcing& operator=(const SequenceSizeForcing&) = default;
    SequenceSizeForcing& operator=(SequenceSizeForcing&&) = default;

    void forceReadElemCount(std::size_t val)
    {
        COMMS_ASSERT(val != Cleared);
        forced_ = val;
    }

    void clearReadElemCount()
    {
        forced_ = Cleared;
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        if (forced_ == Cleared) {
            return BaseImpl::read(iter, len);
        }

        return BaseImpl::readN(forced_, iter, len);
    }

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len) = delete;


    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        if (forced_ == Cleared) {
            BaseImpl::readNoStatus(iter);
            return;
        }

        BaseImpl::readNoStatusN(forced_, iter);
    }

    template <typename TIter>
    void readNoStatusN(std::size_t count, TIter& iter) = delete;


private:
    static const std::size_t Cleared = std::numeric_limits<std::size_t>::max();
    std::size_t forced_ = Cleared;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


