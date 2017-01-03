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
    static const auto MsgId = static_cast<typename TBase::MsgIdType>(TId);

protected:
    ~MessageImplStaticNumIdBase() = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        return MsgId;
    }
};

template <typename TBase, typename TOpt, bool THasStaticMsgId>
struct MessageImplProcessStaticNumIdBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessStaticNumIdBase<TBase, TOpt, true>
{
    typedef MessageImplStaticNumIdBase<TBase, TOpt::MsgId> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessStaticNumIdBase<TBase, TOpt, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpt>
using MessageImplStaticNumIdBaseT =
    typename MessageImplProcessStaticNumIdBase<
        TBase, TOpt, TBase::InterfaceOptions::HasMsgIdType && TOpt::HasStaticMsgId>::Type;

template <typename TBase>
class MessageImplNoIdBase : public TBase
{
public:
    static const auto MsgId = typename TBase::MsgIdType();

protected:
    ~MessageImplNoIdBase() = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        GASSERT(!"The message id is not supposed to be retrieved");
        return MsgId;
    }
};

template <typename TBase, bool THasNoId>
struct MessageImplProcessNoIdBase;

template <typename TBase>
struct MessageImplProcessNoIdBase<TBase, true>
{
    typedef MessageImplNoIdBase<TBase> Type;
};

template <typename TBase>
struct MessageImplProcessNoIdBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpt>
using MessageImplNoIdBaseT =
    typename MessageImplProcessNoIdBase<
        TBase, TBase::InterfaceOptions::HasMsgIdType && TOpt::HasNoIdImpl>::Type;

template <typename TBase, typename TAllFields>
class MessageImplFieldsBase : public TBase
{
public:
    typedef TAllFields AllFields;

    AllFields& fields()
    {
        return fields_;
    }

    const AllFields& fields() const
    {
        return fields_;
    }

    template <typename TIter>
    comms::ErrorStatus doRead(
        TIter& iter,
        std::size_t size)
    {
        return readFieldsFrom<0>(iter, size);
    }

    template <typename TIter>
    comms::ErrorStatus doWrite(
        TIter& iter,
        std::size_t size) const
    {
        return writeFieldsFrom<0>(iter, size);
    }

    bool doValid() const
    {
        return util::tupleAccumulate(fields(), true, FieldValidityRetriever());
    }

    std::size_t doLength() const
    {
        return util::tupleAccumulate(fields(), 0U, FieldLengthRetriever());
    }

protected:
    ~MessageImplFieldsBase() = default;

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
    comms::ErrorStatus readFieldsFrom(
        TIter& iter,
        std::size_t& size)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFrom<TIdx>(fields(), makeFieldReader(iter, status, size));
        return status;
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
    comms::ErrorStatus writeFieldsFrom(
        TIter& iter,
        std::size_t size) const
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEachFrom<TIdx>(fields(), makeFieldWriter(iter, status, remainingSize));
        return status;
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

private:
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

    struct FieldValidityRetriever
    {
        template <typename TField>
        bool operator()(bool valid, const TField& field) const
        {
            return valid && field.valid();
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

template <typename TBase, typename TOpt, bool THasFieldsImpl>
struct MessageImplProcessFieldsBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsBase<TBase, TOpt, true>
{
    typedef MessageImplFieldsBase<TBase, typename TOpt::Fields> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsBase<TBase, TOpt, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TOpt>
using MessageImplFieldsBaseT =
    typename MessageImplProcessFieldsBase<TBase, TOpt, TOpt::HasFieldsImpl>::Type;

template <typename TBase, typename TActual = void>
class MessageImplFieldsReadImplBase : public TBase
{
    typedef TBase Base;
protected:
    ~MessageImplFieldsReadImplBase() = default;
    virtual comms::ErrorStatus readImpl(
        typename Base::ReadIterator& iter,
        std::size_t size) override
    {
        return readImplInternal(iter, size, Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    typedef typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type Tag;

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

template <typename TBase, typename TOpt, bool THasFieldsReadImpl, bool THasMsgType>
struct MessageImplProcessFieldsReadImplBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsReadImplBase<TBase, TOpt, true, true>
{
    typedef MessageImplFieldsReadImplBase<TBase, typename TOpt::MsgType> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsReadImplBase<TBase, TOpt, true, false>
{
    typedef MessageImplFieldsReadImplBase<TBase> Type;
};

template <typename TBase, typename TOpt, bool THasMsgType>
struct MessageImplProcessFieldsReadImplBase<TBase, TOpt, false, THasMsgType>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsReadImplBaseT =
    typename MessageImplProcessFieldsReadImplBase<
        TBase,
        TImplOpt,
        TBase::InterfaceOptions::HasReadIterator && (!TImplOpt::HasNoReadImpl) &&
        (TImplOpt::HasFieldsImpl || TImplOpt::HasAssumeFieldsExistence) ,
        TImplOpt::HasMsgType
    >::Type;

template <typename TBase, typename TActual = void>
class MessageImplFieldsWriteImplBase : public TBase
{
    typedef TBase Base;

protected:
    ~MessageImplFieldsWriteImplBase() = default;
    virtual comms::ErrorStatus writeImpl(
        typename Base::WriteIterator& iter,
        std::size_t size) const override
    {
        return writeImplInternal(iter, size, Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    typedef typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type Tag;

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

template <typename TBase, typename TOpt, bool THasFieldsWriteImpl, bool THasMsgType>
struct MessageImplProcessFieldsWriteImplBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsWriteImplBase<TBase, TOpt, true, true>
{
    typedef MessageImplFieldsWriteImplBase<TBase, typename TOpt::MsgType> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsWriteImplBase<TBase, TOpt, true, false>
{
    typedef MessageImplFieldsWriteImplBase<TBase> Type;
};

template <typename TBase, typename TOpt, bool THasMsgType>
struct MessageImplProcessFieldsWriteImplBase<TBase, TOpt, false, THasMsgType>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsWriteImplBaseT =
    typename MessageImplProcessFieldsWriteImplBase<
        TBase,
        TImplOpt,
        TBase::InterfaceOptions::HasWriteIterator && (!TImplOpt::HasNoWriteImpl) &&
            (TImplOpt::HasFieldsImpl || TImplOpt::HasAssumeFieldsExistence),
        TImplOpt::HasMsgType
    >::Type;

template <typename TBase, typename TActual = void>
class MessageImplFieldsValidBase : public TBase
{
    typedef TBase Base;

protected:
    ~MessageImplFieldsValidBase() = default;
    virtual bool validImpl() const override
    {
        return validImplInternal(Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    typedef typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type Tag;

    bool validImplInternal(NoActual) const
    {
        return Base::doValid();
    }

    bool validImplInternal(HasActual) const
    {
        return static_cast<const TActual*>(this)->doValid();
    }
};

template <typename TBase, typename TOpt, bool THasFieldsValidImpl, bool THasMsgType>
struct MessageImplProcessFieldsValidBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsValidBase<TBase, TOpt, true, true>
{
    typedef MessageImplFieldsValidBase<TBase, typename TOpt::MsgType> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsValidBase<TBase, TOpt, true, false>
{
    typedef MessageImplFieldsValidBase<TBase> Type;
};

template <typename TBase, typename TOpt, bool THasMsgType>
struct MessageImplProcessFieldsValidBase<TBase, TOpt, false, THasMsgType>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsValidBaseT =
    typename MessageImplProcessFieldsValidBase<
        TBase,
        TImplOpt,
        TBase::InterfaceOptions::HasValid && (!TImplOpt::HasNoValidImpl) &&
            (TImplOpt::HasFieldsImpl || TImplOpt::HasAssumeFieldsExistence),
        TImplOpt::HasMsgType
    >::Type;

template <typename TBase, typename TActual = void>
class MessageImplFieldsLengthBase : public TBase
{
    typedef TBase Base;

protected:
    ~MessageImplFieldsLengthBase() = default;
    virtual std::size_t lengthImpl() const override
    {
        return lengthImplInternal(Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    typedef typename std::conditional<
        std::is_same<TActual, void>::value,
        NoActual,
        HasActual
    >::type Tag;

    std::size_t lengthImplInternal(NoActual) const
    {
        return Base::doLength();
    }

    std::size_t lengthImplInternal(HasActual) const
    {
        return static_cast<const TActual*>(this)->doLength();
    }
};

template <typename TBase, typename TOpt, bool THasFieldsLengthImpl, bool THasMsgType>
struct MessageImplProcessFieldsLengthBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsLengthBase<TBase, TOpt, true, true>
{
    typedef MessageImplFieldsLengthBase<TBase, typename TOpt::MsgType> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessFieldsLengthBase<TBase, TOpt, true, false>
{
    typedef MessageImplFieldsLengthBase<TBase> Type;
};

template <typename TBase, typename TOpt, bool THasMsgType>
struct MessageImplProcessFieldsLengthBase<TBase, TOpt, false, THasMsgType>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsLengthBaseT =
    typename MessageImplProcessFieldsLengthBase<
        TBase,
        TImplOpt,
        TBase::InterfaceOptions::HasLength && (!TImplOpt::HasNoLengthImpl) &&
            (TImplOpt::HasFieldsImpl || TImplOpt::HasAssumeFieldsExistence),
        TImplOpt::HasMsgType
    >::Type;

template <typename TBase, typename TActual>
class MessageImplRefreshBase : public TBase
{
    typedef TBase Base;

protected:
    ~MessageImplRefreshBase() = default;
    virtual bool refreshImpl() override
    {
        return static_cast<TActual*>(this)->doRefresh();
    }
};

template <typename TBase, typename TOpt, bool THasDoRefresh>
struct MessageImplProcessRefreshBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessRefreshBase<TBase, TOpt, true>
{
    typedef MessageImplRefreshBase<TBase, typename TOpt::MsgType> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessRefreshBase<TBase, TOpt, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplRefreshBaseT =
    typename MessageImplProcessRefreshBase<
        TBase,
        TImplOpt,
        TBase::InterfaceOptions::HasRefresh && TImplOpt::HasDoRefresh && TImplOpt::HasMsgType
    >::Type;

template <typename TBase, typename TActual>
class MessageImplDispatchBase : public TBase
{
protected:
    ~MessageImplDispatchBase() = default;
    virtual void dispatchImpl(typename TBase::Handler& handler) override
    {
        static_assert(std::is_base_of<TBase, TActual>::value,
            "TActual is not derived class");
        handler.handle(static_cast<TActual&>(*this));
    }
};

template <typename TBase, typename TOpt, bool THasDispatchImpl>
struct MessageImplProcessDispatchBase;

template <typename TBase, typename TOpt>
struct MessageImplProcessDispatchBase<TBase, TOpt, true>
{
    typedef MessageImplDispatchBase<TBase, typename TOpt::MsgType> Type;
};

template <typename TBase, typename TOpt>
struct MessageImplProcessDispatchBase<TBase, TOpt, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplDispatchBaseT =
    typename MessageImplProcessDispatchBase<
        TBase,
        TImplOpt,
        TBase::InterfaceOptions::HasHandler && TImplOpt::HasMsgType && (!TImplOpt::HasNoDispatchImpl)
    >::Type;

template <typename TMessage, typename... TOptions>
class MessageImplBuilder
{
    typedef MessageImplOptionsParser<TOptions...> ParsedOptions;
    typedef typename TMessage::InterfaceOptions InterfaceOptions;

    typedef MessageImplFieldsBaseT<TMessage, ParsedOptions> FieldsBase;
    typedef MessageImplStaticNumIdBaseT<FieldsBase, ParsedOptions> StaticNumIdBase;
    typedef MessageImplNoIdBaseT<StaticNumIdBase, ParsedOptions> NoIdBase;
    typedef MessageImplFieldsReadImplBaseT<NoIdBase, ParsedOptions> FieldsReadImplBase;
    typedef MessageImplFieldsWriteImplBaseT<FieldsReadImplBase, ParsedOptions> FieldsWriteImplBase;
    typedef MessageImplFieldsValidBaseT<FieldsWriteImplBase, ParsedOptions> FieldsValidBase;
    typedef MessageImplFieldsLengthBaseT<FieldsValidBase, ParsedOptions> FieldsLengthBase;
    typedef MessageImplRefreshBaseT<FieldsLengthBase, ParsedOptions> RefreshBase;
    typedef MessageImplDispatchBaseT<RefreshBase, ParsedOptions> DispatchBase;

public:
    typedef ParsedOptions Options;
    typedef DispatchBase Type;
};

template <typename TMessage, typename... TOptions>
using MessageImplBuilderT =
    typename MessageImplBuilder<TMessage, TOptions...>::Type;

}  // namespace details

}  // namespace comms


