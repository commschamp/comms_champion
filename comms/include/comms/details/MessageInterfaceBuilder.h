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

#include <type_traits>
#include <cstddef>
#include <tuple>

#include "comms/Assert.h"
#include "comms/util/access.h"
#include "comms/util/Tuple.h"
#include "comms/ErrorStatus.h"
#include "comms/details/MessageInterfaceOptionsParser.h"

namespace comms
{

namespace details
{

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


class MessageInterfaceEmptyBase {};

//----------------------------------------------------

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

template <bool THasEndian>
struct MessageInterfaceProcessEndianBase;

template <>
struct MessageInterfaceProcessEndianBase<true>
{
    template <typename TOpt>
    using Type = MessageInterfaceEndianBase<typename TOpt::Endian>;
};

template <>
struct MessageInterfaceProcessEndianBase<false>
{
    template <typename TOpt>
    using Type = MessageInterfaceEmptyBase;
};

template <typename TOpt>
using MessageInterfaceEndianBaseT =
    typename MessageInterfaceProcessEndianBase<TOpt::HasEndian>::template Type<TOpt>;

//----------------------------------------------------

template <typename TBase, typename TId>
class MessageInterfaceIdTypeBase : public TBase
{
public:
    using MsgIdType = TId;
    using MsgIdParamType = typename std::conditional<
            std::is_integral<MsgIdType>::value || std::is_enum<MsgIdType>::value,
            MsgIdType,
            const MsgIdType&
        >::type;

protected:
    ~MessageInterfaceIdTypeBase() noexcept = default;
};

template <bool THasIdType>
struct MessageInterfaceProcessIdTypeBase;

template <>
struct MessageInterfaceProcessIdTypeBase<true>
{
    template<typename TBase, typename TOpt>
    using Type = MessageInterfaceIdTypeBase<TBase, typename TOpt::MsgIdType>;
};

template <>
struct MessageInterfaceProcessIdTypeBase<false>
{
    template<typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageInterfaceIdTypeBaseT =
    typename MessageInterfaceProcessIdTypeBase<TOpt::HasMsgIdType>::template Type<TBase, TOpt>;

//----------------------------------------------------

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

template <bool THasExtraTransportFields>
struct MessageInterfaceProcessExtraTransportFieldsBase;

template <>
struct MessageInterfaceProcessExtraTransportFieldsBase<true>
{
    template<typename TBase, typename TOpt>
    using Type = MessageInterfaceExtraTransportFieldsBase<TBase, typename TOpt::ExtraTransportFields>;
};

template <>
struct MessageInterfaceProcessExtraTransportFieldsBase<false>
{
    template<typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageInterfaceExtraTransportFieldsBaseT =
    typename MessageInterfaceProcessExtraTransportFieldsBase<TOpt::HasExtraTransportFields>::template Type<TBase, TOpt>;

//----------------------------------------------------

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

template <bool THasVersionInExtraTransportFields>
struct MessageInterfaceProcessVersionInExtraTransportFieldsBase;

template <>
struct MessageInterfaceProcessVersionInExtraTransportFieldsBase<true>
{
    template<typename TBase, typename TOpt>
    using Type = MessageInterfaceVersionInExtraTransportFieldsBase<TBase, TOpt::VersionInExtraTransportFields>;
};

template <>
struct MessageInterfaceProcessVersionInExtraTransportFieldsBase<false>
{
    template<typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageInterfaceVersionInExtraTransportFieldsBaseT =
    typename MessageInterfaceProcessVersionInExtraTransportFieldsBase<TOpt::HasVersionInExtraTransportFields>::template Type<TBase, TOpt>;

//----------------------------------------------------

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

template <bool THasIdInfo>
struct MessageInterfaceProcessIdInfoBase;

template <>
struct MessageInterfaceProcessIdInfoBase<true>
{
    template <typename TBase>
    using Type = MessageInterfaceIdInfoBase<TBase>;
};

template <>
struct MessageInterfaceProcessIdInfoBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageInterfaceIdInfoBaseT =
    typename MessageInterfaceProcessIdInfoBase<TOpt::HasMsgIdType && TOpt::HasMsgIdInfo>::template Type<TBase>;

//----------------------------------------------------

template <typename TBase, typename TReadIter>
class MessageInterfaceReadOnlyBase : public TBase
{
public:
    using ReadIterator = TReadIter;
    comms::ErrorStatus read(ReadIterator& iter, std::size_t size)
    {
        return this->readImpl(iter, size);
    }

    template <typename TIter>
    static comms::ErrorStatus doRead(TIter& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return comms::ErrorStatus::NotSupported;
    }

protected:
    ~MessageInterfaceReadOnlyBase() noexcept = default;
    virtual comms::ErrorStatus readImpl(ReadIterator& iter, std::size_t size)
    {
        return doRead(iter, size);
    }
};

template <typename TBase, typename TWriteIter>
class MessageInterfaceWriteOnlyBase : public TBase
{
public:
    using WriteIterator = TWriteIter;
    comms::ErrorStatus write(WriteIterator& iter, std::size_t size) const
    {
        return this->writeImpl(iter, size);
    }

    template <typename TIter>
    static comms::ErrorStatus doWrite(TIter& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return comms::ErrorStatus::NotSupported;
    }

protected:
    ~MessageInterfaceWriteOnlyBase() noexcept = default;
    virtual comms::ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const
    {
        return doWrite(iter, size);
    }
};

template <typename TBase, typename TReadIter, typename TWriteIter>
class MessageInterfaceReadWriteBase : public TBase
{
public:
    using ReadIterator = TReadIter;
    comms::ErrorStatus read(ReadIterator& iter, std::size_t size)
    {
        return this->readImpl(iter, size);
    }

    using WriteIterator = TWriteIter;
    comms::ErrorStatus write(WriteIterator& iter, std::size_t size) const
    {
        return this->writeImpl(iter, size);
    }

protected:
    ~MessageInterfaceReadWriteBase() noexcept = default;
    virtual comms::ErrorStatus readImpl(ReadIterator& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return comms::ErrorStatus::NotSupported;
    }

    virtual comms::ErrorStatus writeImpl(WriteIterator& iter, std::size_t size) const
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return comms::ErrorStatus::NotSupported;
    }
};

template <bool THasReadIterator, bool THasWriteIterator>
struct MessageInterfaceProcessReadWriteBase;

template <>
struct MessageInterfaceProcessReadWriteBase<false, false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <>
struct MessageInterfaceProcessReadWriteBase<false, true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageInterfaceWriteOnlyBase<TBase, typename TOpt::WriteIterator>;
};

template <>
struct MessageInterfaceProcessReadWriteBase<true, false>
{
    template <typename TBase, typename TOpt>
    using Type = MessageInterfaceReadOnlyBase<TBase, typename TOpt::ReadIterator>;
};

template <>
struct MessageInterfaceProcessReadWriteBase<true, true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageInterfaceReadWriteBase<TBase, typename TOpt::ReadIterator, typename TOpt::WriteIterator>;
};

template <typename TBase, typename TOpt>
using MessageInterfaceReadWriteBaseT =
    typename MessageInterfaceProcessReadWriteBase<TOpt::HasReadIterator, TOpt::HasWriteIterator>::template Type<TBase, TOpt>;

//----------------------------------------------------

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
            typename std::conditional<
                std::is_void<DispatchRetType>::value,
                VoidHandleRetTypeTag,
                NonVoidHandleRetTypeTag
            >::type;
        return dispatchInternal(Tag());
    }

private:
    struct VoidHandleRetTypeTag {};
    struct NonVoidHandleRetTypeTag {};

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

template <bool THasHandler>
struct MessageInterfaceProcessHandlerBase;

template <>
struct MessageInterfaceProcessHandlerBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageInterfaceHandlerBase<TBase, typename TOpt::Handler>;
};

template <>
struct MessageInterfaceProcessHandlerBase<false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageInterfaceHandlerBaseT =
    typename MessageInterfaceProcessHandlerBase<TOpt::HasHandler>::template Type<TBase, TOpt>;

//----------------------------------------------------

template <typename TBase>
class MessageInterfaceValidBase : public TBase
{
public:
    bool valid() const
    {
        return validImpl();
    }

    static constexpr bool doValid()
    {
        return true;
    }

protected:
    ~MessageInterfaceValidBase() noexcept = default;
    virtual bool validImpl() const
    {
        return doValid();
    }
};


template <bool THasValid>
struct MessageInterfaceProcessValidBase;

template <>
struct MessageInterfaceProcessValidBase<true>
{
    template <typename TBase>
    using Type = MessageInterfaceValidBase<TBase>;
};

template <>
struct MessageInterfaceProcessValidBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpts>
using MessageInterfaceValidBaseT =
    typename MessageInterfaceProcessValidBase<TOpts::HasValid>::template Type<TBase>;

//----------------------------------------------------

template <typename TBase>
class MessageInterfaceLengthBase : public TBase
{
public:
    std::size_t length() const
    {
        return lengthImpl();
    }

    static std::size_t doLength()
    {
        COMMS_ASSERT(!"Not overridden");
        return 0;
    }

protected:
    ~MessageInterfaceLengthBase() noexcept = default;
    virtual std::size_t lengthImpl() const
    {
        return doLength();
    }
};

template <bool THasLength>
struct MessageInterfaceProcessLengthBase;

template <>
struct MessageInterfaceProcessLengthBase<true>
{
    template <typename TBase>
    using Type = MessageInterfaceLengthBase<TBase>;
};

template <>
struct MessageInterfaceProcessLengthBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpts>
using MessageInterfaceLengthBaseT =
    typename MessageInterfaceProcessLengthBase<TOpts::HasLength>::template Type<TBase>;

//----------------------------------------------------

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

template <bool THasRefresh>
struct MessageInterfaceProcessRefreshBase;

template <>
struct MessageInterfaceProcessRefreshBase<true>
{
    template <typename TBase>
    using Type = MessageInterfaceRefreshBase<TBase>;
};

template <>
struct MessageInterfaceProcessRefreshBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpts>
using MessageInterfaceRefreshBaseT =
    typename MessageInterfaceProcessRefreshBase<TOpts::HasRefresh>::template Type<TBase>;

//----------------------------------------------------

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

template <bool THasName>
struct MessageInterfaceProcessNameBase;

template <>
struct MessageInterfaceProcessNameBase<true>
{
    template <typename TBase>
    using Type = MessageInterfaceNameBase<TBase>;
};

template <>
struct MessageInterfaceProcessNameBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpts>
using MessageInterfaceNameBaseT =
    typename MessageInterfaceProcessNameBase<TOpts::HasName>::template Type<TBase>;

//----------------------------------------------------

template <typename TOpts>
constexpr bool messageInterfaceHasVirtualFunctions()
{
    return
        TOpts::HasReadIterator ||
        TOpts::HasWriteIterator ||
        TOpts::HasMsgIdInfo ||
        TOpts::HasHandler ||
        TOpts::HasValid ||
        TOpts::HasLength ||
        TOpts::HasRefresh ||
        TOpts::HasName;
}

template <typename TBase>
class MessageInterfaceVirtDestructorBase : public TBase
{
protected:
    virtual ~MessageInterfaceVirtDestructorBase() noexcept = default;
};

template <bool THasVirtDestructor>
struct MessageInterfaceProcessVirtDestructorBase;

template <>
struct MessageInterfaceProcessVirtDestructorBase<true>
{
    template <typename TBase>
    using Type = MessageInterfaceVirtDestructorBase<TBase>;
};

template <>
struct MessageInterfaceProcessVirtDestructorBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpts>
using MessageInterfaceVirtDestructorBaseT =
    typename MessageInterfaceProcessVirtDestructorBase<
        (!TOpts::HasNoVirtualDestructor) && messageInterfaceHasVirtualFunctions<TOpts>()
    >::template Type<TBase>;

//----------------------------------------------------

template <typename... TOptions>
class MessageInterfaceBuilder
{
    using ParsedOptions = MessageInterfaceOptionsParser<TOptions...>;

    static_assert((!ParsedOptions::HasVersionInExtraTransportFields) || ParsedOptions::HasExtraTransportFields,
        "comms::option::def::VersionInExtraTransportFields option should not be used "
        "without comms::option::def::ExtraTransportFields.");

    using EndianBase = MessageInterfaceEndianBaseT<ParsedOptions>;
    using IdTypeBase = MessageInterfaceIdTypeBaseT<EndianBase, ParsedOptions>;
    using TransportFieldsBase = MessageInterfaceExtraTransportFieldsBaseT<IdTypeBase, ParsedOptions>;
    using VersionInTransportFieldsBase = MessageInterfaceVersionInExtraTransportFieldsBaseT<TransportFieldsBase, ParsedOptions>;
    using IdInfoBase = MessageInterfaceIdInfoBaseT<VersionInTransportFieldsBase, ParsedOptions>;
    using ReadWriteBase = MessageInterfaceReadWriteBaseT<IdInfoBase, ParsedOptions>;
    using ValidBase = MessageInterfaceValidBaseT<ReadWriteBase, ParsedOptions>;
    using LengthBase = MessageInterfaceLengthBaseT<ValidBase, ParsedOptions>;
    using HandlerBase = MessageInterfaceHandlerBaseT<LengthBase, ParsedOptions>;
    using RefreshBase = MessageInterfaceRefreshBaseT<HandlerBase, ParsedOptions>;
    using NameBase = MessageInterfaceNameBaseT<RefreshBase, ParsedOptions>;
    using VirtDestructorBase = MessageInterfaceVirtDestructorBaseT<NameBase, ParsedOptions>;

public:
    using Options = ParsedOptions;
    using Type = VirtDestructorBase;
};

template <typename... TOptions>
using MessageInterfaceBuilderT =
    typename MessageInterfaceBuilder<TOptions...>::Type;

}  // namespace details

}  // namespace comms


