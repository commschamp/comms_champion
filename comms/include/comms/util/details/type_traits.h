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

// Forward declarations
template <typename...>
struct IsAnyOf;

template <bool TCond>
struct Conditional;

namespace details
{
    
template<bool TEmpty>
struct PredicateLoop
{
    template <
        template<typename...> class TAlg, 
        template<typename...> class TPred, 
        typename... TRest
    > 
    using Type = typename TAlg<>::template Type<TPred, TRest...>;
};

template<>
struct PredicateLoop<true>
{
    template <
        template<typename...> class TAlg, 
        template<typename...> class TPred,
        typename...
    > 
    using Type = std::false_type;
};

template <bool TEmpty>
struct IsAnyOfImpl
{
    template <template<typename...> class TPred, typename T, typename... TRest>
    using Type = 
        typename Conditional<
            TPred<>::template Type<T>::value
        >::template Type<
            std::true_type,
            typename PredicateLoop<0U == sizeof...(TRest)>::template Type<comms::util::IsAnyOf, TPred, TRest...>
        >;
};

template <>
struct IsAnyOfImpl<true>
{
    template <template<typename...> class TPred, typename... TRest>
    using Type = std::false_type;
};

} // namespace details

} // namespace util

} // namespace comms
