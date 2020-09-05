//
// Copyright 2013 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Replacement to some types from standard type_traits

#pragma once

namespace comms
{

namespace util
{

/// @brief Replacement to std::conditional
template <bool TCond>
struct Conditional
{
    template <typename TTrue, typename TFalse>
    using Type = TTrue;
};

template <>
struct Conditional<false>
{
    template <typename TTrue, typename TFalse>
    using Type = TFalse;
};

/// @brief Replacement to std::conditional
template <bool TCond, typename TTrue, typename TFalse>
using ConditionalT = typename Conditional<TCond>::template Type<TTrue, TFalse>;

template <bool TCond>
struct LazyShallowConditional
{
    template <template<typename...> class TTrue, template<typename...> class TFalse, typename... TParams>
    using Type = TTrue<TParams...>;
};

template <>
struct LazyShallowConditional<false>
{
    template <template<typename...> class TTrue, template<typename...> class TFalse, typename... TParams>
    using Type = TFalse<TParams...>;
};

template <bool TCond>
struct LazyDeepConditional
{
    template <template<typename...> class TTrue, template<typename...> class TFalse, typename... TParams>
    using Type = typename TTrue<>::template Type<TParams...>;
};

template <>
struct LazyDeepConditional<false>
{
    template <template<typename...> class TTrue, template<typename...> class TFalse, typename... TParams>
    using Type = typename TFalse<>::template Type<TParams...>;
};

template <bool TCond>
struct LazyShallowDeepConditional
{
    template <template<typename...> class TTrue, template<typename...> class TFalse, typename... TParams>
    using Type = TTrue<TParams...>;
};

template <>
struct LazyShallowDeepConditional<false>
{
    template <template<typename...> class TTrue, template<typename...> class TFalse, typename... TParams>
    using Type = typename TFalse<>::template Type<TParams...>;
};

template <typename...>
class TypeDeepWrap
{
public:
    template <typename T, typename...>
    using Type = T;
};

} // namespace util

} // namespace comms
