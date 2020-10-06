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
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"

namespace comms
{

namespace details
{

template <typename...>
class FieldCastHelper
{
public:
    template <typename TFieldTo, typename TFieldFrom>
    static TFieldTo cast(const TFieldFrom& field)
    {
        return castInternal<TFieldTo>(field, Tag<TFieldFrom, TFieldTo>());
    }

private:    

    template <typename... TParams>
    using StaticCastTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using WriteReadTag = comms::details::tag::Tag2<>;

    template <typename TFieldFrom, typename TFieldTo>
    using Convertible = 
        std::is_convertible<typename TFieldFrom::ValueType, typename TFieldTo::ValueType>;

    template <typename TField>
    using IsIntegral =
        std::integral_constant<
            bool,
            std::is_enum<typename TField::ValueType>::value ||
                std::is_integral<typename TField::ValueType>::value
        >;

    template <typename TFieldFrom, typename TFieldTo>
    using UseStaticCast = 
        std::integral_constant<
            bool,
            Convertible<TFieldFrom, TFieldTo>::value || 
                (IsIntegral<TFieldFrom>::value && IsIntegral<TFieldTo>::value)
        >;

    template <typename TFieldFrom, typename TFieldTo>
    using Tag = 
        typename comms::util::LazyShallowConditional<
            UseStaticCast<TFieldFrom, TFieldTo>::value
        >::template Type<
            StaticCastTag,
            WriteReadTag
        >;

    template <typename TFieldTo, typename TFieldFrom, typename... TParams>
    static TFieldTo castInternal(const TFieldFrom& field, StaticCastTag<TParams...>)
    {
        TFieldTo result;
        result.value() = static_cast<typename TFieldTo::ValueType>(field.value()); 
        return result;
    }

    template <typename TFieldTo, typename TFieldFrom, typename... TParams>
    static TFieldTo castInternal(const TFieldFrom& field, WriteReadTag<TParams...>)
    {
        static_assert(TFieldFrom::minLength() == TFieldTo::maxLength(), "Casting between different fields of variable sizes is not supported.");

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


}  // namespace details

}  // namespace comms


