//
// Copyright 2013 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <tuple>
#include <utility>
#include <type_traits>

#include "comms/CompileControl.h"
#include "comms/util/type_traits.h"
#include "comms/Assert.h"
#include "AlignedUnion.h"

namespace comms
{

namespace util
{

/// @brief Check whether provided type is a variant of
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
/// @tparam TType Type to check.
template <typename TType>
struct IsTuple
{
    /// @brief By default Value has value false. Will be true for any
    /// variant of <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
    static constexpr bool Value = false;
};

/// @cond SKIP_DOC
template <typename... TArgs>
struct IsTuple<std::tuple<TArgs...> >
{
    static constexpr bool Value = true;
};
/// @endcond

/// @brief Check whether provided type is a variant of
///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>.
/// @tparam TType Type to check.
template <typename TType>
constexpr bool isTuple()
{
    return IsTuple<TType>::Value;
}

//----------------------------------------

namespace details
{

template <bool THasElems, typename...>
struct TupleElementHelper
{
    template <int TIdx, typename TFirst, typename... TRest>
    using Type = 
        typename comms::util::Conditional<
            TIdx == 0
        >::template Type<
            TFirst,
            typename TupleElementHelper<(0U < sizeof...(TRest))>::template Type<TIdx - 1, TRest...>
        >;
};

template <typename... TParams>
struct TupleElementHelper<false, TParams...>
{
    template <int Idx, typename...>
    struct Type
    {
        static_assert(Idx < 0, "Index exceeds size of tuple");
    };
};    

} // namespace details

template <typename TTuple>
struct TupleElement;

template <typename... TTypes>
struct TupleElement<std::tuple<TTypes...> >
{
    template <std::size_t TIdx>
    using Type = 
        typename details::TupleElementHelper<(0U < sizeof...(TTypes))>::template Type<(int)TIdx, TTypes...>;
};

//----------------------------------------

/// @brief Check whether TType type is included in the tuple TTuple
/// @tparam TType Type to check
/// @tparam TTuple Tuple
/// @pre @code IsTuple<TTuple>::Value == true @endcode
template <typename TType, typename TTuple>
class IsInTuple
{
    static_assert(IsTuple<TTuple>::Value, "TTuple must be std::tuple");
public:

    /// @brief By default the value is false, will be set to true if TType
    ///     is found in TTuple.
    static constexpr bool Value = false;
};

/// @cond SKIP_DOC

#if COMMS_IS_MSVC_2017_OR_BELOW

template <typename TType, typename TFirst, typename... TRest>
class IsInTuple<TType, std::tuple<TFirst, TRest...> >
{
public:
    static constexpr bool Value =
        std::is_same<TType, TFirst>::value ||
        IsInTuple<TType, std::tuple<TRest...> >::Value;
};

template <typename TType>
class IsInTuple<TType, std::tuple<> >
{
public:
    static constexpr bool Value = false;
};

#else // #if COMMS_IS_MSVC_2017_OR_BELOW

template <typename TType, typename... TRest>
class IsInTuple<TType, std::tuple<TRest...> >
{
    template <typename...>
    struct SameTypeCheck
    {
        template <typename T>
        using Type = std::integral_constant<bool, std::is_same<TType, T>::value>;
    };

public:
    static constexpr bool Value = 
        comms::util::Accumulate<>::template Type<
            SameTypeCheck,
            comms::util::LogicalOrBinaryOp,
            std::false_type,
            TRest...
        >::value;
};

#endif // #if COMMS_IS_MSVC_2017_OR_BELOW

/// @endcond

//----------------------------------------

/// @brief Calculated "aligned union" storage type for all the types in
///     provided tuple.
/// @tparam TTuple Tuple
/// @pre @code IsTuple<TTuple>::Value == true @endcode
template <typename TTuple>
struct TupleAsAlignedUnion
{
    /// @cond DOCUMENT_STATIC_ASSERT
    static_assert(IsTuple<TTuple>::Value, "TTuple must be std::tuple");
    /// @endcond

    /// @brief Type definition is invalid for any type that is not
    ///     <a href="http://en.cppreference.com/w/cpp/utility/tuple">std::tuple</a>,
    ///     will be specialised to proper value.
    using Type = void;
};

/// @cond SKIP_DOC
template <typename... TTypes>
struct TupleAsAlignedUnion<std::tuple<TTypes...> >
{
    using Type = typename AlignedUnion<TTypes...>::Type;
};
/// @endcond

/// @brief Alias to @ref TupleAsAlignedUnion::Type
template <typename TTuple>
using TupleAsAlignedUnionT = typename TupleAsAlignedUnion<TTuple>::Type;

//----------------------------------------

namespace details
{

template <bool THasElems>
struct TupleForEachHelper
{
    template <std::size_t TOff, std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static constexpr std::size_t OffsetedRem = TRem + TOff;
        static_assert(OffsetedRem <= TupleSize, "Incorrect parameters");

        static constexpr std::size_t Idx = TupleSize - OffsetedRem;
        static constexpr std::size_t NextRem = TRem - 1;
        static constexpr bool HasElemsToProcess = (NextRem != 0U);
        func(std::get<Idx>(std::forward<TTuple>(tuple)));
        TupleForEachHelper<HasElemsToProcess>::template exec<TOff, NextRem>(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
struct TupleForEachHelper<false>
{
    template <std::size_t TOff, std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        static_cast<void>(tuple);
        static_cast<void>(func);
    }
};

}  // namespace details

/// @brief Invoke provided functor for every element in the tuple.
/// @details The functor object class must define operator() with following signature:
///     @code
///     struct MyFunc
///     {
///         template <typename TTupleElem>
///         void operator()(TTupleElem&& elem) {...}
///     };
///     @endcode
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] func Functor object.
template <typename TTuple, typename TFunc>
void tupleForEach(TTuple&& tuple, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static constexpr bool HasTupleElems = (TupleSize != 0U);

    details::TupleForEachHelper<HasTupleElems>::template exec<0, TupleSize>(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}

/// @brief Invoke provided functor for every element in the tuple until
///     element with specified index is reached.
/// @details Very similar to tupleForEach() function, but also receives
///     index of the last element as a template parameter. The provided functor
///     is NOT invoked for the element with index TIdx.
/// @tparam TIdx Index of the last (not included) element.
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] func Functor object.
/// @pre TIdx mustn't exceed number of elements in the tuple.
template <std::size_t TIdx, typename TTuple, typename TFunc>
void tupleForEachUntil(TTuple&& tuple, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static_assert(TIdx <= TupleSize,
        "The index is too big.");

    static constexpr bool HasTupleElems = (TIdx != 0U);

    details::TupleForEachHelper<HasTupleElems>::template exec<TupleSize - TIdx, TIdx>(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}

/// @brief Invoke provided functor for every element in the tuple starting from
///     element with specified index.
/// @details Very similar to tupleForEach() function, but also receives
///     index of the first element as a template parameter.
/// @tparam TIdx Index of the first element to invoke functor on.
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] func Functor object.
/// @pre TIdx must be less than number of elements in the tuple.
template <std::size_t TIdx, typename TTuple, typename TFunc>
void tupleForEachFrom(TTuple&& tuple, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static_assert(TIdx <= TupleSize,
        "The index is too big.");
    static constexpr std::size_t RemCount = TupleSize - TIdx;
    static constexpr bool HasTupleElems = (RemCount != 0U);

    details::TupleForEachHelper<HasTupleElems>::template exec<0, RemCount>(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}

/// @brief Invoke provided functor for every element in the tuple which indices
///     are in range [TFromIdx, TUntilIdx).
/// @details Very similar to tupleForEach() function, but also receives
///     indices of the first and last elements as a template parameters.
/// @tparam TFromIdx Index of the first element to invoke functor on.
/// @tparam TUntilIdx Index of the last (not included) element.
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] func Functor object.
/// @pre TFromIdx must be less than number of elements in the tuple.
/// @pre TUntilIdx mustn't exceed number of elements in the tuple.
/// @pre TFormIdx <= TUntilIdx
template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TTuple, typename TFunc>
void tupleForEachFromUntil(TTuple&& tuple, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static_assert(TFromIdx <= TupleSize,
        "The from index is too big.");

    static_assert(TUntilIdx <= TupleSize,
        "The until index is too big.");

    static_assert(TFromIdx <= TUntilIdx,
        "The from index must be less than until index.");

    static constexpr std::size_t FieldsCount = TUntilIdx - TFromIdx;
    static constexpr bool HasTupleElems = (FieldsCount != 0U);

    details::TupleForEachHelper<HasTupleElems>::template exec<TupleSize - TUntilIdx, FieldsCount>(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}
//----------------------------------------

namespace details
{

template <bool THasElems>
struct TupleForEachTypeHelper
{
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static constexpr std::size_t Idx = TupleSize - TRem;
        static constexpr std::size_t NextRem = TRem - 1U;
        static constexpr bool NextHasElems = (NextRem != 0U);

        using ElemType = typename TupleElement<Tuple>::template Type<Idx>;
#if COMMS_IS_MSVC
        // VS compiler
        func.operator()<ElemType>();
#else // #if COMMS_IS_MSVC
        func.template operator()<ElemType>();
#endif // #if COMMS_IS_MSVC
        TupleForEachTypeHelper<NextHasElems>::template exec<NextRem, TTuple>(
            std::forward<TFunc>(func));
    }
};

template <>
struct TupleForEachTypeHelper<false>
{
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TFunc&& func)
    {
        static_cast<void>(func);
    }
};

}  // namespace details

/// @brief Invoke provided functor for every type in the tuple.
/// @details The functor object class must define operator() with following signature:
///     @code
///     struct MyFunc
///     {
///         template <typename TTupleElem>
///         void operator()() {...}
///     };
///     @endcode
/// @param[in] func Functor object.
template <typename TTuple, typename TFunc>
void tupleForEachType(TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static constexpr bool HasElems = (TupleSize != 0U);

    details::TupleForEachTypeHelper<HasElems>::template exec<TupleSize, Tuple>(
        std::forward<TFunc>(func));
}
//----------------------------------------


namespace details
{

template <bool THasElems>
struct TupleForEachWithIdxHelper
{
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static constexpr std::size_t Idx = TupleSize - TRem;
        static constexpr std::size_t NextRem = TRem - 1;
        static constexpr bool NextHasElems = (NextRem != 0U);

        func(std::get<Idx>(std::forward<TTuple>(tuple)), Idx);
        TupleForEachWithIdxHelper<NextHasElems>::template exec<NextRem>(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
struct TupleForEachWithIdxHelper<false>
{
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        static_cast<void>(tuple);
        static_cast<void>(func);
    }
};

}  // namespace details

/// @brief Invoke provided functor for every element in the tuple while providing
///     information about element index in the tuple.
/// @details Very similar to tupleForEach(), but the operator() in the functor
///     receives additional information about index of the element.
///     The functor object class must define operator() with following signature:
///     @code
///     struct MyFunc
///     {
///         template <typename TTupleElem>
///         void operator()(TTupleElem&& elem, std::size_t idx) {...}
///     };
///     @endcode
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] func Functor object.
template <typename TTuple, typename TFunc>
void tupleForEachWithIdx(TTuple&& tuple, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static constexpr bool HasElems = (TupleSize != 0U);

    details::TupleForEachWithIdxHelper<HasElems>::template exec<TupleSize>(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}

namespace details
{

template <bool THasElems>
struct TupleForEachWithTemplateParamIdxHelper
{
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static constexpr std::size_t Idx = TupleSize - TRem;
        static constexpr std::size_t NextRem = TRem - 1;
        static constexpr bool NextHasElems = (NextRem != 0U);

#if COMMS_IS_MSVC
        // VS compiler
        func.operator()<Idx>(std::get<Idx>(std::forward<TTuple>(tuple)));
#else // #if COMMS_IS_MSVC
        func.template operator()<Idx>(std::get<Idx>(std::forward<TTuple>(tuple)));
#endif // #if COMMS_IS_MSVC
        TupleForEachWithTemplateParamIdxHelper<NextHasElems>::template exec<NextRem>(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
struct TupleForEachWithTemplateParamIdxHelper<false>
{
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        static_cast<void>(tuple);
        static_cast<void>(func);
    }
};

}  // namespace details

/// @brief Invoke provided functor for every element in the tuple while providing
///     information about element index in the tuple as a template parameter.
/// @details Very similar to tupleForEachWithIdx(), but the operator() in the functor
///     receives additional information about index of the element as a template
///     parameter instead of as argument to the function.
///     The functor object class must define operator() with following signature:
///     @code
///     struct MyFunc
///     {
///         template <std::size_t TIdx, typename TTupleElem>
///         void operator()(TTupleElem&& elem) {...}
///     };
///     @endcode
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] func Functor object.

template <typename TTuple, typename TFunc>
void tupleForEachWithTemplateParamIdx(TTuple&& tuple, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static constexpr bool HasElems = (TupleSize != 0U);

    details::TupleForEachWithTemplateParamIdxHelper<HasElems>::template exec<TupleSize>(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}


//----------------------------------------

namespace details
{

template <bool THasElems>
struct TupleAccumulateHelper
{
    template <std::size_t TOff, std::size_t TRem, typename TTuple, typename TValue, typename TFunc>
    static constexpr TValue exec(TTuple&& tuple, const TValue& value, TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static_assert((TOff + TRem) <= std::tuple_size<Tuple>::value, "Incorrect params");

        return 
            TupleAccumulateHelper<(1U < TRem)>::template exec<TOff + 1, TRem - 1U>(
                std::forward<TTuple>(tuple),
                func(value, std::get<TOff>(std::forward<TTuple>(tuple))),
                std::forward<TFunc>(func));
    }
};

template <>
struct TupleAccumulateHelper<false>
{
    template <std::size_t TOff, std::size_t TRem, typename TTuple, typename TValue, typename TFunc>
    static constexpr TValue exec(TTuple&& /* tuple */, const TValue& value, TFunc&& /* func */)
    {
        return value;
    }
};

}  // namespace details

/// @brief Performs "accumulate" algorithm on every element of the tuple.
/// @details The algorithm invokes operator() of the provided functor object
///     with initial value and first element of the tuple, then provides the
///     returned value as a parameter to the next invocation of operator() and
///     second element in the tuple, and so on until all elements in the tuple
///     is handled.
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] value Initial value.
/// @param[in] func Functor object. The class must provide operator() with the
///     following signature:
///     @code
///     struct MyFunc
///     {
///         template <typename TValue, typename TTupleElem>
///         TValue operator()(const TValue& value, TTupleElem&& elem) {...}
///     };
///     @endcode
/// @return Returns the result of the last invocation of the functor's operator().
template <typename TTuple, typename TValue, typename TFunc>
constexpr TValue tupleAccumulate(TTuple&& tuple, const TValue& value, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;

    return details::TupleAccumulateHelper<std::tuple_size<Tuple>::value != 0>::template exec<0, std::tuple_size<Tuple>::value>(
                std::forward<TTuple>(tuple),
                value,
                std::forward<TFunc>(func));
}

/// @brief Performs "accumulate" algorithm on every element of the tuple.
/// @details Similar to @ref tupleAccumulate(), but allows specifying range of
///     indices of tuple elements.
/// @tparam TFrom Index of the first tuple element to evaluate
/// @tparam TUntil Index of the one past the last tuple element to evaluate.
/// @param[in] tuple Reference (l- or r-value) to tuple object.
/// @param[in] value Initial value.
/// @param[in] func Functor object. The class must provide operator() with the
///     following signature:
///     @code
///     struct MyFunc
///     {
///         template <typename TValue, typename TTupleElem>
///         TValue operator()(const TValue& value, TTupleElem&& elem) {...}
///     };
///     @endcode
/// @return Returns the result of the last invocation of the functor's operator().
template <std::size_t TFrom, std::size_t TUntil, typename TTuple, typename TValue, typename TFunc>
constexpr TValue tupleAccumulateFromUntil(TTuple&& tuple, const TValue& value, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;

    static_assert(TFrom <= TUntil, "TFrom mustn't be greater that TUntil");
    static_assert(TUntil <= std::tuple_size<Tuple>::value, "TUntil mustn't exceed size of the tuple");

    return details::TupleAccumulateHelper<(TFrom < TUntil)>::template exec<TFrom, TUntil - TFrom>(
                std::forward<TTuple>(tuple),
                value,
                std::forward<TFunc>(func));
}

//----------------------------------------

namespace details
{

template <bool THasElems>
class TupleTypeAccumulateHelper
{

public:
    template <std::size_t TOff, std::size_t TRem, typename TTuple, typename TValue, typename TFunc>
    static constexpr TValue exec(const TValue& value, TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static_assert((TOff + TRem) <= std::tuple_size<Tuple>::value, "Incorrect TRem");

        return TupleTypeAccumulateHelper<(1U < TRem)>::template exec<TOff + 1, TRem - 1, Tuple>(
#if COMMS_IS_MSVC
            func.operator()
#else // #if COMMS_IS_MSVC
            func.template operator()
#endif // #if COMMS_IS_MSVC
            <typename TupleElement<Tuple>::template Type<TOff> >(value),
            std::forward<TFunc>(func));
    }

};

template <>
class TupleTypeAccumulateHelper<false>
{

public:
    template <std::size_t TOff, std::size_t TRem, typename TTuple, typename TValue, typename TFunc>
    static constexpr TValue exec(const TValue& value, TFunc&& /* func */)
    {
        return value;
    }
};

}  // namespace details

/// @brief Performs "accumulate" algorithm on every type of the tuple.
/// @details Very similar to tupleAccumulate(), but without actual tuple object,
///     provides only type information to operator() of the functor.
/// @param[in] value Initial value.
/// @param[in] func Functor object. The class must provide operator() with the
///     following signature:
///     @code
///     struct MyFunc
///     {
///         template <typename TTupleElem, typename TValue>
///         TValue operator()(const TValue& value) {...}
///     };
///     @endcode
/// @return Returns the result of the last invocation of the functor's operator().
template <typename TTuple, typename TValue, typename TFunc>
constexpr TValue tupleTypeAccumulate(const TValue& value, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    return 
        details::TupleTypeAccumulateHelper<
            (0U < std::tuple_size<Tuple>::value)
        >::template exec<0, std::tuple_size<Tuple>::value, Tuple>(
            value,
            std::forward<TFunc>(func));
}

/// @brief Performs "accumulate" algorithm on specified types inside the tuple.
/// @details Very similar to @ref tupleTypeAccumulate(), but allows specifying range of
///     indices of tuple elements.
/// @tparam TFrom Index of the first tuple type to evaluate
/// @tparam TUntil Index of the one past the last tuple type to evaluate.
/// @param[in] value Initial value.
/// @param[in] func Functor object. The class must provide operator() with the
///     following signature:
///     @code
///     struct MyFunc
///     {
///         template <typename TTupleElem, typename TValue>
///         TValue operator()(const TValue& value) {...}
///     };
///     @endcode
/// @return Returns the result of the last invocation of the functor's operator().
template <std::size_t TFrom, std::size_t TUntil, typename TTuple, typename TValue, typename TFunc>
constexpr TValue tupleTypeAccumulateFromUntil(const TValue& value, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static_assert(TFrom <= TUntil, "TFrom mustn't be greater that TUntil");
    static_assert(TUntil <= std::tuple_size<Tuple>::value, "TUntil mustn't exceed size of the tuple");
    return 
        details::TupleTypeAccumulateHelper<
            (TFrom < TUntil)
        >::template exec<TFrom, TUntil - TFrom, Tuple>(
            value,
            std::forward<TFunc>(func));
}


//----------------------------------------


/// @brief Provides the type of <a href="http://en.cppreference.com/w/cpp/utility/tuple/tuple_cat">std::tuple_cat</a> operation.
/// @tparam TFirst Type of first tuple
/// @tparam TSecond Type of the second tuple.
template <typename TFirst, typename TSecond>
struct TupleCat
{
    static_assert(IsTuple<TFirst>::Value, "TFirst must be tuple");
    static_assert(IsTuple<TSecond>::Value, "TSecond must be tuple");

    /// @brief Result type of tuples concatenation.
    using Type = typename std::decay<decltype(std::tuple_cat(std::declval<TFirst>(), std::declval<TSecond>()))>::type;
};

/// @brief Alias to typename TupleCat<TField, TTuple>::Type
/// @related TupleCat
template <typename TField, typename TTuple>
using TupleCatT = typename TupleCat<TField, TTuple>::Type;

//----------------------------------------

namespace details
{

template <bool THasElems>
struct TupleSelectedTypeHelper;

template <>
struct TupleSelectedTypeHelper<false>
{
    template <std::size_t TFromIdx, std::size_t TToIdx, std::size_t TCount, typename TTuple, typename TFunc>
    static void exec(std::size_t idx, TFunc&& func)
    {
        static_assert((TFromIdx + 1) == TToIdx, "Internal error: Bad parameters");
        static_assert(TCount == 1, "Internal error: Bad parameters");
        static_cast<void>(idx);
        COMMS_ASSERT(idx == TFromIdx);
        using ElemType = typename TupleElement<TTuple>::template Type<TFromIdx>;
#if COMMS_IS_MSVC
        // VS compiler
        func.operator()<TFromIdx, ElemType>();
#else // #if COMMS_IS_MSVC
        func.template operator()<TFromIdx, ElemType>();
#endif // #if COMMS_IS_MSVC
    }
};

template <bool THasElems>
struct TupleSelectedTypeHelper
{
    template <std::size_t TFromIdx, std::size_t TToIdx, std::size_t TCount, typename TTuple, typename TFunc>
    static void exec(std::size_t idx, TFunc&& func)
    {
        static_assert(1U < TCount, "Internal error: Bad parameters");
        static_assert(TCount == (TToIdx - TFromIdx), "Internal error: Bad parameters");
        static_assert(TFromIdx < TToIdx, "Internal error: Bad parameters");

        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TCount <= TupleSize, "Incorrect TCount");
        static_assert(0U < TCount, "Incorrect instantiation");

        COMMS_ASSERT(TFromIdx <= idx);
        COMMS_ASSERT(idx < TToIdx);
        if (idx == TFromIdx) {
            TupleSelectedTypeHelper<false>::template exec<TFromIdx, TFromIdx + 1, 1U, TTuple>(
                idx, std::forward<TFunc>(func));
            return;
        }

        static constexpr std::size_t MidIdx = TFromIdx + TCount / 2;
        static_assert(MidIdx < TToIdx, "Internal error: bad calculation");
        static_assert(TFromIdx <= MidIdx, "Internal error: bad calculation");
        if (MidIdx <= idx) {
            static constexpr std::size_t NextCount = TToIdx - MidIdx;
            static constexpr bool HasNextElems = (1U < NextCount);

            TupleSelectedTypeHelper<HasNextElems>::template exec<MidIdx, TToIdx, NextCount, TTuple>(
                idx, std::forward<TFunc>(func));
            return;
        }

        static constexpr std::size_t NextCount = MidIdx - TFromIdx;
        static constexpr bool HasNextElems = (1U < NextCount);

        TupleSelectedTypeHelper<HasNextElems>::template exec<TFromIdx, MidIdx, NextCount, TTuple>(
            idx, std::forward<TFunc>(func));
    }
};

}  // namespace details

/// @brief Invoke provided functor for a selected type when element index
///     is known only at run time.
/// @details The functor object class must define operator() with following signature:
///     @code
///     struct MyFunc
///     {
///         // TIdx - index of the type inside the tuple
///         // TTupleElem - type inside the tuple
///         template <std::size_t TIdx, typename TTupleElem>
///         void operator()() {...}
///     };
///     @endcode
/// @param[in] idx Index of the type in the tuple
/// @param[in] func Functor object.
template <typename TTuple, typename TFunc>
void tupleForSelectedType(std::size_t idx, TFunc&& func)
{
    using Tuple = typename std::decay<TTuple>::type;
    static_assert(isTuple<Tuple>(), "Provided tupe must be std::tuple");
    static constexpr std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static_assert(0U < TupleSize, "Empty tuples are not supported");

    details::TupleSelectedTypeHelper<(1U < TupleSize)>::template exec<0, TupleSize, TupleSize, Tuple>(
        idx, std::forward<TFunc>(func));
}
//----------------------------------------

namespace details
{

template <bool THasElems>
struct TupleStripFirstN
{
    template <std::size_t TCount, typename TFirst, typename... TElems>
    using Type = 
        typename TupleStripFirstN<
            (1U < TCount)
        >::template Type<
            TCount - 1, 
            TElems...
        >;
};

template <>
struct TupleStripFirstN<false>
{
    template <std::size_t TCount, typename... TElems>
    using Type = std::tuple<TElems...>;
};

template <typename TTuple>
struct TuplePackedStripFirstN;

template <typename... TElems>
struct TuplePackedStripFirstN<std::tuple<TElems...> >
{
    template <std::size_t TCount>
    using Type = 
        typename TupleStripFirstN<
            (0 < TCount)
        >::template Type<TCount, TElems...>;
};

template <bool TMustStrip>
struct TupleTailCheckHelpler
{
    template <std::size_t TCount, typename TElems>
    using StrippedTail = 
        typename TuplePackedStripFirstN<TElems>::template Type<
            TCount
        >;

    template <typename TTail, typename TElems>
    using Type = 
        std::integral_constant<
            bool,
            std::is_same<
                TTail, 
                StrippedTail<
                    (std::tuple_size<TElems>::value - std::tuple_size<TTail>::value),
                    TElems
                >
            >::value
        >;
};

template <>
struct TupleTailCheckHelpler<false>
{
    template <typename TTail, typename TElems>
    using Type = 
        std::integral_constant<
            bool,
            std::is_same<TTail, TElems>::value
    >;
};

} // namespace details

/// @brief Compile time check of whether one tuple is a "tail" of another.
/// @tparam TTail Tail tuple
/// @tparam TTuple Containing tuple
template <typename TTail, typename TTuple>
constexpr bool tupleIsTailOf()
{
    static_assert(isTuple<TTail>(), "TTail param must be tuple");
    static_assert(isTuple<TTuple>(), "TTuple param must be tuple");
    return
        details::TupleTailCheckHelpler<
            std::tuple_size<TTail>::value < std::tuple_size<TTuple>::value
        >::template Type<TTail, TTuple>::value;
}

//----------------------------------------

namespace details
{

template <bool THasElems>
class TupleTypeIsAnyOfHelper
{
public:
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static constexpr bool check(TFunc&& func)
    {
        using Tuple = typename std::decay<TTuple>::type;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static_assert(TRem <= std::tuple_size<Tuple>::value, "Incorrect TRem");
        using ElemType = typename TupleElement<Tuple>::template Type<std::tuple_size<Tuple>::value - TRem>;
        return
#if COMMS_IS_MSVC
            // VS compiler
            func.operator()<ElemType>() ||
#else // #if COMMS_IS_MSVC
            func.template operator()<ElemType>() ||
#endif // #if COMMS_IS_MSVC
            TupleTypeIsAnyOfHelper<1U < TRem>::template check<TRem - 1, TTuple>(
                std::forward<TFunc>(func));
    }
};

template <>
class TupleTypeIsAnyOfHelper<false>
{

public:
    template <std::size_t TRem, typename TTuple, typename TFunc>
    static constexpr bool check(TFunc&&)
    {
        return false;
    }
};

}

/// @brief Compile time check of whether @b any type within a tuple has a certain condition.
/// @details The functor object class must define operator() with following signature:
///     @code
///     struct MyFunc
///     {
///         // TTupleElem - type inside the tuple
///         template <typename TTupleElem>
///         constexpr bool operator()() const {...}
///     };
///     @endcode
/// @tparam TTuple Tuple
/// @tparam TFunc Functor object type
template <typename TTuple, typename TFunc>
constexpr bool tupleTypeIsAnyOf(TFunc&& func)
{
    static_assert(isTuple<TTuple>(), "Tuple as argument is expected");
    return details::TupleTypeIsAnyOfHelper<(0U < std::tuple_size<TTuple>::value)>::
            template check<std::tuple_size<TTuple>::value, TTuple>(std::forward<TFunc>(func));
}

}  // namespace util

}  // namespace comms
