//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>
#include <iterator>
#include <type_traits>

#include "comms/ErrorStatus.h"

namespace comms
{

namespace details
{

template <typename TFieldTo, typename TFieldFrom, bool TFixedLength>
struct FieldCastNonEqualHelper;

template <typename TFieldTo, typename TFieldFrom>
struct FieldCastNonEqualHelper<TFieldTo, TFieldFrom, true>
{
    static TFieldTo cast(const TFieldFrom& field)
    {
        static const auto MaxBufSize = TFieldFrom::maxLength();
        std::uint8_t buf[MaxBufSize] = {0};
        auto* writeIter = &buf[0];
        auto es = field.write(writeIter, MaxBufSize);
        COMMS_ASSERT(es == comms::ErrorStatus::Success);
        if (es != comms::ErrorStatus::Success) {
            return TFieldTo();
        }

        auto len = static_cast<std::size_t>(std::distance(&buf[0], writeIter));
        COMMS_ASSERT(len <= MaxBufSize);

        TFieldTo result;
        const auto* readIter = &buf[0];
        es = result.read(readIter, len);
        static_cast<void>(es);
        COMMS_ASSERT(es == comms::ErrorStatus::Success);
        return result;
    }
};

template <typename TFieldTo, typename TFieldFrom>
struct FieldCastNonEqualHelper<TFieldTo, TFieldFrom, false>
{
    static TFieldTo cast(const TFieldFrom& field)
    {
        static_cast<void>(field);
        COMMS_ASSERT(!"Casting between different fields of variable sizes is not supported.");
        return TFieldTo();
    }
};

template <typename TFieldTo, typename TFieldFrom, bool TSameValue>
struct FieldCastHelper;

template <typename TFieldTo, typename TFieldFrom>
struct FieldCastHelper<TFieldTo, TFieldFrom, true>
{
    static TFieldTo cast(const TFieldFrom& field)
    {
        TFieldTo result;
        result.value() = static_cast<typename TFieldTo::ValueType>(field.value()); 
        return result;
    }
};

template <typename TFieldTo, typename TFieldFrom>
struct FieldCastHelper<TFieldTo, TFieldFrom, false>
{
    static TFieldTo cast(const TFieldFrom& field)
    {
        static const bool SameValue = TFieldFrom::minLength() == TFieldTo::maxLength();
        return FieldCastNonEqualHelper<TFieldTo, TFieldFrom, SameValue>::cast(field);
    }
};



}  // namespace details

}  // namespace comms


