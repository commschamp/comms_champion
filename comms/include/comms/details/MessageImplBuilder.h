//
// Copyright 2015 - 2017 (C). Alex Robenko. All rights reserved.
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
#include "MessageImplOptionsParser.h"

namespace comms
{

namespace details
{

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

template <bool THasStaticMsgId>
struct MessageImplProcessStaticNumIdBase;

template <>
struct MessageImplProcessStaticNumIdBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplStaticNumIdBase<TBase, TOpt::MsgId>;
};

template <>
struct MessageImplProcessStaticNumIdBase<false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageImplStaticNumIdBaseT =
    typename MessageImplProcessStaticNumIdBase<
        TBase::InterfaceOptions::HasMsgIdType && TOpt::HasStaticMsgId>::template Type<TBase, TOpt>;

template <typename TBase, typename TOpt>
class MessageImplPolymorhpicStaticNumIdBase : public TBase
{
protected:
    ~MessageImplPolymorhpicStaticNumIdBase() noexcept = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        using Tag =
            typename std::conditional<
                TOpt::HasMsgType,
                DowncastTag,
                NoDowncastTag
            >::type;
        return getIdInternal(Tag());
    }

private:
    struct DowncastTag {};
    struct NoDowncastTag {};

    typename TBase::MsgIdParamType getIdInternal(NoDowncastTag) const
    {
        return TBase::doGetId();
    }

    typename TBase::MsgIdParamType getIdInternal(DowncastTag) const
    {
        using Derived = typename TOpt::MsgType;
        return static_cast<const Derived*>(this)->doGetId();
    }
};

template <bool THasStaticMsgId>
struct MessageImplProcessPolymorhpicStaticNumIdBase;

template <>
struct MessageImplProcessPolymorhpicStaticNumIdBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplPolymorhpicStaticNumIdBase<TBase, TOpt>;
};

template <>
struct MessageImplProcessPolymorhpicStaticNumIdBase<false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageImplPolymorhpicStaticNumIdBaseT =
    typename MessageImplProcessPolymorhpicStaticNumIdBase<
        TBase::InterfaceOptions::HasMsgIdType && TBase::InterfaceOptions::HasMsgIdInfo &&
            (TOpt::HasStaticMsgId || (TOpt::HasMsgType && TOpt::HasDoGetId))
        >::template Type<TBase, TOpt>;

template <typename TBase>
class MessageImplNoIdBase : public TBase
{
protected:
    ~MessageImplNoIdBase() noexcept = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        static const typename TBase::MsgIdType MsgId = typename TBase::MsgIdType();
        GASSERT(!"The message id is not supposed to be retrieved");
        return MsgId;
    }
};

template <bool THasNoId>
struct MessageImplProcessNoIdBase;

template <>
struct MessageImplProcessNoIdBase<true>
{
    template <typename TBase>
    using Type = MessageImplNoIdBase<TBase>;
};

template <>
struct MessageImplProcessNoIdBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageImplNoIdBaseT =
    typename MessageImplProcessNoIdBase<
        TBase::InterfaceOptions::HasMsgIdType && TBase::InterfaceOptions::HasMsgIdInfo &&
            TOpt::HasNoIdImpl
        >::template Type<TBase>;

template <typename TBase, typename TAllFields>
class MessageImplFieldsBase : public TBase
{
public:
    using AllFields = TAllFields;

    AllFields& fields()
    {
        return fields_;
    }

    const AllFields& fields() const
    {
        return fields_;
    }

    template <typename TIter>
    comms::ErrorStatus doRead(TIter& iter, std::size_t size)
    {
        return doReadInternal(iter, size, StatusTag());
    }

    template <typename TIter>
    comms::ErrorStatus doWrite(
        TIter& iter,
        std::size_t size) const
    {
        if (size < doLength()) {
            return comms::ErrorStatus::BufferOverflow;
        }

        writeFieldsNoStatusFrom<0>(iter);
        return comms::ErrorStatus::Success;
    }

    bool doValid() const
    {
        return util::tupleAccumulate(fields(), true, FieldValidityRetriever());
    }

    std::size_t doLength() const
    {
        return util::tupleAccumulate(fields(), 0U, FieldLengthRetriever());
    }

    template <std::size_t TFromIdx>
    std::size_t doLengthFrom() const
    {
        return
            util::tupleAccumulateFromUntil<TFromIdx, std::tuple_size<AllFields>::value>(
                fields(), 0U, FieldLengthRetriever());
    }

    template <std::size_t TUntilIdx>
    std::size_t doLengthUntil() const
    {
        return
            util::tupleAccumulateFromUntil<0, TUntilIdx>(
                fields(), 0U, FieldLengthRetriever());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    std::size_t doLengthFromUntil() const
    {
        return
            util::tupleAccumulateFromUntil<TFromIdx, TUntilIdx>(
                fields(), 0U, FieldLengthRetriever());
    }

    bool doRefresh() const
    {
        return util::tupleAccumulate(fields(), false, FieldRefresher());
    }

protected:
    ~MessageImplFieldsBase() noexcept = default;

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus readFieldsUntil(
        TIter& iter,
        std::size_t& size)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachUntil<TIdx>(fields(), makeFieldReader(iter, status, size));
        return status;
    }

    template <std::size_t TIdx, typename TIter>
    void readFieldsNoStatusUntil(TIter& iter)
    {
        util::tupleForEachUntil<TIdx>(fields(), makeFieldNoStatusReader(iter));
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus readFieldsFrom(
        TIter& iter,
        std::size_t& size)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFrom<TIdx>(fields(), makeFieldReader(iter, status, size));
        return status;
    }

    template <std::size_t TIdx, typename TIter>
    void readFieldsNoStatusFrom(TIter& iter)
    {
        util::tupleForEachFrom<TIdx>(fields(), makeFieldNoStatusReader(iter));
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus readFieldsFromUntil(
        TIter& iter,
        std::size_t& size)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldReader(iter, status, size));
        return status;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void readFieldsNoStatusFromUntil(TIter& iter)
    {
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldNoStatusReader(iter));
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus writeFieldsUntil(
        TIter& iter,
        std::size_t size) const
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEachUntil<TIdx>(fields(), makeFieldWriter(iter, status, remainingSize));
        return status;
    }

    template <std::size_t TIdx, typename TIter>
    void writeFieldsNoStatusUntil(TIter& iter) const
    {
        util::tupleForEachUntil<TIdx>(fields(), makeFieldNoStatusWriter(iter));
    }

    template <std::size_t TIdx, typename TIter>
    comms::ErrorStatus writeFieldsFrom(
        TIter& iter,
        std::size_t size) const
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEachFrom<TIdx>(fields(), makeFieldWriter(iter, status, remainingSize));
        return status;
    }

    template <std::size_t TIdx, typename TIter>
    void writeFieldsNoStatusFrom(TIter& iter) const
    {
        util::tupleForEachFrom<TIdx>(fields(), makeFieldNoStatusWriter(iter));
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    comms::ErrorStatus writeFieldsFromUntil(
        TIter& iter,
        std::size_t size) const
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldWriter(iter, status, remainingSize));
        return status;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void writeFieldsNoStatusFromUntil(TIter& iter) const
    {
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(fields(), makeFieldNoStatusWriter(iter));
    }

private:
    struct NoStatusTag {};
    struct UseStatusTag {};

    struct NoStatusDetector
    {
        constexpr NoStatusDetector() = default;

        template <typename TField>
        constexpr bool operator()(bool soFar) const
        {
            return
                (TField::minLength() == TField::maxLength()) &&
                (!TField::ParsedOptions::HasCustomValueReader) &&
                (!TField::ParsedOptions::HasFailOnInvalid) &&
                (!TField::ParsedOptions::HasSequenceSizeFieldPrefix)  &&
                (!TField::ParsedOptions::HasSequenceSerLengthFieldPrefix) &&
                (!TField::ParsedOptions::HasSequenceTrailingFieldSuffix) &&
                (!TField::ParsedOptions::HasSequenceTerminationFieldSuffix) &&
                soFar;
            ;
        }
    };

    using StatusTag =
        typename std::conditional<
            comms::util::tupleTypeAccumulate<AllFields>(true, NoStatusDetector()),
            NoStatusTag,
            UseStatusTag
        >::type;

    template <typename TIter>
    comms::ErrorStatus doReadInternal(
        TIter& iter,
        std::size_t size,
        UseStatusTag)
    {
        return readFieldsFrom<0>(iter, size);
    }

    template <typename TIter>
    comms::ErrorStatus doReadInternal(
        TIter& iter,
        std::size_t size,
        NoStatusTag)
    {
        if (size < doLength()) {
            return comms::ErrorStatus::NotEnoughData;
        }

        readFieldsNoStatusFrom<0>(iter);
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    class FieldReader
    {
    public:
        FieldReader(TIter& iter, comms::ErrorStatus& status, std::size_t& size)
            : iter_(iter),
              status_(status),
              size_(size)
        {
        }

        template <typename TField>
        void operator()(TField& field) {
            if (status_ == comms::ErrorStatus::Success) {
                status_ = field.read(iter_, size_);
                if (status_ == comms::ErrorStatus::Success) {
                    GASSERT(field.length() <= size_);
                    size_ -= field.length();
                }
            }
        }

    private:
        TIter& iter_;
        comms::ErrorStatus& status_;
        std::size_t& size_;
    };

    template <typename TIter>
    static FieldReader<TIter> makeFieldReader(
        TIter& iter,
        comms::ErrorStatus& status,
        std::size_t& size)
    {
        return FieldReader<TIter>(iter, status, size);
    }

    template <typename TIter>
    class FieldNoStatusReader
    {
    public:
        FieldNoStatusReader(TIter& iter)
            : iter_(iter)
        {
        }

        template <typename TField>
        void operator()(TField& field) {
            field.readNoStatus(iter_);
        }

    private:
        TIter& iter_;
    };

    template <typename TIter>
    static FieldNoStatusReader<TIter> makeFieldNoStatusReader(TIter& iter)
    {
        return FieldNoStatusReader<TIter>(iter);
    }

    template <typename TIter>
    class FieldWriter
    {
    public:
        FieldWriter(TIter& iter, comms::ErrorStatus& status, std::size_t& size)
            : iter_(iter),
              status_(status),
              size_(size)
        {
        }

        template <typename TField>
        void operator()(const TField& field) {
            if (status_ == comms::ErrorStatus::Success) {
                status_ = field.write(iter_, size_);
                if (status_ == comms::ErrorStatus::Success) {
                    GASSERT(field.length() <= size_);
                    size_ -= field.length();
                }
            }
        }

    private:
        TIter& iter_;
        comms::ErrorStatus& status_;
        std::size_t& size_;
    };

    template <typename TIter>
    static FieldWriter<TIter> makeFieldWriter(
        TIter& iter,
        comms::ErrorStatus& status,
        std::size_t& size)
    {
        return FieldWriter<TIter>(iter, status, size);
    }

    template <typename TIter>
    class FieldNoStatusWriter
    {
    public:
        FieldNoStatusWriter(TIter& iter)
            : iter_(iter)
        {
        }

        template <typename TField>
        void operator()(const TField& field) {
            field.writeNoStatus(iter_);
        }

    private:
        TIter& iter_;
    };

    template <typename TIter>
    static FieldNoStatusWriter<TIter> makeFieldNoStatusWriter(TIter& iter)
    {
        return FieldNoStatusWriter<TIter>(iter);
    }

    struct FieldValidityRetriever
    {
        template <typename TField>
        bool operator()(bool valid, const TField& field) const
        {
            return valid && field.valid();
        }
    };

    struct FieldRefresher
    {
        template <typename TField>
        bool operator()(bool refreshed, TField& field) const
        {
            return field.refreshed() || refreshed;
        }
    };


    struct FieldLengthRetriever
    {
        template <typename TField>
        std::size_t operator()(std::size_t size, const TField& field) const
        {
            return size + field.length();
        }
    };


private:
    AllFields fields_;
};

template <bool THasFieldsImpl>
struct MessageImplProcessFieldsBase;

template <>
struct MessageImplProcessFieldsBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsBase<TBase, typename TOpt::Fields>;
};

template <>
struct MessageImplProcessFieldsBase<false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageImplFieldsBaseT =
    typename MessageImplProcessFieldsBase<TOpt::HasFieldsImpl>::template Type<TBase, TOpt>;

template <typename TBase, typename TActual = void>
class MessageImplFieldsReadImplBase : public TBase
{
    using Base = TBase;
protected:
    ~MessageImplFieldsReadImplBase() noexcept = default;
    virtual comms::ErrorStatus readImpl(
        typename Base::ReadIterator& iter,
        std::size_t size) override
    {
        return readImplInternal(iter, size, Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    using Tag = typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type;

    comms::ErrorStatus readImplInternal(
        typename Base::ReadIterator& iter,
        std::size_t size,
        NoActual)
    {
        return Base::doRead(iter, size);
    }

    comms::ErrorStatus readImplInternal(
        typename Base::ReadIterator& iter,
        std::size_t size,
        HasActual)
    {
        return static_cast<TActual*>(this)->doRead(iter, size);
    }
};

template <bool THasFieldsReadImpl, bool THasMsgType>
struct MessageImplProcessFieldsReadImplBase;

template <>
struct MessageImplProcessFieldsReadImplBase<true, true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsReadImplBase<TBase, typename TOpt::MsgType>;
};

template <>
struct MessageImplProcessFieldsReadImplBase<true, false>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsReadImplBase<TBase>;
};

template <bool THasMsgType>
struct MessageImplProcessFieldsReadImplBase<false, THasMsgType>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsReadImplBaseT =
    typename MessageImplProcessFieldsReadImplBase<
        TBase::InterfaceOptions::HasReadIterator && (!TImplOpt::HasNoReadImpl),
        TImplOpt::HasMsgType
    >::template Type<TBase, TImplOpt>;

template <typename TBase, typename TActual = void>
class MessageImplFieldsWriteImplBase : public TBase
{
    using Base = TBase;

protected:
    ~MessageImplFieldsWriteImplBase() noexcept = default;
    virtual comms::ErrorStatus writeImpl(
        typename Base::WriteIterator& iter,
        std::size_t size) const override
    {
        return writeImplInternal(iter, size, Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    using Tag = typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type;

    comms::ErrorStatus writeImplInternal(
        typename Base::WriteIterator& iter,
        std::size_t size,
        NoActual) const
    {
        return Base::doWrite(iter, size);
    }

    comms::ErrorStatus writeImplInternal(
        typename Base::WriteIterator& iter,
        std::size_t size,
        HasActual) const
    {
        return static_cast<const TActual*>(this)->doWrite(iter, size);
    }
};

template <bool THasFieldsWriteImpl, bool THasMsgType>
struct MessageImplProcessFieldsWriteImplBase;

template <>
struct MessageImplProcessFieldsWriteImplBase<true, true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsWriteImplBase<TBase, typename TOpt::MsgType>;
};

template <>
struct MessageImplProcessFieldsWriteImplBase<true, false>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsWriteImplBase<TBase>;
};

template <bool THasMsgType>
struct MessageImplProcessFieldsWriteImplBase<false, THasMsgType>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsWriteImplBaseT =
    typename MessageImplProcessFieldsWriteImplBase<
        TBase::InterfaceOptions::HasWriteIterator && (!TImplOpt::HasNoWriteImpl),
        TImplOpt::HasMsgType
    >::template Type<TBase, TImplOpt>;

template <typename TBase, typename TActual = void>
class MessageImplFieldsValidBase : public TBase
{
    using Base = TBase;

protected:
    ~MessageImplFieldsValidBase() noexcept = default;
    virtual bool validImpl() const override
    {
        return validImplInternal(Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    using Tag = typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type;

    bool validImplInternal(NoActual) const
    {
        return Base::doValid();
    }

    bool validImplInternal(HasActual) const
    {
        return static_cast<const TActual*>(this)->doValid();
    }
};

template <bool THasFieldsValidImpl, bool THasMsgType>
struct MessageImplProcessFieldsValidBase;

template <>
struct MessageImplProcessFieldsValidBase<true, true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsValidBase<TBase, typename TOpt::MsgType>;
};

template <>
struct MessageImplProcessFieldsValidBase<true, false>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsValidBase<TBase>;
};

template <bool THasMsgType>
struct MessageImplProcessFieldsValidBase<false, THasMsgType>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsValidBaseT =
    typename MessageImplProcessFieldsValidBase<
        TBase::InterfaceOptions::HasValid && (!TImplOpt::HasNoValidImpl),
        TImplOpt::HasMsgType
    >::template Type<TBase, TImplOpt>;

template <typename TBase, typename TActual = void>
class MessageImplFieldsLengthBase : public TBase
{
    using Base = TBase;

protected:
    ~MessageImplFieldsLengthBase() noexcept = default;
    virtual std::size_t lengthImpl() const override
    {
        return lengthImplInternal(Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    using Tag = typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type;

    std::size_t lengthImplInternal(NoActual) const
    {
        return Base::doLength();
    }

    std::size_t lengthImplInternal(HasActual) const
    {
        return static_cast<const TActual*>(this)->doLength();
    }
};

template <bool THasFieldsLengthImpl, bool THasMsgType>
struct MessageImplProcessFieldsLengthBase;

template <>
struct MessageImplProcessFieldsLengthBase<true, true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsLengthBase<TBase, typename TOpt::MsgType>;
};

template <>
struct MessageImplProcessFieldsLengthBase<true, false>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplFieldsLengthBase<TBase>;
};

template <bool THasMsgType>
struct MessageImplProcessFieldsLengthBase<false, THasMsgType>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsLengthBaseT =
    typename MessageImplProcessFieldsLengthBase<
        TBase::InterfaceOptions::HasLength && (!TImplOpt::HasNoLengthImpl),
        TImplOpt::HasMsgType
    >::template Type<TBase, TImplOpt>;

template <typename TBase, typename TOpt>
class MessageImplRefreshBase : public TBase
{
protected:
    ~MessageImplRefreshBase() noexcept = default;
    virtual bool refreshImpl() override
    {
        using Tag =
            typename std::conditional<
                TOpt::HasMsgType,
                Downcast,
                NoDowncast
            >::type;
        return refreshInternal(Tag());
    }

private:
    struct Downcast {};
    struct NoDowncast {};
    bool refreshInternal(Downcast)
    {
        using Actual = typename TOpt::MsgType;
        return static_cast<Actual*>(this)->doRefresh();
    }

    bool refreshInternal(NoDowncast)
    {
        static_assert(TOpt::HasFieldsImpl, "Must use FieldsImpl option");
        return TBase::doRefresh();
    }
};

template <bool THasDoRefresh>
struct MessageImplProcessRefreshBase;

template <>
struct MessageImplProcessRefreshBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplRefreshBase<TBase, TOpt>;
};

template <>
struct MessageImplProcessRefreshBase<false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TImplOpt>
using MessageImplRefreshBaseT =
    typename MessageImplProcessRefreshBase<
        TBase::InterfaceOptions::HasRefresh && TImplOpt::HasDoRefresh
    >::template Type<TBase, TImplOpt>;

template <typename TBase, typename TActual>
class MessageImplDispatchBase : public TBase
{
protected:
    ~MessageImplDispatchBase() noexcept = default;
    virtual void dispatchImpl(typename TBase::Handler& handler) override
    {
        static_assert(std::is_base_of<TBase, TActual>::value,
            "TActual is not derived class");
        handler.handle(static_cast<TActual&>(*this));
    }
};

template <bool THasDispatchImpl>
struct MessageImplProcessDispatchBase;

template <>
struct MessageImplProcessDispatchBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplDispatchBase<TBase, typename TOpt::MsgType>;
};

template <>
struct MessageImplProcessDispatchBase<false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TImplOpt>
using MessageImplDispatchBaseT =
    typename MessageImplProcessDispatchBase<
        TBase::InterfaceOptions::HasHandler && TImplOpt::HasMsgType && (!TImplOpt::HasNoDispatchImpl)
    >::template Type<TBase, TImplOpt>;

template <typename TMessage, typename... TOptions>
class MessageImplBuilder
{
    using ParsedOptions = MessageImplOptionsParser<TOptions...>;
    using InterfaceOptions = typename TMessage::InterfaceOptions;

    using FieldsBase = MessageImplFieldsBaseT<TMessage, ParsedOptions>;
    using StaticNumIdBase = MessageImplStaticNumIdBaseT<FieldsBase, ParsedOptions>;
    using PolymorphicStaticNumIdBase = MessageImplPolymorhpicStaticNumIdBaseT<StaticNumIdBase, ParsedOptions>;
    using NoIdBase = MessageImplNoIdBaseT<PolymorphicStaticNumIdBase, ParsedOptions>;
    using FieldsReadImplBase = MessageImplFieldsReadImplBaseT<NoIdBase, ParsedOptions>;
    using FieldsWriteImplBase = MessageImplFieldsWriteImplBaseT<FieldsReadImplBase, ParsedOptions>;
    using FieldsValidBase = MessageImplFieldsValidBaseT<FieldsWriteImplBase, ParsedOptions>;
    using FieldsLengthBase = MessageImplFieldsLengthBaseT<FieldsValidBase, ParsedOptions>;
    using RefreshBase = MessageImplRefreshBaseT<FieldsLengthBase, ParsedOptions>;
    using DispatchBase = MessageImplDispatchBaseT<RefreshBase, ParsedOptions>;

public:
    using Options = ParsedOptions;
    using Type = DispatchBase;
};

template <typename TMessage, typename... TOptions>
using MessageImplBuilderT =
    typename MessageImplBuilder<TMessage, TOptions...>::Type;

}  // namespace details

}  // namespace comms


