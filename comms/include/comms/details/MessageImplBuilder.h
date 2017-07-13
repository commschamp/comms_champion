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
    static const typename TBase::MsgIdType MsgId = static_cast<typename TBase::MsgIdType>(TId);

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

template <typename TBase>
class MessageImplPolymorhpicStaticNumIdBase : public TBase
{
protected:
    ~MessageImplPolymorhpicStaticNumIdBase() = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        return TBase::doGetId();
    }
};

template <bool THasStaticMsgId>
struct MessageImplProcessPolymorhpicStaticNumIdBase;

template <>
struct MessageImplProcessPolymorhpicStaticNumIdBase<true>
{
    template <typename TBase>
    using Type = MessageImplPolymorhpicStaticNumIdBase<TBase>;
};

template <>
struct MessageImplProcessPolymorhpicStaticNumIdBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageImplPolymorhpicStaticNumIdBaseT =
    typename MessageImplProcessPolymorhpicStaticNumIdBase<
        TBase::InterfaceOptions::HasMsgIdType && TBase::InterfaceOptions::HasMsgIdInfo &&
            TOpt::HasStaticMsgId
        >::template Type<TBase>;

template <typename TBase>
class MessageImplNoIdBase : public TBase
{
protected:
    ~MessageImplNoIdBase() = default;
    virtual typename TBase::MsgIdParamType getIdImpl() const override
    {
        static const auto MsgId = typename TBase::MsgIdType();
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
    ~MessageImplFieldsValidBase() = default;
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
    ~MessageImplFieldsLengthBase() = default;
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

template <typename TBase, typename TActual>
class MessageImplRefreshBase : public TBase
{
protected:
    ~MessageImplRefreshBase() = default;
    virtual bool refreshImpl() override
    {
        return static_cast<TActual*>(this)->doRefresh();
    }
};

template <bool THasDoRefresh>
struct MessageImplProcessRefreshBase;

template <>
struct MessageImplProcessRefreshBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplRefreshBase<TBase, typename TOpt::MsgType>;
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
        TBase::InterfaceOptions::HasRefresh && TImplOpt::HasDoRefresh && TImplOpt::HasMsgType
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


