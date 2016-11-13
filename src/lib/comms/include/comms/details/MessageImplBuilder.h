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

protected:
    ~MessageImplFieldsBase() = default;

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

template <typename TBase>
class MessageImplFieldsReadBase : public TBase
{
    typedef TBase Base;
protected:
    ~MessageImplFieldsReadBase() = default;

    template <std::size_t TIdx>
    comms::ErrorStatus readFieldsUntil(
        typename Base::ReadIterator& iter,
        std::size_t& size)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachUntil<TIdx>(Base::fields(), FieldReader(iter, status, size));
        return status;
    }

    template <std::size_t TIdx>
    comms::ErrorStatus readFieldsFrom(
        typename Base::ReadIterator& iter,
        std::size_t& size)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFrom<TIdx>(Base::fields(), FieldReader(iter, status, size));
        return status;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    comms::ErrorStatus readFieldsFromUntil(
        typename Base::ReadIterator& iter,
        std::size_t& size)
    {
        auto status = comms::ErrorStatus::Success;
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(Base::fields(), FieldReader(iter, status, size));
        return status;
    }

private:
    class FieldReader
    {
        typedef typename TBase::ReadIterator ReadIterator;
    public:
        FieldReader(ReadIterator& iter, comms::ErrorStatus& status, std::size_t& size)
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
        ReadIterator& iter_;
        comms::ErrorStatus& status_;
        std::size_t& size_;
    };

};

template <typename TBase, bool THasFieldsReadImpl>
struct MessageImplProcessFieldsReadBase;

template <typename TBase>
struct MessageImplProcessFieldsReadBase<TBase, true>
{
    typedef MessageImplFieldsReadBase<TBase> Type;
};

template <typename TBase>
struct MessageImplProcessFieldsReadBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsReadBaseT =
    typename MessageImplProcessFieldsReadBase<
        TBase, TBase::InterfaceOptions::HasReadIterator && TImplOpt::HasFieldsImpl>::Type;

template <typename TBase>
class MessageImplFieldsReadImplBase : public TBase
{
    typedef TBase Base;
protected:
    ~MessageImplFieldsReadImplBase() = default;
    virtual comms::ErrorStatus readImpl(
        typename Base::ReadIterator& iter,
        std::size_t size) override
    {
        return Base::template readFieldsFrom<0>(iter, size);
    }
};

template <typename TBase, bool THasFieldsReadImpl>
struct MessageImplProcessFieldsReadImplBase;

template <typename TBase>
struct MessageImplProcessFieldsReadImplBase<TBase, true>
{
    typedef MessageImplFieldsReadImplBase<TBase> Type;
};

template <typename TBase>
struct MessageImplProcessFieldsReadImplBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsReadImplBaseT =
    typename MessageImplProcessFieldsReadImplBase<
        TBase,
        TBase::InterfaceOptions::HasReadIterator && TImplOpt::HasFieldsImpl && (!TImplOpt::HasNoDefaultFieldsReadImpl)
    >::Type;

template <typename TBase>
class MessageImplFieldsWriteBase : public TBase
{
    typedef TBase Base;

protected:
    ~MessageImplFieldsWriteBase() = default;
    virtual comms::ErrorStatus writeImpl(
        typename Base::WriteIterator& iter,
        std::size_t size) const override
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEach(Base::fields(), FieldWriter(iter, status, remainingSize));
        return status;
    }

    template <std::size_t TIdx>
    comms::ErrorStatus writeFieldsUntil(
        typename Base::WriteIterator& iter,
        std::size_t size) const
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEachUntil<TIdx>(Base::fields(), FieldWriter(iter, status, remainingSize));
        return status;
    }

    template <std::size_t TIdx>
    comms::ErrorStatus writeFieldsFrom(
        typename Base::WriteIterator& iter,
        std::size_t size) const
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEachFrom<TIdx>(Base::fields(), FieldWriter(iter, status, remainingSize));
        return status;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    comms::ErrorStatus writeFieldsFromUntil(
        typename Base::WriteIterator& iter,
        std::size_t size) const
    {
        auto status = comms::ErrorStatus::Success;
        std::size_t remainingSize = size;
        util::tupleForEachFromUntil<TFromIdx, TUntilIdx>(Base::fields(), FieldWriter(iter, status, remainingSize));
        return status;
    }
private:
    class FieldWriter
    {
    public:
        typedef typename TBase::WriteIterator WriteIterator;
        FieldWriter(WriteIterator& iter, comms::ErrorStatus& status, std::size_t& size)
            : iter_(iter),
              status_(status),
              size_(size)
        {
        }

        template <typename TField>
        void operator()(TField& field) {
            if (status_ == comms::ErrorStatus::Success) {
                status_ = field.write(iter_, size_);
                if (status_ == comms::ErrorStatus::Success) {
                    GASSERT(field.length() <= size_);
                    size_ -= field.length();
                }
            }
        }

    private:
        WriteIterator& iter_;
        comms::ErrorStatus& status_;
        std::size_t& size_;
    };
};

template <typename TBase, bool THasFieldsWriteImpl>
struct MessageImplProcessFieldsWriteBase;

template <typename TBase>
struct MessageImplProcessFieldsWriteBase<TBase, true>
{
    typedef MessageImplFieldsWriteBase<TBase> Type;
};

template <typename TBase>
struct MessageImplProcessFieldsWriteBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsWriteBaseT =
    typename MessageImplProcessFieldsWriteBase<
        TBase, TBase::InterfaceOptions::HasWriteIterator && TImplOpt::HasFieldsImpl>::Type;


template <typename TBase>
class MessageImplFieldsValidBase : public TBase
{
    typedef TBase Base;

protected:
    ~MessageImplFieldsValidBase() = default;
    virtual bool validImpl() const override
    {
        return util::tupleAccumulate(Base::fields(), true, FieldValidityRetriever());
    }

private:
    struct FieldValidityRetriever
    {
        template <typename TField>
        bool operator()(bool valid, const TField& field) const
        {
            return valid && field.valid();
        }
    };
};

template <typename TBase, bool THasFieldsValidImpl>
struct MessageImplProcessFieldsValidBase;

template <typename TBase>
struct MessageImplProcessFieldsValidBase<TBase, true>
{
    typedef MessageImplFieldsValidBase<TBase> Type;
};

template <typename TBase>
struct MessageImplProcessFieldsValidBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsValidBaseT =
    typename MessageImplProcessFieldsValidBase<
        TBase,
        TBase::InterfaceOptions::HasValid && TImplOpt::HasFieldsImpl
    >::Type;

template <typename TBase>
class MessageImplFieldsLengthBase : public TBase
{
    typedef TBase Base;

protected:
    ~MessageImplFieldsLengthBase() = default;
    virtual std::size_t lengthImpl() const override
    {
        return util::tupleAccumulate(Base::fields(), 0U, FieldLengthRetriever());
    }

private:
    struct FieldLengthRetriever
    {
        template <typename TField>
        std::size_t operator()(std::size_t size, const TField& field) const
        {
            return size + field.length();
        }
    };
};

template <typename TBase, bool THasFieldsLengthImpl>
struct MessageImplProcessFieldsLengthBase;

template <typename TBase>
struct MessageImplProcessFieldsLengthBase<TBase, true>
{
    typedef MessageImplFieldsLengthBase<TBase> Type;
};

template <typename TBase>
struct MessageImplProcessFieldsLengthBase<TBase, false>
{
    typedef TBase Type;
};

template <typename TBase, typename TImplOpt>
using MessageImplFieldsLengthBaseT =
    typename MessageImplProcessFieldsLengthBase<
        TBase,
        TBase::InterfaceOptions::HasLength && TImplOpt::HasFieldsImpl
    >::Type;

template <typename TBase, typename TActual>
class MessageImplDispatchBase : public TBase
{
protected:
    ~MessageImplDispatchBase() = default;
    virtual void dispatchImpl(typename TBase::Handler& handler) override
    {
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
        TBase, TImplOpt, TBase::InterfaceOptions::HasHandler && TImplOpt::HasDispatchImpl>::Type;

template <typename TMessage, typename... TOptions>
class MessageImplBuilder
{
    typedef MessageImplOptionsParser<TOptions...> ParsedOptions;
    typedef typename TMessage::InterfaceOptions InterfaceOptions;

    typedef MessageImplFieldsBaseT<TMessage, ParsedOptions> FieldsBase;
    typedef MessageImplFieldsReadBaseT<FieldsBase, ParsedOptions> FieldsReadBase;
    typedef MessageImplFieldsReadImplBaseT<FieldsReadBase, ParsedOptions> FieldsReadImplBase;
    typedef MessageImplFieldsWriteBaseT<FieldsReadImplBase, ParsedOptions> FieldsWriteBase;
    typedef MessageImplFieldsValidBaseT<FieldsWriteBase, ParsedOptions> FieldsValidBase;
    typedef MessageImplFieldsLengthBaseT<FieldsValidBase, ParsedOptions> FieldsLengthBase;
    typedef MessageImplStaticNumIdBaseT<FieldsLengthBase, ParsedOptions> StaticNumIdBase;
    typedef MessageImplNoIdBaseT<StaticNumIdBase, ParsedOptions> NoIdBase;
    typedef MessageImplDispatchBaseT<NoIdBase, ParsedOptions> DispatchBase;

public:
    typedef ParsedOptions Options;
    typedef DispatchBase Type;
};

template <typename TMessage, typename... TOptions>
using MessageImplBuilderT =
    typename MessageImplBuilder<TMessage, TOptions...>::Type;

}  // namespace details

}  // namespace comms


