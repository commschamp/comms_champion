//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <tuple>
#include <cstddef>

#include "comms/Field.h"
#include "comms/util/access.h"
#include "comms/util/type_traits.h"
#include "comms/options.h"
#include "comms/Assert.h"
#include "comms/details/tag.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace details
{

using MessageInterfaceEmptyBase = comms::util::EmptyStruct<>;

// ------------------------------------------------------

template <typename TEndian>
class MessageInterfaceEndianBase
{
public:
    using Endian = TEndian;

    using Field = comms::Field<comms::option::def::Endian<Endian> >;

protected:
    ~MessageInterfaceEndianBase() noexcept = default;

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

// ------------------------------------------------------

template <typename TBase, typename TId>
class MessageInterfaceIdTypeBase : public TBase
{
public:
    using MsgIdType = TId;
    using MsgIdParamType = 
        typename comms::util::Conditional<
            std::is_integral<MsgIdType>::value || std::is_enum<MsgIdType>::value
        >::template Type<
            MsgIdType,
            const MsgIdType&
        >;

protected:
    ~MessageInterfaceIdTypeBase() noexcept = default;
};

// ------------------------------------------------------

template <typename TBase, typename TFields>
class MessageInterfaceExtraTransportFieldsBase : public TBase
{
public:
    using TransportFields = TFields;

    static_assert(comms::util::isTuple<TransportFields>(),
                  "TransportFields is expected to be tuple");

    TransportFields& transportFields()
    {
        return transportFields_;
    }

    const TransportFields& transportFields() const
    {
        return transportFields_;
    }

protected:
    ~MessageInterfaceExtraTransportFieldsBase() noexcept = default;
private:
    TransportFields transportFields_;
};

// ------------------------------------------------------

template <typename TBase, std::size_t TIdx>
class MessageInterfaceVersionInExtraTransportFieldsBase : public TBase
{
public:
    using TransportFields = typename TBase::TransportFields;

    static_assert(comms::util::isTuple<TransportFields>(),
                  "TransportFields is expected to be tuple");

    static_assert(TIdx < std::tuple_size<TransportFields>::value,
                  "Index provided to comms::option::def::VersionInExtraTransportFields exceeds size of the tuple");

    using VersionType = typename std::tuple_element<TIdx, TransportFields>::type::ValueType;

    VersionType& version()
    {
        return std::get<TIdx>(TBase::transportFields()).value();
    }

    const VersionType& version() const
    {
        return std::get<TIdx>(TBase::transportFields()).value();
    }

protected:
    ~MessageInterfaceVersionInExtraTransportFieldsBase() noexcept = default;
};

// ------------------------------------------------------

template <typename TBase>
class MessageInterfaceIdInfoBase : public TBase
{
public:
    using MsgIdParamType = typename TBase::MsgIdParamType;

    MsgIdParamType getId() const
    {
        return getIdImpl();
    }

protected:
    ~MessageInterfaceIdInfoBase() noexcept = default;
    virtual MsgIdParamType getIdImpl() const = 0;
};

// ------------------------------------------------------

template <typename TBase, typename TReadIter>
class MessageInterfaceReadBase : public TBase
{
public:
    using ReadIterator = TReadIter;
    comms::ErrorStatus read(ReadIterator& iter, std::size_t size)
    {
        return this->readImpl(iter, size);
    }

protected:
    ~MessageInterfaceReadBase() noexcept = default;
    virtual comms::ErrorStatus readImpl(ReadIterator& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return comms::ErrorStatus::NotSupported;
    }
};

// ------------------------------------------------------

template <typename TBase, typename TWriteIter>
class MessageInterfaceWriteBase : public TBase
{
public:
    using WriteIterator = TWriteIter;
    comms::ErrorStatus write(WriteIterator& iter, std::size_t size) const
    {
        return this->writeImpl(iter, size);
    }

protected:
    ~MessageInterfaceWriteBase() noexcept = default;
    virtual comms::ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return comms::ErrorStatus::NotSupported;
    }
};

// ------------------------------------------------------

template <typename TBase>
class MessageInterfaceValidBase : public TBase
{
public:
    bool valid() const
    {
        return validImpl();
    }

protected:
    ~MessageInterfaceValidBase() noexcept = default;
    virtual bool validImpl() const
    {
        return true;
    }
};

// ------------------------------------------------------

template <typename TBase>
class MessageInterfaceLengthBase : public TBase
{
public:
    std::size_t length() const
    {
        return lengthImpl();
    }

protected:
    ~MessageInterfaceLengthBase() noexcept = default;
    virtual std::size_t lengthImpl() const
    {
        COMMS_ASSERT(!"Not overridden");
        return 0;
    }
};

// ------------------------------------------------------

template <class T, class R = void>
struct MessageInterfaceIfHasRetType { using Type = R; };

template <class T, class Enable = void>
struct MessageInterfaceDispatchRetTypeHelper
{
    using Type = void;
};

template <class T>
struct MessageInterfaceDispatchRetTypeHelper<T, typename MessageInterfaceIfHasRetType<typename T::RetType>::Type>
{
    using Type = typename T::RetType;
};

template <class T>
using MessageInterfaceDispatchRetType = typename MessageInterfaceDispatchRetTypeHelper<T>::Type;


template <typename TBase, typename THandler>
class MessageInterfaceHandlerBase : public TBase
{
public:
    using Handler = THandler;
    using DispatchRetType = MessageInterfaceDispatchRetType<Handler>;

    DispatchRetType dispatch(Handler& handler)
    {
        return dispatchImpl(handler);
    }

protected:
    ~MessageInterfaceHandlerBase() noexcept = default;
    virtual DispatchRetType dispatchImpl(Handler& handler)
    {
        static_cast<void>(handler);
        COMMS_ASSERT(!"Mustn't be called");
        using Tag =
            typename comms::util::Conditional<
                std::is_void<DispatchRetType>::value
            >::template Type<
                VoidHandleRetTypeTag,
                NonVoidHandleRetTypeTag
            >;
        return dispatchInternal(Tag());
    }

private:
    using VoidHandleRetTypeTag = comms::details::tag::Tag1<>;
    using NonVoidHandleRetTypeTag = comms::details::tag::Tag2<>;

    static DispatchRetType dispatchInternal(VoidHandleRetTypeTag)
    {
        return;
    }

    static DispatchRetType dispatchInternal(NonVoidHandleRetTypeTag)
    {
        using RetTypeInternal = typename std::decay<DispatchRetType>::type;
        static const RetTypeInternal Ret = RetTypeInternal();
        return Ret;
    }
};

// ------------------------------------------------------

template <typename TBase>
class MessageInterfaceRefreshBase : public TBase
{
public:
    bool refresh()
    {
        return refreshImpl();
    }

protected:
    ~MessageInterfaceRefreshBase() noexcept = default;
    virtual bool refreshImpl()
    {
        return false;
    }
};

// ------------------------------------------------------

template <typename TBase>
class MessageInterfaceNameBase : public TBase
{
public:
    const char* name() const
    {
        return nameImpl();
    }

protected:
    ~MessageInterfaceNameBase() noexcept = default;
    virtual const char* nameImpl() const = 0;
};

// ------------------------------------------------------

template <typename TBase>
class MessageInterfaceVirtDestructorBase : public TBase
{
protected:
    virtual ~MessageInterfaceVirtDestructorBase() noexcept = default;
};

} // namespace details

} // namespace comms
