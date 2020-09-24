//
// Copyright 2013 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/CompileControl.h"
#include <type_traits>
#include "comms/util/type_traits.h"

namespace comms
{

namespace util
{

/// @cond SKIP_DOC
//#if COMMS_IS_MSVC_2017_OR_BELOW
#if 1 // TODO: CLEAN

template <typename TType, typename... TTypes>
class AlignedUnion
{
    using OtherStorage = typename AlignedUnion<TTypes...>::Type;
    static const std::size_t OtherSize = sizeof(OtherStorage);
    static const std::size_t OtherAlignment = std::alignment_of<OtherStorage>::value;
    using FirstStorage = typename AlignedUnion<TType>::Type;
    static const std::size_t FirstSize = sizeof(FirstStorage);
    static const std::size_t FirstAlignment = std::alignment_of<FirstStorage>::value;
    static const std::size_t MaxSize = FirstSize > OtherSize ? FirstSize : OtherSize;
    static const std::size_t MaxAlignment = FirstAlignment > OtherAlignment ? FirstAlignment : OtherAlignment;
public:
    /// Type that has proper size and proper alignment to keep any of the
    /// specified types
    using Type = typename std::aligned_storage<MaxSize, MaxAlignment>::type;
};

template <typename TType>
class AlignedUnion<TType>
{
public:
    using Type = typename std::aligned_storage<sizeof(TType), std::alignment_of<TType>::value>::type;
};

#else // #if COMMS_IS_MSVC_2017_OR_BELOW

template <typename... TTypes>
class AlignedUnion
{
    using AlignmentIntType = 
        comms::util::Accumulate<>::template Type<
            comms::util::AlignmentIntType,
            comms::util::IntMaxBinaryOp,
            std::integral_constant<std::size_t, 0U>,
            TTypes...
        >;

    using SizeIntType = 
        comms::util::Accumulate<>::template Type<
            comms::util::SizeIntType,
            comms::util::IntMaxBinaryOp,
            std::integral_constant<std::size_t, 0U>,
            TTypes...
        >;        
public:
    /// Type that has proper size and proper alignment to keep any of the
    /// specified types
    using Type = typename std::aligned_storage<SizeIntType::value, AlignmentIntType::value>::type;
};


#endif // #if COMMS_IS_MSVC_2017_OR_BELOW

/// @endcond

}  // namespace util

}  // namespace comms
