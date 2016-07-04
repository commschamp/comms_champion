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

#include <type_traits>
#include <cstddef>
#include <tuple>

#include "comms/util/access.h"
#include "comms/ErrorStatus.h"
#include "MessageInterfaceOptionsParser.h"

namespace comms
{

namespace details
{

class MessageInterfaceEmptyBase {};

template <typename TEndian>
class MessageInterfaceEndianBase
{
public:
    typedef TEndian Endian;

    typedef comms::Field<comms::option::Endian<Endian> > Field;

protected:
    ~MessageInterfaceEndianBase() = default;

    template <typename T, typename TIter>
    static void writeData(T value, TIter& iter)
    {
        writeData<sizeof(T), T>(value, iter);
    }

    template <std::size_t TSize, typename T, typename TIter>
    static void writeData(T value, TIter& iter)
    {
        static_assert(TSize <= sizeof(T),
                                    "Cannot put more bytes than type contains");
        return util::writeData<TSize, T>(value, iter, Endian());
    }

    template <typename T, typename TIter>
    static T readData(TIter& iter)
    {
        return readData<T, sizeof(T)>(iter);
    }

    template <typename T, std::size_t TSize, typename TIter>
    static T readData(TIter& iter)
    {
        static_assert(TSize <= sizeof(T),
            "Cannot get more bytes than type contains");
        return util::readData<T, TSize>(iter, Endian());
    }
};

template <typename TOpt, bool THasEndian>
struct MessageInterfaceProcessEndianBase;

template <typename TOpt>
struct MessageInterfaceProcessEndianBase<TOpt, true>
{
    typedef MessageInterfaceEndianBase<typename TOpt::Endian> Type;
};

template <typename TOpt>
struct MessageInterfaceProcessEndianBase<TOpt, false>
{
    typedef MessageInterfaceEmptyBase Type;
};

template <typename TOpt>
using MessageInterfaceEndianBaseT =
    typename MessageInterfaceProcessEndianBase<TOpt, TOpt::HasEndian>::Type;

template <typename TBase, typename TId>
class MessageInterfaceIdTypeBase : public TBase
{
public:
    typedef TId MsgIdType;
    typedef typename std::conditional<
            std::is_integral<MsgIdType>::value || std::is_enum<MsgIdType>::value,
            MsgIdType,
            const MsgIdType&
        >::type MsgIdParamType;

    MsgIdParamType getId() const
    {
        return getIdImpl();
    }

protected:
    ~MessageInterfaceIdTypeBase() = default;
    virtual MsgIdParamType getIdImpl() const = 0;
};

template <typename TBase, typename TOpt, bool THasIdType>
struct MessageInterfaceProcessIdTypeBase;

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessIdTypeBase<TBase, TOpt, true>
{
    typedef MessageInterfaceIdTypeBase<TBase, typename TOpt::MsgIdType> Type;
};

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessIdTypeBase<TBase, TOpt, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpt>
using MessageInterfaceIdTypeBaseT =
    typename MessageInterfaceProcessIdTypeBase<TBase, TOpt, TOpt::HasMsgIdType>::Type;

template <typename TBase, typename TReadIter>
class MessageInterfaceReadOnlyBase : public TBase
{
public:
    typedef TReadIter ReadIterator;
    comms::ErrorStatus read(ReadIterator& iter, std::size_t size)
    {
        return this->readImpl(iter, size);
    }

protected:
    ~MessageInterfaceReadOnlyBase() = default;
    virtual comms::ErrorStatus readImpl(ReadIterator& iter, std::size_t size) = 0;
};

template <typename TBase, typename TWriteIter>
class MessageInterfaceWriteOnlyBase : public TBase
{
public:
    typedef TWriteIter WriteIterator;
    comms::ErrorStatus write(WriteIterator& iter, std::size_t size) const
    {
        return this->writeImpl(iter, size);
    }

protected:
    ~MessageInterfaceWriteOnlyBase() = default;
    virtual comms::ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const = 0;
};

template <typename TBase, typename TReadIter, typename TWriteIter>
class MessageInterfaceReadWriteBase : public TBase
{
public:
    typedef TReadIter ReadIterator;
    comms::ErrorStatus read(ReadIterator& iter, std::size_t size)
    {
        return this->readImpl(iter, size);
    }

    typedef TWriteIter WriteIterator;
    comms::ErrorStatus write(WriteIterator& iter, std::size_t size) const
    {
        return this->writeImpl(iter, size);
    }

protected:
    ~MessageInterfaceReadWriteBase() = default;
    virtual comms::ErrorStatus readImpl(ReadIterator& iter, std::size_t size) = 0;
    virtual comms::ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const = 0;
};

template <typename TBase, typename TOpt, bool THasReadIterator, bool THasWriteIterator>
struct MessageInterfaceProcessReadWriteBase;

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessReadWriteBase<TBase, TOpt, false, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessReadWriteBase<TBase, TOpt, false, true>
{
    typedef MessageInterfaceWriteOnlyBase<TBase, typename TOpt::WriteIterator> Type;
};

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessReadWriteBase<TBase, TOpt, true, false>
{
    typedef MessageInterfaceReadOnlyBase<TBase, typename TOpt::ReadIterator> Type;
};

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessReadWriteBase<TBase, TOpt, true, true>
{
    typedef MessageInterfaceReadWriteBase<TBase, typename TOpt::ReadIterator, typename TOpt::WriteIterator> Type;
};

template <typename TBase, typename TOpt>
using MessageInterfaceReadWriteBaseT =
    typename MessageInterfaceProcessReadWriteBase<TBase, TOpt, TOpt::HasReadIterator, TOpt::HasWriteIterator>::Type;

template <typename TBase, typename THandler>
class MessageInterfaceHandlerBase : public TBase
{
public:
    typedef THandler Handler;

    void dispatch(Handler& handler)
    {
        dispatchImpl(handler);
    }

protected:
    ~MessageInterfaceHandlerBase() = default;
    virtual void dispatchImpl(Handler& handler) = 0;
};

template <typename TBase, typename TOpt, bool THasHandler>
struct MessageInterfaceProcessHandlerBase;

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessHandlerBase<TBase, TOpt, true>
{
    typedef MessageInterfaceHandlerBase<TBase, typename TOpt::Handler> Type;
};

template <typename TBase, typename TOpt>
struct MessageInterfaceProcessHandlerBase<TBase, TOpt, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpt>
using MessageInterfaceHandlerBaseT =
    typename MessageInterfaceProcessHandlerBase<TBase, TOpt, TOpt::HasHandler>::Type;


template <typename TBase>
class MessageInterfaceValidBase : public TBase
{
public:
    bool valid() const
    {
        return validImpl();
    }

protected:
    ~MessageInterfaceValidBase() = default;
    virtual bool validImpl() const = 0;
};


template <typename TBase, bool THasValid>
struct MessageInterfaceProcessValidBase;

template <typename TBase>
struct MessageInterfaceProcessValidBase<TBase, true>
{
    typedef MessageInterfaceValidBase<TBase> Type;
};

template <typename TBase>
struct MessageInterfaceProcessValidBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpts>
using MessageInterfaceValidBaseT =
    typename MessageInterfaceProcessValidBase<TBase, TOpts::HasValid>::Type;

template <typename TBase>
class MessageInterfaceLengthBase : public TBase
{
public:
    std::size_t length() const
    {
        return lengthImpl();
    }

protected:
    ~MessageInterfaceLengthBase() = default;
    virtual std::size_t lengthImpl() const = 0;
};


template <typename TBase, bool THasLength>
struct MessageInterfaceProcessLengthBase;

template <typename TBase>
struct MessageInterfaceProcessLengthBase<TBase, true>
{
    typedef MessageInterfaceLengthBase<TBase> Type;
};

template <typename TBase>
struct MessageInterfaceProcessLengthBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpts>
using MessageInterfaceLengthBaseT =
    typename MessageInterfaceProcessLengthBase<TBase, TOpts::HasLength>::Type;

template <typename TBase>
class MessageInterfaceRefreshBase : public TBase
{
public:
    bool refresh()
    {
        return refreshImpl();
    }

protected:
    ~MessageInterfaceRefreshBase() = default;
    virtual bool refreshImpl()
    {
        return false;
    }
};


template <typename TBase, bool THasRefresh>
struct MessageInterfaceProcessRefreshBase;

template <typename TBase>
struct MessageInterfaceProcessRefreshBase<TBase, true>
{
    typedef MessageInterfaceRefreshBase<TBase> Type;
};

template <typename TBase>
struct MessageInterfaceProcessRefreshBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpts>
using MessageInterfaceRefreshBaseT =
    typename MessageInterfaceProcessRefreshBase<TBase, TOpts::HasRefresh>::Type;



template <typename... TOptions>
class MessageInterfaceBuilder
{
    typedef MessageInterfaceOptionsParser<TOptions...> ParsedOptions;

    static_assert(ParsedOptions::HasEndian,
        "The Message interface must specify Endian in its options");

    typedef MessageInterfaceEndianBaseT<ParsedOptions> EndianBase;
    typedef MessageInterfaceIdTypeBaseT<EndianBase, ParsedOptions> IdTypeBase;
    typedef MessageInterfaceReadWriteBaseT<IdTypeBase, ParsedOptions> ReadWriteBase;
    typedef MessageInterfaceValidBaseT<ReadWriteBase, ParsedOptions> ValidBase;
    typedef MessageInterfaceLengthBaseT<ValidBase, ParsedOptions> LengthBase;
    typedef MessageInterfaceHandlerBaseT<LengthBase, ParsedOptions> HandlerBase;
    typedef MessageInterfaceRefreshBaseT<HandlerBase, ParsedOptions> RefreshBase;

public:
    typedef ParsedOptions Options;
    typedef RefreshBase Type;
};

template <typename... TOptions>
using MessageInterfaceBuilderT =
    typename MessageInterfaceBuilder<TOptions...>::Type;

}  // namespace details

}  // namespace comms


