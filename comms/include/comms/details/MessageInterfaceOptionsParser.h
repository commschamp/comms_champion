//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>
#include <tuple>

#include "comms/options.h"
#include "comms/util/Tuple.h"

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
    static const bool HasMsgIdType = false;
    static const bool HasEndian = false;
    static const bool HasReadIterator = false;
    static const bool HasWriteIterator = false;
    static const bool HasMsgIdInfo = false;
    static const bool HasHandler = false;
    static const bool HasValid = false;
    static const bool HasLength = false;
    static const bool HasRefresh = false;
    static const bool HasName = false;
    static const bool HasNoVirtualDestructor = false;
    static const bool HasExtraTransportFields = false;
    static const bool HasVersionInExtraTransportFields = false;
};

template <typename T, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::def::MsgIdType<T>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    using MsgIdType = T;
    static const bool HasMsgIdType = true;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::IdInfoInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasMsgIdInfo = true;
};

template <typename TEndian, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::def::Endian<TEndian>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasEndian = true;
    using Endian = TEndian;
};

template <typename TIter, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::ReadIterator<TIter>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasReadIterator = true;
    using ReadIterator = TIter;
};

template <typename TIter, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::WriteIterator<TIter>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasWriteIterator = true;
    using WriteIterator = TIter;
};

template <typename T, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::Handler<T>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasHandler = true;
    using Handler = T;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::ValidCheckInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasValid = true;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::LengthInfoInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasLength = true;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::RefreshInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasRefresh = true;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::NameInterface,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasName = true;
};

template <typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::app::NoVirtualDestructor,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasNoVirtualDestructor = true;
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
    static const bool HasExtraTransportFields = true;
    using ExtraTransportFields = TFields;
};

template <std::size_t TIdx, typename... TOptions>
class MessageInterfaceOptionsParser<
    comms::option::def::VersionInExtraTransportFields<TIdx>,
    TOptions...> : public MessageInterfaceOptionsParser<TOptions...>
{
public:
    static const bool HasVersionInExtraTransportFields = true;
    static const std::size_t VersionInExtraTransportFields = TIdx;
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


