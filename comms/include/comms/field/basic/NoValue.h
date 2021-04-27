//
// Copyright 2016 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>

#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TFieldBase>
class NoValue : public TFieldBase
{
    using BaseImpl = TFieldBase;
public:

    using ValueType = unsigned;
    using SerialisedType = ValueType;

    NoValue() = default;

    NoValue(const NoValue&) = default;
    NoValue(NoValue&&) = default;
    ~NoValue() noexcept = default;

    NoValue& operator=(const NoValue&) = default;
    NoValue& operator=(NoValue&&) = default;

    static ValueType& value()
    {
        static ValueType value = ValueType();
        return value;
    }

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

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return static_cast<SerialisedType>(val);
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return static_cast<ValueType>(val);
    }

    template <typename TIter>
    static ErrorStatus read(TIter& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    static void readNoStatus(TIter& iter)
    {
        static_cast<void>(iter);
    }

    template <typename TIter>
    static ErrorStatus write(TIter& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    static void writeNoStatus(TIter& iter)
    {
        static_cast<void>(iter);
    }
};

}  // namespace basic

}  // namespace field

}  // namespace comms


