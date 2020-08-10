//
// Copyright 2016 - 2020 (C). Alex Robenko. All rights reserved.
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

template <typename T, typename TBase>
class CustomValueReader : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    CustomValueReader() = default;

    explicit CustomValueReader(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit CustomValueReader(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    CustomValueReader(const CustomValueReader&) = default;
    CustomValueReader(CustomValueReader&&) = default;
    CustomValueReader& operator=(const CustomValueReader&) = default;
    CustomValueReader& operator=(CustomValueReader&&) = default;


    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t size)
    {
        return T()(static_cast<BaseImpl&>(*this), iter, size);
    }

    template <std::size_t TFromIdx, typename TIter>
    comms::ErrorStatus readFrom(TIter& iter, std::size_t size) = delete;

    template <std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus readUntil(TIter& iter, std::size_t size) = delete;

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus readFromUntil(TIter& iter, std::size_t size) = delete;

    static constexpr bool hasReadNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <std::size_t TFromIdx, typename TIter>
    void readFromNoStatus(TIter& iter) = delete;

    template <std::size_t TUntilIdx, typename TIter>
    void readUntilNoStatus(TIter& iter) = delete;

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void readFromUntilNoStatus(TIter& iter) = delete;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

