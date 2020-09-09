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

template <typename...>
struct AccumulateFromUntil;

template <bool TCond>
struct Conditional;

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
        typename... TRest
    > 
    using Type = typename TAlg<>::template Type<TTransformOp, TBinaryOp, TStart, TRest...>;
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
    using Type = TStart;
};

template<bool TEmpty>
struct AccumulateFromUntilLoop
{
    template <
        std::size_t TFrom,
        std::size_t TUntil,
        template<typename...> class TAlg, 
        template<typename...> class TTransformOp, 
        template<typename...> class TBinaryOp, 
        typename TStart, 
        typename... TRest
    > 
    using Type = typename TAlg<>::template Type<TFrom, TUntil, TTransformOp, TBinaryOp, TStart, TRest...>;
};

template<>
struct AccumulateFromUntilLoop<true>
{
    template <
        std::size_t TFrom,
        std::size_t TUntil,    
        template<typename...> class TAlg, 
        template<typename...> class TTransformOp, 
        template<typename...> class TBinaryOp, 
        typename TStart, 
        typename... TRest
    > 
    using Type = TStart;
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

template <bool TEmpty>
struct AccumulateFromUntilImpl
{
    template <
        std::size_t TFrom,
        std::size_t TUntil,
        template<typename...> class TTransformOp,
        template<typename...> class TBinaryOp, 
        typename TStart,
        typename T, 
        typename... TRest>
    using Type = 
        typename Conditional<
            0U < TFrom
        >::template Type<
            typename AccumulateFromUntilImpl<
                0U == sizeof...(TRest)
            >::template Type<
                TFrom - 1U, 
                TUntil - 1U, 
                TTransformOp,
                TBinaryOp,
                TStart,
                TRest...
            >,
            typename AccumulateFromUntilLoop<
                (0U == sizeof...(TRest)) || (TUntil <= sizeof...(TRest))
            >::template Type<
                0U,
                TUntil - 1,
                comms::util::AccumulateFromUntil, 
                TTransformOp, 
                TBinaryOp, 
                typename TBinaryOp<>::template Type<
                    TStart, 
                    typename TTransformOp<>::template Type<T>
                >,
                TRest...>
            >;
};

template <>
struct AccumulateFromUntilImpl<true>
{
    template <
        std::size_t TFrom,
        std::size_t TUntil,    
        template<typename...> class TTransformOp,
        template<typename...> class TBinaryOp, 
        typename TStart,
        typename...>
    using Type = TStart;
};

} // namespace details

} // namespace util

} // namespace comms
