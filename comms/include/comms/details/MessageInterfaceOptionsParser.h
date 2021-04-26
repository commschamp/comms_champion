//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>
#include <tuple>

#include "comms/options.h"
#include "comms/util/Tuple.h"
#include "MessageInterfaceBases.h"

namespace comms
{

namespace details
{

template <typename... TOptions>
class MessageInterfaceOptionsParser;

template <>
class MessageInterfaceOptionsParser<>
{
public:
    static constexpr bool HasEndian = false;
    static constexpr bool HasMsgIdType = false;
    static constexpr bool HasExtraTransportFields = false;
    static constexpr bool HasVersionInExtraTransportFields = false;
    static constexpr bool HasMsgIdInfo = false;
    static constexpr bool HasReadIterator = false;
    static constexpr bool HasWriteIterator = false;
    static constexpr bool HasValid = false;
    static constexpr bool HasLength = false;
    static constexpr bool HasHandler = false;
    static constexpr bool HasRefresh = false;
    static constexpr bool HasName = false;
    static constexpr bool HasNoVirtualDestructor = false;

    template <typename TBase = MessageInterfaceEmptyBase>
    using BuildEndian = TBase;

    template <typename TBase>
    using BuildMsgIdType = TBase;    

    template <typename TBase>
    using BuildExtraTransportFields = TBase;

    template <typename TBase>
    using BuildVersionInExtraTransportFields = TBase;

    template <typename TBase>
    using BuildMsgIdInfo = TBase;

    template <typename TBase>
    using BuildReadBase = TBase;    

    template <typename TBase>
    using BuildWriteBase = TBase;    

    template <typename TBase>
    using BuildValid = TBase;

    template <typename TBase>
    using BuildLength = TBase;

    template <typename TBase>
    using BuildHandler = TBase;   

    template <typename TBase>
    using BuildRefresh = TBase;  

    template <typename TBase>
    using BuildName = TBase;       
};

template <typename T, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::def::MsgIdType<T>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    using MsgIdType = T;
    static constexpr bool HasMsgIdType = true;

    template <typename TBase>
    using BuildMsgIdType = MessageInterfaceIdTypeBase<TBase, MsgIdType>;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::IdInfoInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasMsgIdInfo = true;

    template <typename TBase>
    using BuildMsgIdInfo = MessageInterfaceIdInfoBase<TBase>;
};

template <typename TEndian, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::def::Endian<TEndian>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasEndian = true;
    using Endian = TEndian;

    template <typename TBase = MessageInterfaceEmptyBase>
    using BuildEndian = MessageInterfaceEndianBase<Endian>;
};

template <typename TIter, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::ReadIterator<TIter>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasReadIterator = true;
    using ReadIterator = TIter;

    template <typename TBase>
    using BuildReadBase = MessageInterfaceReadBase<TBase, ReadIterator>;      
};

template <typename TIter, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::WriteIterator<TIter>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasWriteIterator = true;
    using WriteIterator = TIter;

    template <typename TBase>
    using BuildWriteBase = MessageInterfaceWriteBase<TBase, WriteIterator>;    
};

template <typename T, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::Handler<T>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasHandler = true;
    using Handler = T;

    template <typename TBase>
    using BuildHandler = MessageInterfaceHandlerBase<TBase, Handler>;    
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::ValidCheckInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasValid = true;

    template <typename TBase>
    using BuildValid = MessageInterfaceValidBase<TBase>;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::LengthInfoInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasLength = true;

    template <typename TBase>
    using BuildLength = MessageInterfaceLengthBase<TBase>;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::RefreshInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasRefresh = true;

    template <typename TBase>
    using BuildRefresh = MessageInterfaceRefreshBase<TBase>;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::NameInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasName = true;

    template <typename TBase>
    using BuildName = MessageInterfaceNameBase<TBase>;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::NoVirtualDestructor,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasNoVirtualDestructor = true;
};

template <typename TFields, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::def::ExtraTransportFields<TFields>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
    static_assert(comms::util::isTuple<TFields>(),
        "Template parameter to comms::option::def::ExtraTransportFields is expected to "
        "be std::tuple.");
public:
    static constexpr bool HasExtraTransportFields = true;
    using ExtraTransportFields = TFields;

    template <typename TBase>
    using BuildExtraTransportFields = MessageInterfaceExtraTransportFieldsBase<TBase, ExtraTransportFields>;
};

template <std::size_t TIdx, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::def::VersionInExtraTransportFields<TIdx>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static constexpr bool HasVersionInExtraTransportFields = true;
    static constexpr std::size_t VersionInExtraTransportFields = TIdx;

    template <typename TBase>
    using BuildVersionInExtraTransportFields = 
        MessageInterfaceVersionInExtraTransportFieldsBase<TBase, VersionInExtraTransportFields>;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class MessageInterfaceOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public MessageInterfaceOptionsParser<TBundledOptions..., TOptions...>
{
};



}  // namespace details

}  // namespace comms


