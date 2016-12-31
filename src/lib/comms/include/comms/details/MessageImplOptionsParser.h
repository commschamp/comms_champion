//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

namespace comms
{

namespace details
{

template <typename... TOptions>
struct MessageImplOptionsParser;

template <>
struct MessageImplOptionsParser<>
{
    static const bool HasStaticMsgId = false;
    static const bool HasDispatchImpl = false;
    static const bool HasFieldsImpl = false;
    static const bool HasNoIdImpl = false;
    static const bool HasNoDefaultFieldsReadImpl = false;
    static const bool HasNoDefaultFieldsWriteImpl = false;
    static const bool HasMsgType = false;
    static const bool HasMsgDoRead = false;
    static const bool HasMsgDoWrite = false;
    static const bool HasMsgDoValid = false;
    static const bool HasMsgDoLength = false;
    static const bool HasMsgDoRefresh = false;
};

template <std::intmax_t TId,
          typename... TOptions>
class MessageImplOptionsParser<
    comms::option::StaticNumIdImpl<TId>,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;
    typedef comms::option::StaticNumIdImpl<TId> Option;

    static_assert(!Base::HasStaticMsgId,
        "comms::option::StaticNumIdImpl option is used more than once");
    static_assert(!Base::HasNoIdImpl,
        "comms::option::NoIdImpl and comms::option::StaticNumIdImpl options cannot be used together");
public:
    static const bool HasStaticMsgId = true;
    static const auto MsgId = Option::Value;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::DispatchImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;

    static_assert(!Base::HasDispatchImpl,
        "comms::option::DispatchImpl option is used more than once");
public:
    static const bool HasDispatchImpl = true;
};

template <typename TFields,
          typename... TOptions>
class MessageImplOptionsParser<
    comms::option::FieldsImpl<TFields>,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;
    typedef comms::option::FieldsImpl<TFields> Option;

    static_assert(!Base::HasFieldsImpl,
        "comms::option::FieldsImpl option is used more than once");
public:
    typedef typename Option::Fields Fields;
    static const bool HasFieldsImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::NoIdImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;

    static_assert(!Base::HasNoIdImpl,
        "comms::option::NoIdImpl option is used more than once");
    static_assert(!Base::HasStaticMsgId,
        "comms::option::NoIdImpl and comms::option::StaticNumIdImpl options cannot be used together");
public:
    static const bool HasNoIdImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::NoDefaultFieldsReadImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoDefaultFieldsReadImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::NoDefaultFieldsWriteImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoDefaultFieldsWriteImpl = true;
};

template <typename TMsgType,
          typename... TOptions>
class MessageImplOptionsParser<
    comms::option::MsgType<TMsgType>,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;
    typedef comms::option::MsgType<TMsgType> Option;

    static_assert(!Base::HasMsgType,
        "comms::option::MsgType option is used more than once");
public:
    typedef typename Option::Type MsgType;
    static const bool HasMsgType = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::MsgDoRead,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;

    static_assert(!Base::HasMsgDoRead,
        "comms::option::MsgDoRead option is used more than once");
public:
    static const bool HasMsgDoRead = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::MsgDoWrite,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;

    static_assert(!Base::HasMsgDoWrite,
        "comms::option::MsgDoWrite option is used more than once");
public:
    static const bool HasMsgDoWrite = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::MsgDoValid,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;

    static_assert(!Base::HasMsgDoValid,
        "comms::option::MsgDoValid option is used more than once");
public:
    static const bool HasMsgDoValid = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::MsgDoLength,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;

    static_assert(!Base::HasMsgDoLength,
        "comms::option::MsgDoLength option is used more than once");
public:
    static const bool HasMsgDoLength = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::MsgDoRefresh,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
    typedef MessageImplOptionsParser<TOptions...> Base;

    static_assert(!Base::HasMsgDoRefresh,
        "comms::option::MsgDoLength option is used more than once");
public:
    static const bool HasMsgDoRefresh = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::EmptyOption,
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



