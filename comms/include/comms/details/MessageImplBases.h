//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <tuple>
#include <cstddef>

#include "comms/util/Tuple.h"
#include "comms/details/tag.h"
#include "comms/field/basic/CommonFuncs.h"
#include "comms/field/details/FieldOpHelpers.h"

namespace comms
{

namespace details
{

template <typename TAllFields>
class MessageImplFieldsContainer;

template <typename... TAllFields>
class MessageImplFieldsContainer<std::tuple<TAllFields...> >    
{
public:
    using AllFields = std::tuple<TAllFields...>;

    AllFields& fields()
    {
        return fields_;
    }

    const AllFields& fields() const
    {
        return fields_;
    }

    static constexpr bool areFieldsVersionDependent()
    {
        return comms::field::basic::CommonFuncs::IsAnyFieldVersionDependentBoolType<TAllFields...>::value;
    }

    static constexpr bool doFieldsHaveNonDefaultRefresh()
    {
        return comms::field::basic::CommonFuncs::AnyFieldHasNonDefaultRefreshBoolType<TAllFields...>::value;
    }    

    template <typename TIter>
    comms::ErrorStatus doRead(TIter& iter, std::size_t size)
    {
        using Tag =
            typename comms::util::LazyShallowConditional<
                comms::field::basic::CommonFuncs::AllFieldsHaveReadNoStatusBoolType<TAllFields...>::value
            >::template Type<
                NoStatusTag,
                UseStatusTag
            >;

        return doReadInternal(iter, size, Tag());
    }

    template <typename TIter>
    comms::ErrorStatus doWrite(
        TIter& iter,
        std::size_t size) const
    {

        using Tag =
            typename comms::util::LazyShallowConditional<
                comms::field::basic::CommonFuncs::AllFieldsHaveWriteNoStatusBoolType<TAllFields...>::value
            >::template Type<
                NoStatusTag,
                UseStatusTag
            >;

        return doWriteInternal(iter, size, Tag());
    }

    bool doValid() const
    {
        return util::tupleAccumulate(fields(), true, comms::field::details::FieldValidCheckHelper<>());
    }

    std::size_t doLength() const
    {
        return util::tupleAccumulate(fields(), static_cast<std::size_t>(0U), comms::field::details::FieldLengthSumCalcHelper<>());
    }

    template <std::size_t TFromIdx>
    std::size_t doLengthFrom() const
    {
        return
            util::tupleAccumulateFromUntil<TFromIdx, std::tuple_size<AllFields>::value>(
                fields(), static_cast<std::size_t>(0U), comms::field::details::FieldLengthSumCalcHelper<>());
    }

    template <std::size_t TUntilIdx>
    std::size_t doLengthUntil() const
    {
        return
            util::tupleAccumulateFromUntil<0, TUntilIdx>(
                fields(), static_cast<std::size_t>(0U), comms::field::details::FieldLengthSumCalcHelper<>());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    std::size_t doLengthFromUntil() const
    {
        return
            util::tupleAccumulateFromUntil<TFromIdx, TUntilIdx>(
                fields(), static_cast<std::size_t>(0U), comms::field::details::FieldLengthSumCalcHelper<>());
    }

    static constexpr std::size_t doMinLength()
    {
        return comms::field::basic::CommonFuncs::FieldSumMinLengthIntType<TAllFields...>::value;
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t doMinLengthFrom()
    {
        return 
            comms::field::basic::CommonFuncs::FieldSumMinLengthFromUntilIntType<
                TFromIdx, 
                std::tuple_size<AllFields>::value,
                TAllFields...
            >::value;
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t doMinLengthUntil()
    {
        return 
            comms::field::basic::CommonFuncs::FieldSumMinLengthFromUntilIntType<
                0U,
                TUntilIdx, 
                TAllFields...
            >::value;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t doMinLengthFromUntil()
    {
        return 
            comms::field::basic::CommonFuncs::FieldSumMinLengthFromUntilIntType<
                TFromIdx,
                TUntilIdx, 
                TAllFields...
            >::value;
    }

    static constexpr std::size_t doMaxLength()
    {
        return comms::field::basic::CommonFuncs::FieldSumMaxLengthIntType<TAllFields...>::value;
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t doMaxLengthFrom()
    {
        return 
            comms::field::basic::CommonFuncs::FieldSumMaxLengthFromUntilIntType<
                TFromIdx, 
                std::tuple_size<AllFields>::value,
                TAllFields...
            >::value;
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t doMaxLengthUntil()
    {
        return 
            comms::field::basic::CommonFuncs::FieldSumMaxLengthFromUntilIntType<
                0U,
                TUntilIdx, 
                TAllFields...
            >::value;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t doMaxLengthFromUntil()
    {
        return 
            comms::field::basic::CommonFuncs::FieldSumMaxLengthFromUntilIntType<
                TFromIdx,
                TUntilIdx, 
                TAllFields...
            >::value;
    }

    bool doRefresh()
    {
        return util::tupleAccumulate(fields(), false, comms::field::details::FieldRefreshHelper<>());
    }

protected:
    ~MessageImplFieldsContainer() noexcept = default;

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doReadUntil(
        TIter& iter,
        std::size_t len)
    {
        return doReadUntilAndUpdateLen<TIdx>(iter, len);
    }    

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doReadUntilAndUpdateLen(
        TIter& iter,
        std::size_t& len)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachUntil<TIdx>(fields(), makeFieldReader(iter, status, len));
        return status;
    }

    template <std::size_t TIdx, typename TIter>
    void doReadNoStatusUntil(TIter& iter)
    {
        util::tupleForEachUntil<TIdx>(fields(), makeFieldNoStatusReader(iter));
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doReadFrom(
        TIter& iter,
        std::size_t len)
    {
        return doReadFromAndUpdateLen<TIdx>(iter, len);
    }    

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doReadFromAndUpdateLen(
        TIter& iter,
        std::size_t& len)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFrom<TIdx>(fields(), makeFieldReader(iter, status, len));
        return status;
    }

    template <std::size_t TIdx, typename TIter>
    void doReadNoStatusFrom(TIter& iter)
    {
        util::tupleForEachFrom<TIdx>(fields(), makeFieldNoStatusReader(iter));
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus doReadFromUntil(
        TIter& iter,
        std::size_t len)
    {
        return doReadFromUntilAndUpdateLen<TFromIdx, TUntilIdx>(iter, len);
    }    

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus doReadFromUntilAndUpdateLen(
        TIter& iter,
        std::size_t& len)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldReader(iter, status, len));
        return status;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void doReadNoStatusFromUntil(TIter& iter)
    {
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldNoStatusReader(iter));
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doWriteUntil(
        TIter& iter,
        std::size_t len) const
    {
        return doWriteUntilAndUpdateLen<TIdx>(iter, len);
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doWriteUntilAndUpdateLen(
        TIter& iter,
        std::size_t& len) const
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachUntil<TIdx>(fields(), makeFieldWriter(iter, status, len));
        return status;
    }    

    template <std::size_t TIdx, typename TIter>
    void doWriteNoStatusUntil(TIter& iter) const
    {
        util::tupleForEachUntil<TIdx>(fields(), makeFieldNoStatusWriter(iter));
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doWriteFrom(
        TIter& iter,
        std::size_t len) const
    {
        return doWriteFromAndUpdateLen<TIdx>(iter, len);
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus doWriteFromAndUpdateLen(
        TIter& iter,
        std::size_t& len) const
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFrom<TIdx>(fields(), makeFieldWriter(iter, status, len));
        return status;
    }    

    template <std::size_t TIdx, typename TIter>
    void doWriteNoStatusFrom(TIter& iter) const
    {
        util::tupleForEachFrom<TIdx>(fields(), makeFieldNoStatusWriter(iter));
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus doWriteFromUntil(
        TIter& iter,
        std::size_t len) const
    {
        return doWriteFromUntilAndUpdateLen<TFromIdx, TUntilIdx>(iter, len);
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus doWriteFromUntilAndUpdateLen(
        TIter& iter,
        std::size_t& len) const
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldWriter(iter, status, len));
        return status;
    }    

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void doWriteNoStatusFromUntil(TIter& iter) const
    {
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldNoStatusWriter(iter));
    }

private:
    template <typename... TParams>
    using NoStatusTag = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using UseStatusTag = comms::details::tag::Tag2<TParams...>;

    template <typename TIter, typename... TParams>
    comms::ErrorStatus doReadInternal(
        TIter& iter,
        std::size_t size,
        UseStatusTag<TParams...>)
    {
        return doReadFromAndUpdateLen<0>(iter, size);
    }

    template <typename TIter, typename... TParams>
    comms::ErrorStatus doReadInternal(
        TIter& iter,
        std::size_t size,
        NoStatusTag<TParams...>)
    {
        if (size < doLength()) {
            return comms::ErrorStatus::NotEnoughData;
        }

        doReadNoStatusFrom<0>(iter);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter, typename... TParams>
    comms::ErrorStatus doWriteInternal(
        TIter& iter,
        std::size_t size,
        UseStatusTag<TParams...>) const
    {
        return doWriteFromAndUpdateLen<0>(iter, size);
    }

    template <typename TIter, typename... TParams>
    comms::ErrorStatus doWriteInternal(
        TIter& iter,
        std::size_t size,
        NoStatusTag<TParams...>) const
    {
        if (size < doLength()) {
            return comms::ErrorStatus::BufferOverflow;
        }

        doWriteNoStatusFrom<0>(iter);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    static comms::field::details::FieldReadHelper<TIter> makeFieldReader(
        TIter& iter,
        comms::ErrorStatus& status,
        std::size_t& size)
    {
        return comms::field::details::FieldReadHelper<TIter>(status, iter, size);
    }

    template <typename TIter>
    static comms::field::details::FieldReadNoStatusHelper<TIter> makeFieldNoStatusReader(TIter& iter)
    {
        return comms::field::details::FieldReadNoStatusHelper<TIter>(iter);
    }

    template <typename TIter>
    static comms::field::details::FieldWriteHelper<TIter> makeFieldWriter(
        TIter& iter,
        comms::ErrorStatus& status,
        std::size_t& size)
    {
        return comms::field::details::FieldWriteHelper<TIter>(status, iter, size);
    }

    template <typename TIter>
    static comms::field::details::FieldWriteNoStatusHelper<TIter> makeFieldNoStatusWriter(TIter& iter)
    {
        return comms::field::details::FieldWriteNoStatusHelper<TIter>(iter);
    }

    AllFields fields_;
};

// ------------------------------------------------------

template <typename TBase, typename TAllFields>
class MessageImplFieldsBase : public TBase, public MessageImplFieldsContainer<TAllFields>
{
    using ContainerBase = MessageImplFieldsContainer<TAllFields>;
public:
    using ContainerBase::doRead;
    using ContainerBase::doWrite;
    using ContainerBase::doLength;
    using ContainerBase::doValid;
    using ContainerBase::doRefresh;
    using ContainerBase::doLengthFrom;
    using ContainerBase::doLengthUntil;
    using ContainerBase::doLengthFromUntil;
    using ContainerBase::doMinLength;
    using ContainerBase::doMinLengthFrom;
    using ContainerBase::doMinLengthUntil;
    using ContainerBase::doMinLengthFromUntil;
    using ContainerBase::doMaxLength;
    using ContainerBase::doMaxLengthFrom;
    using ContainerBase::doMaxLengthUntil;
    using ContainerBase::doMaxLengthFromUntil;
    using ContainerBase::areFieldsVersionDependent;

protected:
    ~MessageImplFieldsBase() noexcept = default;

    using ContainerBase::doReadUntil;
    using ContainerBase::doReadUntilAndUpdateLen;
    using ContainerBase::doReadNoStatusUntil;
    using ContainerBase::doReadFrom;
    using ContainerBase::doReadFromAndUpdateLen;
    using ContainerBase::doReadNoStatusFrom;
    using ContainerBase::doReadFromUntil;
    using ContainerBase::doReadFromUntilAndUpdateLen;
    using ContainerBase::doReadNoStatusFromUntil;
    using ContainerBase::doWriteUntil;
    using ContainerBase::doWriteUntilAndUpdateLen;
    using ContainerBase::doWriteNoStatusUntil;
    using ContainerBase::doWriteFrom;
    using ContainerBase::doWriteFromAndUpdateLen;
    using ContainerBase::doWriteNoStatusFrom;
    using ContainerBase::doWriteFromUntil;
    using ContainerBase::doWriteFromUntilAndUpdateLen;
    using ContainerBase::doWriteNoStatusFromUntil;
};

// ------------------------------------------------------

template <typename TBase>
class MessageImplVersionBase : public TBase
{
public:
    using VersionType = typename TBase::VersionType;

    bool doFieldsVersionUpdate()
    {
        return comms::field::basic::CommonFuncs::setVersionForMembers(TBase::fields(), TBase::version());
    }

    template <typename TIter>
    comms::ErrorStatus doRead(TIter& iter, std::size_t len)
    {
        doFieldsVersionUpdate();
        return TBase::doRead(iter, len);
    }

    bool doRefresh()
    {
        bool updated = doFieldsVersionUpdate();
        return TBase::doRefresh() || updated;
    }

protected:
    MessageImplVersionBase()
    {
        doFieldsVersionUpdate();
    }

    MessageImplVersionBase(const MessageImplVersionBase&) = default;
    MessageImplVersionBase(MessageImplVersionBase&&) = default;
    ~MessageImplVersionBase() noexcept = default;

    MessageImplVersionBase& operator=(const MessageImplVersionBase&) = default;
    MessageImplVersionBase& operator=(MessageImplVersionBase&&) = default;
};

// ------------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplFieldsReadImplBase : public TBase
{
    using BaseImpl = TBase;
protected:
    ~MessageImplFieldsReadImplBase() noexcept = default;
    virtual comms::ErrorStatus readImpl(
        typename BaseImpl::ReadIterator& iter,
        std::size_t size) override
    {
        using Tag = 
            typename comms::util::LazyShallowConditional<
                std::is_same<TActual, void>::value
            >::template Type<
                NoActual,
                HasActual
            >;        
        return readImplInternal(iter, size, Tag());
    }

private:
    template <typename... TParams>
    using HasActual = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using NoActual = comms::details::tag::Tag2<TParams...>;

    template <typename... TParams>
    comms::ErrorStatus readImplInternal(
        typename BaseImpl::ReadIterator& iter,
        std::size_t size,
        NoActual<TParams...>)
    {
        return BaseImpl::doRead(iter, size);
    }

    template <typename... TParams>
    comms::ErrorStatus readImplInternal(
        typename BaseImpl::ReadIterator& iter,
        std::size_t size,
        HasActual<TParams...>)
    {
        return static_cast<TActual*>(this)->doRead(iter, size);
    }
};

// ------------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplFieldsWriteImplBase : public TBase
{
    using BaseImpl = TBase;

protected:
    ~MessageImplFieldsWriteImplBase() noexcept = default;
    virtual comms::ErrorStatus writeImpl(
        typename BaseImpl::WriteIterator& iter,
        std::size_t size) const override
    {
        using Tag = 
            typename comms::util::LazyShallowConditional<
                std::is_same<TActual, void>::value
            >::template Type<
                NoActual,
                HasActual
            >;        
        return writeImplInternal(iter, size, Tag());
    }

private:
    template <typename... TParams>
    using HasActual = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using NoActual = comms::details::tag::Tag2<TParams...>;

    template <typename... TParams>
    comms::ErrorStatus writeImplInternal(
        typename BaseImpl::WriteIterator& iter,
        std::size_t size,
        NoActual<TParams...>) const
    {
        return BaseImpl::doWrite(iter, size);
    }

    template <typename... TParams>
    comms::ErrorStatus writeImplInternal(
        typename BaseImpl::WriteIterator& iter,
        std::size_t size,
        HasActual<TParams...>) const
    {
        return static_cast<const TActual*>(this)->doWrite(iter, size);
    }
};

// ------------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplFieldsValidBase : public TBase
{
    using BaseImpl = TBase;

protected:
    ~MessageImplFieldsValidBase() noexcept = default;
    virtual bool validImpl() const override
    {
        using Tag = 
            typename comms::util::LazyShallowConditional<
                std::is_same<TActual, void>::value
            >::template Type<
                NoActual,
                HasActual
            >;
        return validImplInternal(Tag());
    }

private:
    template <typename... TParams>
    using HasActual = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using NoActual = comms::details::tag::Tag2<TParams...>;

    template <typename... TParams>
    bool validImplInternal(NoActual<TParams...>) const
    {
        return BaseImpl::doValid();
    }

    template <typename... TParams>
    bool validImplInternal(HasActual<TParams...>) const
    {
        return static_cast<const TActual*>(this)->doValid();
    }
};

// ------------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplFieldsLengthBase : public TBase
{
    using BaseImpl = TBase;

protected:
    ~MessageImplFieldsLengthBase() noexcept = default;
    virtual std::size_t lengthImpl() const override
    {
        using Tag = 
            typename comms::util::LazyShallowConditional<
                std::is_same<TActual, void>::value
            >::template Type<
                NoActual,
                HasActual
            >;        
        return lengthImplInternal(Tag());
    }

private:
    template <typename... TParams>
    using HasActual = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using NoActual = comms::details::tag::Tag2<TParams...>;

    template <typename... TParams>
    std::size_t lengthImplInternal(NoActual<TParams...>) const
    {
        return BaseImpl::doLength();
    }

    template <typename... TParams>
    std::size_t lengthImplInternal(HasActual<TParams...>) const
    {
        return static_cast<const TActual*>(this)->doLength();
    }
};

// ------------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplRefreshBase : public TBase
{
protected:
    ~MessageImplRefreshBase() noexcept = default;
    virtual bool refreshImpl() override
    {
        using Tag =
            typename comms::util::LazyShallowConditional<
                std::is_same<TActual, void>::value
            >::template Type<
                NoDowncast,
                Downcast
            >;
        return refreshInternal(Tag());
    }

private:
    template <typename... TParams>
    using Downcast = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using NoDowncast = comms::details::tag::Tag2<TParams...>;

    template <typename... TParams>
    bool refreshInternal(Downcast<TParams...>)
    {
        return static_cast<TActual*>(this)->doRefresh();
    }

    template <typename... TParams>
    bool refreshInternal(NoDowncast<TParams...>)
    {
        return TBase::doRefresh();
    }
};

// ------------------------------------------------------

template <typename TBase, typename TActual>
class MessageImplDispatchBase : public TBase
{
    using BaseImpl = TBase;
protected:
    ~MessageImplDispatchBase() noexcept = default;
    virtual typename TBase::DispatchRetType dispatchImpl(typename TBase::Handler& handler) override
    {
        static_assert(std::is_base_of<TBase, TActual>::value,
            "TActual is not derived class");
        return handler.handle(static_cast<TActual&>(*this));
    }
};

// ------------------------------------------------------

template <typename TBase, std::intmax_t TId>
class MessageImplStaticNumIdBase : public TBase
{
public:
    static const typename TBase::MsgIdType MsgId =
            static_cast<typename TBase::MsgIdType>(TId);

    static constexpr typename TBase::MsgIdParamType doGetId()
    {
        return MsgId;
    }

protected:
    ~MessageImplStaticNumIdBase() noexcept = default;
};

// ------------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplPolymorhpicStaticNumIdBase : public TBase
{
protected:
    ~MessageImplPolymorhpicStaticNumIdBase() noexcept = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        using Tag =
            typename comms::util::LazyShallowConditional<
                std::is_same<TActual, void>::value
            >::template Type<
                NoDowncastTag,
                DowncastTag
            >;
        return getIdInternal(Tag());
    }

private:
    template <typename... TParams>
    using DowncastTag = comms::details::tag::Tag1<TParams...>;

    template <typename... TParams>
    using NoDowncastTag = comms::details::tag::Tag2<TParams...>;

    template <typename... TParams>
    typename TBase::MsgIdParamType getIdInternal(NoDowncastTag<TParams...>) const
    {
        return TBase::doGetId();
    }

    template <typename... TParams>
    typename TBase::MsgIdParamType getIdInternal(DowncastTag<TParams...>) const
    {
        return static_cast<const TActual*>(this)->doGetId();
    }
};

// ------------------------------------------------------

template <typename TBase>
class MessageImplNoIdBase : public TBase
{
protected:
    ~MessageImplNoIdBase() noexcept = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        static const typename TBase::MsgIdType MsgId = typename TBase::MsgIdType();
        COMMS_ASSERT(!"The message id is not supposed to be retrieved");
        return MsgId;
    }
};

// ------------------------------------------------------

template <typename TBase, typename TActual>
class MessageImplNameBase : public TBase
{
protected:
    ~MessageImplNameBase() noexcept = default;
    virtual const char* nameImpl() const override
    {
        return static_cast<const TActual*>(this)->doName();
    }
};


} // namespace details

} // namespace comms
