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
#include "comms/util/details/type_traits.h"

namespace comms
{

namespace util
{

template <typename...>
struct EmptyStruct {};

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

template <typename...>
class FieldCheckVersionDependent
{
public:
    template <typename T>
    using Type = 
        typename 
        Conditional<
            T::isVersionDependent()
        >::template Type<
            std::true_type,
            std::false_type
        >;
};

template <bool TIsField>
class EagerFieldCheckVersionDependent
{
public:
    template <typename T>
    using Type = 
        typename 
        Conditional<
            T::isVersionDependent()
        >::template Type<
            std::true_type,
            std::false_type
        >;
};

template <>
class EagerFieldCheckVersionDependent<false>
{
public:
    template <typename T>
    using Type = std::false_type;
};

template <typename...>
class FieldCheckNonDefaultRefresh
{
public:
    template <typename T>
    using Type = 
        typename 
        Conditional<
            T::hasNonDefaultRefresh()
        >::template Type<
            std::true_type,
            std::false_type
        >;
};

template <bool TIsField>
class EagerFieldCheckNonDefaultRefresh
{
public:
    template <typename T>
    using Type = 
        typename 
        Conditional<
            T::hasNonDefaultRefresh()
        >::template Type<
            std::true_type,
            std::false_type
        >;
};

template <>
class EagerFieldCheckNonDefaultRefresh<false>
{
public:
    template <typename T>
    using Type = std::false_type;
};


template <typename...>
class FieldCheckVarLength
{
public:
    template <typename T>
    using Type = 
        typename 
        Conditional<
            T::minLength() != T::maxLength()
        >::template Type<
            std::true_type,
            std::false_type
        >;
};

template <bool TIsField>
class EagerFieldCheckVarLength
{
public:
    template <typename T>
    using Type = 
        typename 
        Conditional<
            T::minLength() != T::maxLength()
        >::template Type<
            std::true_type,
            std::false_type
        >;
};

template <>
class EagerFieldCheckVarLength<false>
{
public:
    template <typename T>
    using Type = std::false_type;
};


template <typename...>
class TrueType
{
public:
    template <typename...>
    using Type = std::true_type;    
};


template <typename...>
class FalseType
{
public:
    template <typename...>
    using Type = std::false_type;    
};

template <typename...>
struct LogicalOrBinaryOp
{
    // TFirst and TSecond are either std::true_type || std::false_type
    template <typename TFirst, typename TSecond>
    using Type = 
        typename Conditional<
            (TFirst::value || TSecond::value)
        >::template Type<
            std::true_type,
            std::false_type
        >;
};

template <typename...>
struct IntMaxBinaryOp
{
    // TFirst and TSecond are either std::true_type || std::false_type
    template <typename TFirst, typename TSecond>
    using Type = 
        typename Conditional<
            (TFirst::value >= TSecond::value)
        >::template Type<
            TFirst,
            TSecond
        >;
};

template <typename...>
class FieldMaxLengthIntType
{
public:
    template <typename T>
    using Type = std::integral_constant<std::size_t, T::maxLength()>;
};

template <typename...>
struct Accumulate
{
    template <
        template<typename...> class TTransformOp,
        template<typename...> class TBinaryOp, 
        typename TStart,
        typename... TRest>
    using Type = 
        typename details::AccumulateImpl<0 == sizeof...(TRest)>::template Type<
            TTransformOp,
            TBinaryOp,
            TStart,
            TRest...>;
};

} // namespace util

} // namespace comms
