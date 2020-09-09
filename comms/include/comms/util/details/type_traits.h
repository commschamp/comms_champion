//
// Copyright 2013 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Replacement to some types from standard type_traits

#pragma once

#include <type_traits>

namespace comms
{

namespace util
{

template <typename...>
struct Accumulate;

namespace details
{

template<bool TEmpty>
struct AccumulateLoop
{
    template <
        template<typename...> class TAlg, 
        template<typename...> class TTransformOp, 
        template<typename...> class TBinaryOp, 
        typename TStart, 
        typename...
    > 
    using Type = TStart;
};

template<>
struct AccumulateLoop<true>
{
    template <
        template<typename...> class TAlg, 
        template<typename...> class TTransformOp, 
        template<typename...> class TBinaryOp, 
        typename TStart, 
        typename... TRest
    > 
    using Type = std::false_type;
};

template <bool TEmpty>
struct AccumulateImpl
{
    template <
        template<typename...> class TTransformOp,
        template<typename...> class TBinaryOp, 
        typename TStart,
        typename T, 
        typename... TRest>
    using Type = 
        typename AccumulateLoop<0U == sizeof...(TRest)>::template Type<
            comms::util::Accumulate, 
            TTransformOp, 
            TBinaryOp, 
            typename TBinaryOp<>::template Type<
                TStart, 
                typename TTransformOp<>::template Type<T>
            >,
            TRest...>;
};

template <>
struct AccumulateImpl<true>
{
    template <
        template<typename...> class TTransformOp,
        template<typename...> class TBinaryOp, 
        typename TStart,
        typename...>
    using Type = TStart;
};

} // namespace details

} // namespace util

} // namespace comms
