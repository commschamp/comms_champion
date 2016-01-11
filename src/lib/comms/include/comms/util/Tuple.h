//
// Copyright 2013 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <tuple>
#include <utility>
#include <type_traits>

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
    static const bool Value = false;
};

/// @cond SKIP_DOC
template <typename... TArgs>
struct IsTuple<std::tuple<TArgs...> >
{
    static const bool Value = true;
};
/// @endcond

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
    static const bool Value = false;
};

/// @cond SKIP_DOC
template <typename TType, typename TFirst, typename... TRest>
class IsInTuple<TType, std::tuple<TFirst, TRest...> >
{
public:
    static const bool Value =
        std::is_same<TType, TFirst>::value ||
        IsInTuple<TType, std::tuple<TRest...> >::Value;
};

template <typename TType>
class IsInTuple<TType, std::tuple<> >
{
public:
    static const bool Value = false;
};

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
    typedef void Type;
};

/// @cond SKIP_DOC
template <typename... TTypes>
struct TupleAsAlignedUnion<std::tuple<TTypes...> >
{
    typedef typename AlignedUnion<TTypes...>::Type Type;
};
/// @endcond

//----------------------------------------

/// @brief Check whether tuple is unique, i.e. doesn't have contain types.
template <typename TTuple>
struct TupleIsUnique
{
    static_assert(IsTuple<TTuple>::Value, "TTuple must be std::tuple");

    /// @brief Value is set to true when tuple is discovered to be unique.
    static const bool Value = false;
};

/// @cond SKIP_DOC
template <typename TFirst, typename... TRest>
struct TupleIsUnique<std::tuple<TFirst, TRest...> >
{
    static const bool Value =
        (!IsInTuple<TFirst, std::tuple<TRest...> >::Value) &&
        TupleIsUnique<std::tuple<TRest...> >::Value;
};

template <>
struct TupleIsUnique<std::tuple<> >
{
    static const bool Value = true;
};

/// @endcond

//----------------------------------------

namespace details
{

template <std::size_t TRem, std::size_t TOff = 0>
class TupleForEachHelper
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static const std::size_t OffsetedRem = TRem + TOff;
        static_assert(OffsetedRem <= TupleSize, "Incorrect parameters");

        static const std::size_t Idx = TupleSize - OffsetedRem;
        func(std::get<Idx>(std::forward<TTuple>(tuple)));
        TupleForEachHelper<TRem - 1, TOff>::exec(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <std::size_t TOff>
class TupleForEachHelper<0, TOff>
{

public:
    template <typename TTuple, typename TFunc>
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
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    details::TupleForEachHelper<TupleSize>::exec(
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
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static_assert(TIdx <= TupleSize,
        "The index is too big.");

    static_assert(0U < TIdx,
        "The index must be greater than 0.");

    details::TupleForEachHelper<TIdx, TupleSize - TIdx>::exec(
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
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static_assert(TIdx < TupleSize,
        "The index is too big.");

    details::TupleForEachHelper<TupleSize - TIdx>::exec(
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
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
    static_assert(TFromIdx < TupleSize,
        "The from index is too big.");

    static_assert(TUntilIdx <= TupleSize,
        "The until index is too big.");

    static_assert(TFromIdx < TUntilIdx,
        "The from index must be less than until index.");

    static const std::size_t FieldsCount = TUntilIdx - TFromIdx;

    details::TupleForEachHelper<FieldsCount, TupleSize - TUntilIdx>::exec(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}
//----------------------------------------

namespace details
{

template <std::size_t TRem>
class TupleForEachTypeHelper
{
public:
    template <typename TTuple, typename TFunc>
    static void exec(TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static const std::size_t Idx = TupleSize - TRem;
        typedef typename std::tuple_element<Idx, Tuple>::type ElemType;
#ifdef _MSC_VER
        // VS compiler
        func.operator()<ElemType>();
#else // #ifdef _MSC_VER
        func.template operator()<ElemType>();
#endif // #ifdef _MSC_VER
        TupleForEachTypeHelper<TRem - 1>::template exec<TTuple>(
            std::forward<TFunc>(func));
    }
};

template <>
class TupleForEachTypeHelper<0>
{

public:
    template <typename TTuple, typename TFunc>
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
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    details::TupleForEachTypeHelper<TupleSize>::template exec<Tuple>(
        std::forward<TFunc>(func));
}
//----------------------------------------


namespace details
{

template <std::size_t TRem>
class TupleForEachWithIdxHelper
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static const std::size_t Idx = TupleSize - TRem;
        func(std::get<Idx>(std::forward<TTuple>(tuple)), Idx);
        TupleForEachWithIdxHelper<TRem - 1>::exec(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
class TupleForEachWithIdxHelper<0>
{

public:
    template <typename TTuple, typename TFunc>
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
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    details::TupleForEachWithIdxHelper<TupleSize>::exec(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}

namespace details
{

template <std::size_t TRem>
class TupleForEachWithTemplateParamIdxHelper
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static const std::size_t Idx = TupleSize - TRem;
#ifdef _MSC_VER
        // VS compiler
        func.operator()<Idx>(std::get<Idx>(std::forward<TTuple>(tuple)));
#else // #ifdef _MSC_VER
        func.template operator()<Idx>(std::get<Idx>(std::forward<TTuple>(tuple)));
#endif // #ifdef _MSC_VER
        TupleForEachWithTemplateParamIdxHelper<TRem - 1>::exec(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
class TupleForEachWithTemplateParamIdxHelper<0>
{

public:
    template <typename TTuple, typename TFunc>
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
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    details::TupleForEachWithTemplateParamIdxHelper<TupleSize>::exec(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}


//----------------------------------------

namespace details
{

template <std::size_t TRem>
class TupleAccumulateHelper
{

public:
    template <typename TTuple, typename TValue, typename TFunc>
    static constexpr TValue exec(TTuple&& tuple, const TValue& value, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static_assert(TRem <= std::tuple_size<Tuple>::value, "Incorrect TRem");

        return TupleAccumulateHelper<TRem - 1>::exec(
                    std::forward<TTuple>(tuple),
                    func(value, std::get<std::tuple_size<Tuple>::value - TRem>(std::forward<TTuple>(tuple))),
                    std::forward<TFunc>(func));
    }
};

template <>
class TupleAccumulateHelper<0>
{

public:
    template <typename TTuple, typename TValue, typename TFunc>
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
    typedef typename std::decay<TTuple>::type Tuple;

    return details::TupleAccumulateHelper<std::tuple_size<Tuple>::value>::exec(
                std::forward<TTuple>(tuple),
                value,
                std::forward<TFunc>(func));
}

//----------------------------------------

namespace details
{

template <std::size_t TRem>
class TupleTypeAccumulateHelper
{

public:
    template <typename TTuple, typename TValue, typename TFunc>
    static constexpr TValue exec(const TValue& value, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static_assert(TRem <= std::tuple_size<Tuple>::value, "Incorrect TRem");

        return TupleTypeAccumulateHelper<TRem - 1>::template exec<Tuple>(
            func.template operator()<typename std::tuple_element<std::tuple_size<Tuple>::value - TRem, Tuple>::type>(value),
            std::forward<TFunc>(func));
    }
};

template <>
class TupleTypeAccumulateHelper<0>
{

public:
    template <typename TTuple, typename TValue, typename TFunc>
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
    typedef typename std::decay<TTuple>::type Tuple;
    return details::TupleTypeAccumulateHelper<std::tuple_size<Tuple>::value>::template exec<Tuple>(
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
    typedef typename std::decay<decltype(std::tuple_cat(std::declval<TFirst>(), std::declval<TSecond>()))>::type Type;
};

/// @brief Alias to typename TupleCat<TField, TTuple>::Type
/// @related TupleCat
template <typename TField, typename TTuple>
using TupleCatT = typename TupleCat<TField, TTuple>::Type;

}  // namespace util

}  // namespace comms
