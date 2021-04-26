//
// Copyright 2013 - 2021 (C). Alex Robenko. All rights reserved.
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
    using Type = std::integral_constant<bool, T::isVersionDependent()>;
};

template <typename...>
class FieldCheckNonDefaultRefresh
{
public:
    template <typename T>
    using Type = std::integral_constant<bool, T::hasNonDefaultRefresh()>;
};

template <typename...>
class FieldCheckVarLength
{
public:
    template <typename T>
    using Type = std::integral_constant<bool, T::minLength() != T::maxLength()>;
};

template <typename...>
class FieldCheckReadNoStatus
{
public:
    template <typename T>
    using Type = std::integral_constant<bool, T::hasReadNoStatus()>;
};

template <typename...>
class FieldCheckWriteNoStatus
{
public:
    template <typename T>
    using Type = std::integral_constant<bool, T::hasWriteNoStatus()>;
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
class AliasType
{
public:
    template <typename T, typename...>
    using Type = T;    
};

template <typename...>
struct LogicalOrBinaryOp
{
    // TFirst and TSecond are either std::true_type || std::false_type
    template <typename TFirst, typename TSecond>
    using Type = std::integral_constant<bool, (TFirst::value || TSecond::value)>;
};

template <typename...>
struct LogicalAndBinaryOp
{
    // TFirst and TSecond are either std::true_type || std::false_type
    template <typename TFirst, typename TSecond>
    using Type = std::integral_constant<bool, (TFirst::value && TSecond::value)>;
};

template <typename...>
struct IntMaxBinaryOp
{
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
struct IntMinBinaryOp
{
    template <typename TFirst, typename TSecond>
    using Type = 
        typename Conditional<
            (TFirst::value <= TSecond::value)
        >::template Type<
            TFirst,
            TSecond
        >;
};

template <typename...>
struct IntSumBinaryOp
{
    template <typename TFirst, typename TSecond>
    using Type = 
        std::integral_constant<typename TFirst::value_type, TFirst::value + TSecond::value>;
};

template <typename...>
class FieldMinLengthIntType
{
public:
    template <typename T>
    using Type = std::integral_constant<std::size_t, T::minLength()>;
};

template <typename...>
class FieldMaxLengthIntType
{
public:
    template <typename T>
    using Type = std::integral_constant<std::size_t, T::maxLength()>;
};

template <typename...>
class FieldBitLengthIntType
{
public:
    template <typename T>
    using Type = 
        typename details::FieldBitLengthIntTypeImpl<
            T::ParsedOptions::HasFixedBitLengthLimit
        >::template Type<T>;
};

template <typename...>
class AlignmentIntType
{
public:
    template <typename T>
    using Type = std::integral_constant<std::size_t, alignof(T)>;
};

template <typename...>
class SizeIntType
{
public:
    template <typename T>
    using Type = std::integral_constant<std::size_t, sizeof(T)>;
};

template <typename...>
class MessageCheckHasStaticId
{
public:
    template <typename TMessage>
    using Type = std::integral_constant<bool, TMessage::ImplOptions::HasStaticMsgId>;
};

template <typename...>
struct AccumulateFromUntil
{
    template <
        std::size_t TFrom,
        std::size_t TUntil,
        template<typename...> class TTransformOp,
        template<typename...> class TBinaryOp, 
        typename TStart,
        typename... TRest>
    using Type = 
        typename details::AccumulateFromUntilImpl<
            (0 == sizeof...(TRest)) || (TUntil == 0U)
        >::template Type<
            TFrom,
            TUntil,
            TTransformOp,
            TBinaryOp,
            TStart,
            TRest...>;
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
        typename AccumulateFromUntil<>::template Type<
            0,
            sizeof...(TRest),
            TTransformOp,
            TBinaryOp,
            TStart,
            TRest...
        >;
};

} // namespace util

} // namespace comms
