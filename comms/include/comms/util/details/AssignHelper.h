//
// Copyright 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// Various compile-time detection functions of whether specific member functions and/or types exist

#pragma once

#include <type_traits>
#include <iterator>

#include "comms/util/detect.h"
#include "comms/util/type_traits.h"
#include "comms/Assert.h"
#include "comms/details/tag.h"

namespace comms
{

namespace util
{

namespace details
{

template <typename...>
class AssignHelper
{
public:
    template <typename T, typename TIter>
    static void assign(T& obj, TIter from, TIter to)
    {
        using ObjType = typename std::decay<decltype(obj)>::type;
        static_assert(!std::is_same<Tag<ObjType>, UnknownTag<> >::value, "Assignment to provided type is not supported");
        assignInternal(obj, from, to, Tag<ObjType>());
    }

private:
    template <typename... TParams>
    using UseAssignTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using UsePtrSizeConstructorTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using StdSpanTag = comms::details::tag::Tag3<>;

    template <typename... TParams>
    using UnknownTag = comms::details::tag::Tag4<>;   

    template <typename T>
    using ConstructorTag = 
        typename comms::util::LazyShallowConditional<
            comms::util::detect::hasPtrSizeConstructor<T>()
        >::template Type<
            UsePtrSizeConstructorTag,
            UnknownTag
        >;    

    template <typename T>
    using SpanConstructorTag = 
        typename comms::util::LazyShallowConditional<
            comms::util::detect::details::IsStdSpan<T>::Value
        >::template Type<
            StdSpanTag,
            ConstructorTag,
            T
        >;    

    template <typename T>
    using Tag =
        typename comms::util::LazyShallowConditional<
            comms::util::detect::hasAssignFunc<T>()
        >::template Type<
            UseAssignTag,
            SpanConstructorTag,
            T
        >;    

    template <typename T, typename TIter, typename... TParams>
    static void assignInternal(T& obj, TIter from, TIter to, UseAssignTag<TParams...>)
    {
        obj.assign(from, to);
    }

    template <typename T, typename TIter, typename... TParams>
    static void assignInternal(T& obj, TIter from, TIter to, UsePtrSizeConstructorTag<TParams...>)
    {
        using IterType = typename std::decay<TIter>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(std::is_base_of<std::random_access_iterator_tag, IterTag>::value,    
            "Only random access iterator is supported for provided type assignments");         

        auto diff = std::distance(from, to);
        if (diff < 0) {
            static constexpr bool Invalid_iterators_used_for_assignment = false;
            static_cast<void>(Invalid_iterators_used_for_assignment);
            COMMS_ASSERT(Invalid_iterators_used_for_assignment);
            return;
        }

        using ObjType = typename std::decay<decltype(obj)>::type;
        obj = ObjType(&(*from), static_cast<std::size_t>(diff));
    } 

    template <typename T, typename TIter, typename... TParams>
    static void assignInternal(T& obj, TIter from, TIter to, StdSpanTag<TParams...>)
    {
        using ObjType = typename std::decay<decltype(obj)>::type;
        using ConstPointerType = typename ObjType::const_pointer;
        using PointerType = typename ObjType::pointer;
        auto fromPtr = const_cast<PointerType>(reinterpret_cast<ConstPointerType>(&(*from)));
        auto toPtr = const_cast<PointerType>(reinterpret_cast<ConstPointerType>(&(*to)));
        assignInternal(obj, fromPtr, toPtr, UsePtrSizeConstructorTag<TParams...>());
    }          
};

} // namespace details

} // namespace util

} // namespace comms
