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

//----------------------------------------------------

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

//----------------------------------------------------

template <typename TAllFields>
class MessageImplFieldsContainer
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

    static constexpr bool areFieldsVersionDependent()
    {
        return comms::util::tupleTypeIsAnyOf<AllFields>(VersionDepChecker());
    }

    template <typename TIter>
    comms::ErrorStatus doRead(TIter& iter, std::size_t size)
    {
#ifdef _MSC_VER
// For some reason VS2015 32 bit compiler may generate "integral constant overflow"
// warning on the code below
#pragma warning( push )
#pragma warning( disable : 4307)
#endif

        using Tag =
            typename std::conditional<
                comms::util::tupleTypeAccumulate<AllFields>(true, ReadNoStatusDetector()),
                NoStatusTag,
                UseStatusTag
            >::type;

#ifdef _MSC_VER
#pragma warning( pop )
#endif
        return doReadInternal(iter, size, Tag());
    }

    template <typename TIter>
    comms::ErrorStatus doWrite(
        TIter& iter,
        std::size_t size) const
    {
        if (size < doLength()) {
            return comms::ErrorStatus::BufferOverflow;
        }

        doWriteNoStatusFrom<0>(iter);
        return comms::ErrorStatus::Success;
    }

    bool doValid() const
    {
        return util::tupleAccumulate(fields(), true, FieldValidityRetriever());
    }

    std::size_t doLength() const
    {
        return util::tupleAccumulate(fields(), static_cast<std::size_t>(0U), FieldLengthRetriever());
    }

    template <std::size_t TFromIdx>
    std::size_t doLengthFrom() const
    {
        return
            util::tupleAccumulateFromUntil<TFromIdx, std::tuple_size<AllFields>::value>(
                fields(), static_cast<std::size_t>(0U), FieldLengthRetriever());
    }

    template <std::size_t TUntilIdx>
    std::size_t doLengthUntil() const
    {
        return
            util::tupleAccumulateFromUntil<0, TUntilIdx>(
                fields(), static_cast<std::size_t>(0U), FieldLengthRetriever());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    std::size_t doLengthFromUntil() const
    {
        return
            util::tupleAccumulateFromUntil<TFromIdx, TUntilIdx>(
                fields(), static_cast<std::size_t>(0U), FieldLengthRetriever());
    }

    static constexpr std::size_t doMinLength()
    {
        return util::tupleTypeAccumulate<AllFields>(static_cast<std::size_t>(0U), FieldMinLengthRetriever());
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t doMinLengthFrom()
    {
        return util::tupleTypeAccumulateFromUntil<TFromIdx, std::tuple_size<AllFields>::value, AllFields>(
                    static_cast<std::size_t>(0U), FieldMinLengthRetriever());
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t doMinLengthUntil()
    {
        return util::tupleTypeAccumulateFromUntil<0, TUntilIdx, AllFields>(
                    static_cast<std::size_t>(0U), FieldMinLengthRetriever());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t doMinLengthFromUntil()
    {
        return util::tupleTypeAccumulateFromUntil<TFromIdx, TUntilIdx, AllFields>(
                    static_cast<std::size_t>(0U), FieldMinLengthRetriever());
    }

    static constexpr std::size_t doMaxLength()
    {
        return util::tupleTypeAccumulate<AllFields>(static_cast<std::size_t>(0U), FieldMaxLengthRetriever());
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t doMaxLengthFrom()
    {
        return util::tupleTypeAccumulateFromUntil<TFromIdx, std::tuple_size<AllFields>::value, AllFields>(
                    static_cast<std::size_t>(0U), FieldMaxLengthRetriever());
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t doMaxLengthUntil()
    {
        return util::tupleTypeAccumulateFromUntil<0, TUntilIdx, AllFields>(
                    static_cast<std::size_t>(0U), FieldMaxLengthRetriever());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t doMaxLengthFromUntil()
    {
        return util::tupleTypeAccumulateFromUntil<TFromIdx, TUntilIdx, AllFields>(
                    static_cast<std::size_t>(0U), FieldMaxLengthRetriever());
    }

    bool doRefresh()
    {
        return util::tupleAccumulate(fields(), false, FieldRefresher());
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
    struct NoStatusTag {};
    struct UseStatusTag {};

    struct ReadNoStatusDetector
    {
        constexpr ReadNoStatusDetector() = default;

        template <typename TField>
        constexpr bool operator()(bool soFar) const
        {
            return
                (TField::minLength() == TField::maxLength()) &&
                (!TField::ParsedOptions::HasCustomValueReader) &&
                (!TField::ParsedOptions::HasCustomRead) &&
                (!TField::ParsedOptions::HasFailOnInvalid) &&
                (!TField::ParsedOptions::HasSequenceElemLengthForcing)  &&
                (!TField::ParsedOptions::HasSequenceSizeForcing)  &&
                (!TField::ParsedOptions::HasSequenceSizeFieldPrefix)  &&
                (!TField::ParsedOptions::HasSequenceSerLengthFieldPrefix) &&
                (!TField::ParsedOptions::HasSequenceElemSerLengthFieldPrefix) &&
                (!TField::ParsedOptions::HasSequenceElemFixedSerLengthFieldPrefix) &&
                (!TField::ParsedOptions::HasSequenceTrailingFieldSuffix) &&
                (!TField::ParsedOptions::HasSequenceTerminationFieldSuffix) &&
                soFar;
            ;
        }
    };

    template <typename TIter>
    comms::ErrorStatus doReadInternal(
        TIter& iter,
        std::size_t size,
        UseStatusTag)
    {
        return doReadFromAndUpdateLen<0>(iter, size);
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

        doReadNoStatusFrom<0>(iter);
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
                auto fromIter = iter_;
                status_ = field.read(iter_, size_);
                if (status_ == comms::ErrorStatus::Success) {
                    auto diff = static_cast<std::size_t>(std::distance(fromIter, iter_));
                    COMMS_ASSERT(diff <= size_);
                    size_ -= diff;
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
                    COMMS_ASSERT(field.length() <= size_);
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
            return field.refresh() || refreshed;
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

    struct FieldMinLengthRetriever
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t size) const
        {
            return size + TField::minLength();
        }
    };

    struct FieldMaxLengthRetriever
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t size) const
        {
            return size + TField::maxLength();
        }
    };

    struct VersionDepChecker
    {
        template <typename TField>
        constexpr bool operator()() const
        {
            return TField::isVersionDependent();
        }
    };

    AllFields fields_;
};

//----------------------------------------------------

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


//----------------------------------------------------

template <typename TBase>
class MessageImplVersionBase : public TBase
{
public:
    using VersionType = typename TBase::VersionType;

    bool doFieldsVersionUpdate()
    {
        return util::tupleAccumulate(TBase::fields(), false, FieldVersionUpdater(TBase::version()));
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

private:

    struct FieldVersionUpdater
    {
        FieldVersionUpdater(VersionType version) : version_(version) {}

        template <typename TField>
        bool operator()(bool updated, TField& field) const
        {
            using FieldVersionType = typename std::decay<decltype(field)>::type::VersionType;
            return field.setVersion(static_cast<FieldVersionType>(version_)) || updated;
        }

    private:
        const VersionType version_ = static_cast<VersionType>(0);
    };
};

template <bool THasVersion>
struct MessageImplProcessVersionBase;

template <>
struct MessageImplProcessVersionBase<true>
{
    template <typename TBase>
    using Type = MessageImplVersionBase<TBase>;
};

template <>
struct MessageImplProcessVersionBase<false>
{
    template <typename TBase>
    using Type = TBase;
};

template <typename TBase, typename TOpt>
using MessageImplVersionBaseT =
    typename MessageImplProcessVersionBase<
            TOpt::HasFieldsImpl && TBase::InterfaceOptions::HasVersionInExtraTransportFields
    >::template Type<TBase>;

//----------------------------------------------------

template <typename TBase, bool THasFields>
class AnyFieldHasNonDefaultRefresh;

template <typename TBase>
class AnyFieldHasNonDefaultRefresh<TBase, true>
{
    struct RefreshChecker
    {
        template <typename TField>
        constexpr bool operator()() const
        {
            return TField::hasNonDefaultRefresh();
        }
    };
public:
    static const bool Value =
            util::tupleTypeIsAnyOf<typename TBase::AllFields>(RefreshChecker());
};

template <typename TBase>
class AnyFieldHasNonDefaultRefresh<TBase, false>
{
public:
    static const bool Value = false;
};

template <typename TBase, typename TImplOpt>
constexpr bool anyFieldHasNonDefaultRefresh()
{
    return AnyFieldHasNonDefaultRefresh<TBase, TImplOpt::HasFieldsImpl>::Value;
}

//----------------------------------------------------

template <typename TBase, bool THasFields>
struct AnyFieldsIsVersionDependent;

template <typename TBase>
struct AnyFieldsIsVersionDependent<TBase, true>
{
    static const bool Value = TBase::areFieldsVersionDependent();
};

template <typename TBase>
struct AnyFieldsIsVersionDependent<TBase, false>
{
    static const bool Value = false;
};

template <typename TBase, typename TImplOpt>
constexpr bool anyFieldIsVersionDependent()
{
    return AnyFieldsIsVersionDependent<TBase, TImplOpt::HasFieldsImpl>::Value;
}

//----------------------------------------------------

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
        using Tag = typename std::conditional<
            std::is_same<TActual, void>::value,
            NoActual,
            HasActual
        >::type;        
        return readImplInternal(iter, size, Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    comms::ErrorStatus readImplInternal(
        typename BaseImpl::ReadIterator& iter,
        std::size_t size,
        NoActual)
    {
        return BaseImpl::doRead(iter, size);
    }

    comms::ErrorStatus readImplInternal(
        typename BaseImpl::ReadIterator& iter,
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

//----------------------------------------------------

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
        using Tag = typename std::conditional<
            std::is_same<TActual, void>::value,
            NoActual,
            HasActual
        >::type;        
        return writeImplInternal(iter, size, Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    comms::ErrorStatus writeImplInternal(
        typename BaseImpl::WriteIterator& iter,
        std::size_t size,
        NoActual) const
    {
        return BaseImpl::doWrite(iter, size);
    }

    comms::ErrorStatus writeImplInternal(
        typename BaseImpl::WriteIterator& iter,
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

//----------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplFieldsValidBase : public TBase
{
    using BaseImpl = TBase;

protected:
    ~MessageImplFieldsValidBase() noexcept = default;
    virtual bool validImpl() const override
    {
        using Tag = typename std::conditional<
            std::is_same<TActual, void>::value,
            NoActual,
            HasActual
        >::type;
        return validImplInternal(Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    bool validImplInternal(NoActual) const
    {
        return BaseImpl::doValid();
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

//----------------------------------------------------

template <typename TBase, typename TActual = void>
class MessageImplFieldsLengthBase : public TBase
{
    using BaseImpl = TBase;

protected:
    ~MessageImplFieldsLengthBase() noexcept = default;
    virtual std::size_t lengthImpl() const override
    {
        using Tag = typename std::conditional<
            std::is_same<TActual, void>::value,
            NoActual,
            HasActual
        >::type;        
        return lengthImplInternal(Tag());
    }

private:
    struct HasActual {};
    struct NoActual {};

    std::size_t lengthImplInternal(NoActual) const
    {
        return BaseImpl::doLength();
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

//----------------------------------------------------

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

template <bool THasCustomRefresh>
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
        TBase::InterfaceOptions::HasRefresh &&
        (!TImplOpt::HasNoRefreshImpl) &&
            (
                TImplOpt::HasCustomRefresh ||
                anyFieldHasNonDefaultRefresh<TBase, TImplOpt>() ||
                (TBase::InterfaceOptions::HasVersionInExtraTransportFields && anyFieldIsVersionDependent<TBase, TImplOpt>())
            )
    >::template Type<TBase, TImplOpt>;

//----------------------------------------------------

template <typename TBase, typename TOpt>
class MessageImplNameBase : public TBase
{
protected:
    ~MessageImplNameBase() noexcept = default;
    virtual const char* nameImpl() const override
    {
        using Actual = typename TOpt::MsgType;
        return static_cast<const Actual*>(this)->doName();
    }
};

template <bool THasName>
struct MessageImplProcessNameBase;

template <>
struct MessageImplProcessNameBase<true>
{
    template <typename TBase, typename TOpt>
    using Type = MessageImplNameBase<TBase, TOpt>;
};

template <>
struct MessageImplProcessNameBase<false>
{
    template <typename TBase, typename TOpt>
    using Type = TBase;
};

template <typename TBase, typename TImplOpt>
using MessageImplNameBaseT =
    typename MessageImplProcessNameBase<
        TBase::InterfaceOptions::HasName && TImplOpt::HasName
    >::template Type<TBase, TImplOpt>;

//----------------------------------------------------

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

//----------------------------------------------------

template <typename TMessage, typename... TOptions>
class MessageImplBuilder
{
    using ParsedOptions = MessageImplOptionsParser<TOptions...>;
    using InterfaceOptions = typename TMessage::InterfaceOptions;

    using FieldsBase = MessageImplFieldsBaseT<TMessage, ParsedOptions>;
    using VersionBase = MessageImplVersionBaseT<FieldsBase, ParsedOptions>;
    using StaticNumIdBase = MessageImplStaticNumIdBaseT<VersionBase, ParsedOptions>;
    using PolymorphicStaticNumIdBase = MessageImplPolymorhpicStaticNumIdBaseT<StaticNumIdBase, ParsedOptions>;
    using NoIdBase = MessageImplNoIdBaseT<PolymorphicStaticNumIdBase, ParsedOptions>;
    using FieldsReadImplBase = MessageImplFieldsReadImplBaseT<NoIdBase, ParsedOptions>;
    using FieldsWriteImplBase = MessageImplFieldsWriteImplBaseT<FieldsReadImplBase, ParsedOptions>;
    using FieldsValidBase = MessageImplFieldsValidBaseT<FieldsWriteImplBase, ParsedOptions>;
    using FieldsLengthBase = MessageImplFieldsLengthBaseT<FieldsValidBase, ParsedOptions>;
    using RefreshBase = MessageImplRefreshBaseT<FieldsLengthBase, ParsedOptions>;
    using NameBase = MessageImplNameBaseT<RefreshBase, ParsedOptions>;
    using DispatchBase = MessageImplDispatchBaseT<NameBase, ParsedOptions>;

public:
    using Options = ParsedOptions;
    using Type = DispatchBase;
};

template <typename TMessage, typename... TOptions>
using MessageImplBuilderT =
    typename MessageImplBuilder<TMessage, TOptions...>::Type;

}  // namespace details

}  // namespace comms


