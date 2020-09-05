//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/options.h"

namespace comms
{

namespace details
{

template <typename... TOptions>
class MessageImplOptionsParser;

template <>
class MessageImplOptionsParser<>
{
public:
    static constexpr bool HasStaticMsgId = false;
    static constexpr bool HasFieldsImpl = false;
    static constexpr bool HasNoIdImpl = false;
    static constexpr bool HasMsgType = false;
    static constexpr bool HasNoDispatchImpl = false;
    static constexpr bool HasNoReadImpl = false;
    static constexpr bool HasNoWriteImpl = false;
    static constexpr bool HasNoLengthImpl = false;
    static constexpr bool HasNoValidImpl = false;
    static constexpr bool HasNoRefreshImpl = false;
    static constexpr bool HasCustomRefresh = false;
    static constexpr bool HasName = false;
    static constexpr bool HasDoGetId = false;
};

template <std::intmax_t TId,
          typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::StaticNumIdImpl<TId>,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    using BaseImpl = MessageImplOptionsParser<TOptions...>;

    static_assert(!BaseImpl::HasStaticMsgId,
        "comms::option::def::StaticNumIdImpl option is used more than once");
    static_assert(!BaseImpl::HasNoIdImpl,
        "comms::option::app::NoIdImpl and comms::option::def::StaticNumIdImpl options cannot be used together");
public:
    static constexpr bool HasStaticMsgId = true;
    static constexpr auto MsgId = TId;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoDispatchImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasNoDispatchImpl = true;
};

template <typename TFields,
          typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::FieldsImpl<TFields>,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    using BaseImpl = MessageImplOptionsParser<TOptions...>;

    static_assert(!BaseImpl::HasFieldsImpl,
        "comms::option::def::FieldsImpl option is used more than once");
public:
    static constexpr bool HasFieldsImpl = true;
    using Fields = TFields;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::NoIdImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    using BaseImpl = MessageImplOptionsParser<TOptions...>;

    static_assert(!BaseImpl::HasNoIdImpl,
        "comms::option::def::NoIdImpl option is used more than once");
    static_assert(!BaseImpl::HasStaticMsgId,
        "comms::option::def::NoIdImpl and comms::option::def::StaticNumIdImpl options cannot be used together");
public:
    static constexpr bool HasNoIdImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoReadImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasNoReadImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoWriteImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasNoWriteImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoLengthImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasNoLengthImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoValidImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasNoValidImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoRefreshImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasNoRefreshImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::HasCustomRefresh,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasCustomRefresh = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::HasName,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasName = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::HasDoGetId,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static constexpr bool HasDoGetId = true;
};

template <typename TMsgType,
          typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::MsgType<TMsgType>,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    using BaseImpl = MessageImplOptionsParser<TOptions...>;

    static_assert(!BaseImpl::HasMsgType,
        "comms::option::def::MsgType option is used more than once");
public:
    static constexpr bool HasMsgType = true;
    using MsgType = TMsgType;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class MessageImplOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public MessageImplOptionsParser<TBundledOptions..., TOptions...>
{
};

}  // namespace details

}  // namespace comms



