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
    static const bool HasFieldsImpl = false;
    static const bool HasNoIdImpl = false;
    static const bool HasMsgType = false;
    static const bool HasNoDispatchImpl = false;
    static const bool HasNoReadImpl = false;
    static const bool HasNoWriteImpl = false;
    static const bool HasNoLengthImpl = false;
    static const bool HasNoValidImpl = false;
    static const bool HasDoRefresh = false;
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
    comms::option::NoDispatchImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoDispatchImpl = true;
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
    comms::option::NoReadImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoReadImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::NoWriteImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoWriteImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::NoLengthImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoLengthImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::NoValidImpl,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasNoValidImpl = true;
};

template <typename... TOptions>
class MessageImplOptionsParser<
    comms::option::HasDoRefresh,
    TOptions...> : public MessageImplOptionsParser<TOptions...>
{
public:
    static const bool HasDoRefresh = true;
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



