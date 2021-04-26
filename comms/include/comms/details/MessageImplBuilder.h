//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <cstddef>
#include <tuple>

#include "comms/CompileControl.h"
#include "comms/Assert.h"
#include "comms/util/access.h"
#include "comms/util/Tuple.h"
#include "comms/util/type_traits.h"
#include "comms/ErrorStatus.h"
#include "MessageImplOptionsParser.h"

namespace comms
{

namespace details
{

template <typename TMessage, typename... TOptions>
class MessageImplBuilder
{
    using ParsedOptions = MessageImplOptionsParser<TOptions...>;
    using InterfaceOptions = typename TMessage::InterfaceOptions;

    static_assert(ParsedOptions::HasFieldsImpl, "Option comms::option::def::FieldsImpl must be used");

    using FieldsBase = 
        typename ParsedOptions::template BuildFieldsImpl<TMessage>;

    using VersionBase = 
        typename ParsedOptions::template BuildVersionImpl<FieldsBase>;

    using FieldsReadImplBase = 
        typename ParsedOptions::template BuildReadImpl<VersionBase>;

    using FieldsWriteImplBase = 
        typename ParsedOptions::template BuildWriteImpl<FieldsReadImplBase>;

    using FieldsValidBase = 
        typename ParsedOptions::template BuildValidImpl<FieldsWriteImplBase>;

    using FieldsLengthBase = 
        typename ParsedOptions::template BuildLengthImpl<FieldsValidBase>;

    using RefreshBase = 
        typename ParsedOptions::template BuildRefreshImpl<FieldsLengthBase>;

    using DispatchBase = 
        typename ParsedOptions::template BuildDispatchImpl<RefreshBase>;        

    using StaticNumIdBase = 
        typename ParsedOptions::template BuildStaticMsgId<DispatchBase>;  

    using PolymorphicStaticNumIdBase = 
        typename ParsedOptions::template BuildMsgIdImpl<StaticNumIdBase>;  

    using NameBase = 
        typename ParsedOptions::template BuildNameImpl<PolymorphicStaticNumIdBase>;  

public:
    using Options = ParsedOptions;
    using Type = NameBase;
};

template <typename TMessage, typename... TOptions>
using MessageImplBuilderT =
    typename MessageImplBuilder<TMessage, TOptions...>::Type;

}  // namespace details

}  // namespace comms


