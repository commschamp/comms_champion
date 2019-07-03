//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
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
    static const bool HasStaticMsgId = false;
    static const bool HasFieldsImpl = false;
    static const bool HasNoIdImpl = false;
    static const bool HasMsgType = false;
    static const bool HasNoDispatchImpl = false;
    static const bool HasNoReadImpl = false;
    static const bool HasNoWriteImpl = false;
    static const bool HasNoLengthImpl = false;
    static const bool HasNoValidImpl = false;
    static const bool HasNoRefreshImpl = false;
    static const bool HasCustomRefresh = false;
    static const bool HasName = false;
    static const bool HasDoGetId = false;
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
    static const bool HasStaticMsgId = true;
    static const auto MsgId = TId;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoDispatchImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoDispatchImpl = true;
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
    static const bool HasFieldsImpl = true;
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
    static const bool HasNoIdImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoReadImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoReadImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoWriteImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoWriteImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoLengthImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoLengthImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoValidImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoValidImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::app::NoRefreshImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoRefreshImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::HasCustomRefresh,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasCustomRefresh = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::HasName,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasName = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::def::HasDoGetId,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasDoGetId = true;
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
    static const bool HasMsgType = true;
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



