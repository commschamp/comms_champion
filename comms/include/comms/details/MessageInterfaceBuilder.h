//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <cstddef>
#include <tuple>

#include "comms/Assert.h"
#include "comms/util/Tuple.h"
#include "comms/util/type_traits.h"
#include "comms/ErrorStatus.h"
#include "comms/details/MessageInterfaceOptionsParser.h"

namespace comms
{

namespace details
{

template <typename... TOptions>
class MessageInterfaceBuilder
{
    using ParsedOptions = MessageInterfaceOptionsParser<TOptions...>;

    static_assert((!ParsedOptions::HasVersionInExtraTransportFields) || ParsedOptions::HasExtraTransportFields,
        "comms::option::def::VersionInExtraTransportFields option should not be used "
        "without comms::option::def::ExtraTransportFields.");

    static constexpr bool MustHaveVirtualDestructor = 
        (!ParsedOptions::HasNoVirtualDestructor) && 
        (
            ParsedOptions::HasReadIterator ||
            ParsedOptions::HasWriteIterator ||
            ParsedOptions::HasMsgIdInfo ||
            ParsedOptions::HasHandler ||
            ParsedOptions::HasValid ||
            ParsedOptions::HasLength ||
            ParsedOptions::HasRefresh ||
            ParsedOptions::HasName          
        );

    using EndianBase = typename ParsedOptions::template BuildEndian<>;

    using IdTypeBase = 
        typename ParsedOptions::template BuildMsgIdType<EndianBase>;

    using TransportFieldsBase = 
        typename ParsedOptions::template BuildExtraTransportFields<IdTypeBase>;

    using VersionInTransportFieldsBase = 
        typename ParsedOptions::template BuildVersionInExtraTransportFields<TransportFieldsBase>;

    using IdInfoBase = 
        typename ParsedOptions::template BuildMsgIdInfo<VersionInTransportFieldsBase>;

    using ReadBase = 
        typename ParsedOptions::template BuildReadBase<IdInfoBase>;        

    using WriteBase = 
        typename ParsedOptions::template BuildWriteBase<ReadBase>;           
    
    using ValidBase = 
        typename ParsedOptions::template BuildValid<WriteBase>;           

    using LengthBase = 
        typename ParsedOptions::template BuildLength<ValidBase>;     
            
    using HandlerBase = 
        typename ParsedOptions::template BuildHandler<LengthBase>;     

    using RefreshBase = 
        typename ParsedOptions::template BuildRefresh<HandlerBase>;     

    using NameBase = 
        typename ParsedOptions::template BuildName<RefreshBase>;     

    using VirtDestructorBase = 
        typename comms::util::LazyShallowDeepConditional<
            MustHaveVirtualDestructor
        >::template Type<
            MessageInterfaceVirtDestructorBase,
            comms::util::TypeDeepWrap,
            NameBase
        >;    
public:
    using Options = ParsedOptions;
    using Type = VirtDestructorBase;
};

template <typename... TOptions>
using MessageInterfaceBuilderT =
    typename MessageInterfaceBuilder<TOptions...>::Type;

}  // namespace details

}  // namespace comms


