//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
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
#include <algorithm>

#include "comms/CompileControl.h"
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/Tuple.h"
#include "comms/field/details/VersionStorage.h"
#include "CommonFuncs.h"

namespace comms
{

namespace field
{

namespace basic
{

namespace details
{

struct VariantElemVersionDependHelper
{
    template <typename TElem>
    constexpr bool operator()() const
    {
        return TElem::isVersionDependent();
    }
};

template <typename TMembers>
constexpr bool variantIsAnyMemberVersionDependent()
{
    return comms::util::tupleTypeIsAnyOf<TMembers>(VariantElemVersionDependHelper());
}

} // namespace details

template <typename TFieldBase, typename TMembers>
class Variant :
        public TFieldBase,
        public comms::field::details::VersionStorage<
            typename TFieldBase::VersionType,
            details::variantIsAnyMemberVersionDependent<TMembers>()
        >
{
    using BaseImpl = TFieldBase;
    using VersionBaseImpl =
        comms::field::details::VersionStorage<
            typename TFieldBase::VersionType,
            details::variantIsAnyMemberVersionDependent<TMembers>()
        >;
public:
    using Members = TMembers;
    using ValueType = comms::util::TupleAsAlignedUnionT<Members>;
    using VersionType = typename BaseImpl::VersionType;

    Variant() = default;
    Variant(const ValueType& val)  : storage_(val) {}
    Variant(ValueType&& val)  : storage_(std::move(val)) {}


    Variant(const Variant& other)
    {
        if (!other.currentFieldValid()) {
            return;
        }

        comms::util::tupleForSelectedType<Members>(
            other.memIdx_, CopyConstructHelper(&storage_, &other.storage_));

        memIdx_ = other.memIdx_;
    }

    Variant(Variant&& other)
    {
        if (!other.currentFieldValid()) {
            return;
        }

        comms::util::tupleForSelectedType<Members>(
            other.memIdx_, MoveConstructHelper(&storage_, &other.storage_));

        memIdx_ = other.memIdx_;
    }

    ~Variant() noexcept
    {
        checkDestruct();
    }

    Variant& operator=(const Variant& other)
    {
        if (this == &other) {
            return *this;
        }

        checkDestruct();
        if (!other.currentFieldValid()) {
            return *this;
        }

        comms::util::tupleForSelectedType<Members>(
            other.memIdx_, CopyConstructHelper(&storage_, &other.storage_));

        memIdx_ = other.memIdx_;
        return *this;
    }

    Variant& operator=(Variant&& other)
    {
        if (this == &other) {
            return *this;
        }

        checkDestruct();

        if (!other.currentFieldValid()) {
            return *this;
        }

        comms::util::tupleForSelectedType<Members>(
            other.memIdx_, MoveConstructHelper(&storage_, &other.storage_));

        memIdx_ = other.memIdx_;
        return *this;
    }

    const ValueType& value() const
    {
        return storage_;
    }

    ValueType& value()
    {
        return storage_;
    }

    std::size_t length() const
    {
        if (!currentFieldValid()) {
            return 0U;
        }

        std::size_t len = std::numeric_limits<std::size_t>::max();
        comms::util::tupleForSelectedType<Members>(memIdx_, LengthCalcHelper(len, &storage_));
        return len;
    }

    static constexpr std::size_t minLength()
    {
        return 0U;
    }

    static constexpr std::size_t maxLength()
    {
        return comms::util::tupleTypeAccumulate<Members>(std::size_t(0), MaxLengthCalcHelper());
    }

    bool valid() const
    {
        if (!currentFieldValid()) {
            return false;
        }

        bool val = false;
        comms::util::tupleForSelectedType<Members>(memIdx_, ValidCheckHelper(val, &storage_));
        return val;
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return CommonFuncs::doMembersMembersHaveNonDefaultRefresh<Members>();
    }

    bool refresh()
    {
        if (!currentFieldValid()) {
            return false;
        }

        bool val = false;
        comms::util::tupleForSelectedType<Members>(memIdx_, RefreshHelper(val, &storage_));
        return val;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        checkDestruct();
        auto es = comms::ErrorStatus::NumOfErrorStatuses;
        comms::util::tupleForEachType<Members>(makeReadHelper(es, iter, len, &storage_, static_cast<VersionBaseImpl&>(*this)));
        COMMS_ASSERT((es == comms::ErrorStatus::Success) || (MembersCount <= memIdx_));
        COMMS_ASSERT((es != comms::ErrorStatus::Success) || (memIdx_ < MembersCount));

        return es;
    }

    static constexpr bool hasReadNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    bool canWrite() const
    {
        if (!currentFieldValid()) {
            return true;
        }

        bool val = false;
        comms::util::tupleForSelectedType<Members>(memIdx_, CanWriteHelper(val, &storage_));
        return val;

    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (!currentFieldValid()) {
            return comms::ErrorStatus::Success;
        }

        auto es = ErrorStatus::NumOfErrorStatuses;
        comms::util::tupleForSelectedType<Members>(memIdx_, makeWriteHelper(es, iter, len, &storage_));
        return es;
    }

    static constexpr bool hasWriteNoStatus()
    {
        return comms::util::tupleTypeAccumulate<TMembers>(true, WriteNoStatusDetector());
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        if (!currentFieldValid()) {
            return;
        }

        comms::util::tupleForSelectedType<Members>(memIdx_, makeWriteNoStatusHelper(iter, &storage_));
    }


    std::size_t currentField() const
    {
        return memIdx_;
    }

    void selectField(std::size_t idx)
    {
        if (idx == memIdx_) {
            return;
        }

        checkDestruct();
        if (!isIdxValid(idx)) {
            return;
        }

        comms::util::tupleForSelectedType<Members>(idx, ConstructHelper(&storage_));
        memIdx_ = idx;
    }

    template <typename TFunc>
    void currentFieldExec(TFunc&& func)
    {
        if (!currentFieldValid()) {
            COMMS_ASSERT(!"Invalid field execution");
            return;
        }

        comms::util::tupleForSelectedType<Members>(memIdx_, makeExecHelper(std::forward<TFunc>(func)));
    }

    template <typename TFunc>
    void currentFieldExec(TFunc&& func) const
    {
        if (!currentFieldValid()) {
            COMMS_ASSERT(!"Invalid field execution");
            return;
        }

        comms::util::tupleForSelectedType<Members>(memIdx_, makeConstExecHelper(std::forward<TFunc>(func)));
    }

    template <std::size_t TIdx, typename... TArgs>
    typename std::tuple_element<TIdx, Members>::type& initField(TArgs&&... args)
    {
        static_assert(isIdxValid(TIdx), "Only valid field index can be used");
        checkDestruct();

        using FieldType = typename std::tuple_element<TIdx, Members>::type;
        new (&storage_) FieldType(std::forward<TArgs>(args)...);
        memIdx_ = TIdx;
        updateVersionInternal(VersionTag());
        return reinterpret_cast<FieldType&>(storage_);
    }

    template <std::size_t TIdx>
    typename std::tuple_element<TIdx, Members>::type& accessField()
    {
        static_assert(isIdxValid(TIdx), "Only valid field index can be used");
        COMMS_ASSERT(TIdx == memIdx_); // Accessing non initialised field

        using FieldType = typename std::tuple_element<TIdx, Members>::type;
        return reinterpret_cast<FieldType&>(storage_);
    }

    template <std::size_t TIdx>
    const typename std::tuple_element<TIdx, Members>::type& accessField() const
    {
        static_assert(isIdxValid(TIdx), "Something is wrong");
        COMMS_ASSERT(TIdx == memIdx_); // Accessing non initialised field

        using FieldType = typename std::tuple_element<TIdx, Members>::type;
        return reinterpret_cast<const FieldType&>(storage_);
    }

    bool currentFieldValid() const
    {
        return isIdxValid(memIdx_);
    }

    void reset()
    {
        checkDestruct();
        COMMS_ASSERT(!currentFieldValid());
    }

    static constexpr bool isVersionDependent()
    {
        return details::variantIsAnyMemberVersionDependent<Members>();
    }

    bool setVersion(VersionType version)
    {
        return setVersionInternal(version, VersionTag());
    }

    VersionType getVersion() const
    {
        return getVersionInternal(VersionTag());
    }

private:
    struct VersionDependentTag {};
    struct NoVersionDependencyTag {};

    using VersionTag =
        typename std::conditional<
            details::variantIsAnyMemberVersionDependent<Members>(),
            VersionDependentTag,
            NoVersionDependencyTag
        >::type;


    class ConstructHelper
    {
    public:
        ConstructHelper(void* storage) : storage_(storage) {}

        template <std::size_t TIdx, typename TField>
        void operator()() const
        {
            new (storage_) TField;
        }
    private:
        void* storage_ = nullptr;
    };

    class CopyConstructHelper
    {
    public:
        CopyConstructHelper(void* storage, const void* other) : storage_(storage), other_(other) {}

        template <std::size_t TIdx, typename TField>
        void operator()() const
        {
            new (storage_) TField(*(reinterpret_cast<const TField*>(other_)));
        }

    private:
        void* storage_ = nullptr;
        const void* other_ = nullptr;
    };

    class MoveConstructHelper
    {
    public:
        MoveConstructHelper(void* storage, void* other) : storage_(storage), other_(other) {}

        template <std::size_t TIdx, typename TField>
        void operator()() const
        {
            new (storage_) TField(std::move(*(reinterpret_cast<const TField*>(other_))));
        }

    private:
        void* storage_ = nullptr;
        void* other_ = nullptr;
    };

    class DestructHelper
    {
    public:
        DestructHelper(void* storage) : storage_(storage) {}

        template <std::size_t TIdx, typename TField>
        void operator()() const
        {
            reinterpret_cast<TField*>(storage_)->~TField();
        }
    private:
        void* storage_ = nullptr;
    };

    class LengthCalcHelper
    {
    public:
        LengthCalcHelper(std::size_t& len, const void* storage)
          : len_(len),
            storage_(storage)
        {
        }

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
            len_ = reinterpret_cast<const TField*>(storage_)->length();
        }

    private:
        std::size_t& len_;
        const void* storage_;
    };

    struct MaxLengthCalcHelper
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t val) const
        {
            return val >= TField::maxLength() ? val : TField::maxLength();
        }
    };

    class ValidCheckHelper
    {
    public:
        ValidCheckHelper(bool& result, const void* storage)
          : result_(result),
            storage_(storage)
        {
        }

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
            result_ = reinterpret_cast<const TField*>(storage_)->valid();
        }

    private:
        bool& result_;
        const void* storage_;
    };

    class RefreshHelper
    {
    public:
        RefreshHelper(bool& result, void* storage)
          : result_(result),
            storage_(storage)
        {
        }

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
            result_ = reinterpret_cast<TField*>(storage_)->refresh();
        }

    private:
        bool& result_;
        void* storage_ = nullptr;
    };


    template <typename TFunc>
    class ExecHelper
    {
        static_assert(std::is_lvalue_reference<TFunc>::value || std::is_rvalue_reference<TFunc>::value,
            "Wrong type of template parameter");
    public:
        template <typename U>
        ExecHelper(void* storage, U&& func) : storage_(storage), func_(std::forward<U>(func)) {}

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
#if COMMS_IS_MSVC
            // VS compiler
            func_.operator()<TIdx>(*(reinterpret_cast<TField*>(storage_)));
#else // #if COMMS_IS_MSVC
            func_.template operator()<TIdx>(*(reinterpret_cast<TField*>(storage_)));
#endif // #if COMMS_IS_MSVC
        }
    private:
        void* storage_ = nullptr;
        TFunc func_;
    };

    template <typename TFunc>
    auto makeExecHelper(TFunc&& func) -> ExecHelper<decltype(std::forward<TFunc>(func))>
    {
        using FuncType = decltype(std::forward<TFunc>(func));
        return ExecHelper<FuncType>(&storage_, std::forward<TFunc>(func));
    }

    template <typename TFunc>
    class ConstExecHelper
    {
        static_assert(std::is_lvalue_reference<TFunc>::value || std::is_rvalue_reference<TFunc>::value,
            "Wrong type of template parameter");
    public:
        template <typename U>
        ConstExecHelper(const void* storage, U&& func) : storage_(storage), func_(std::forward<U>(func)) {}

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
#if COMMS_IS_MSVC
            // VS compiler
            func_.operator()<TIdx>(*(reinterpret_cast<const TField*>(storage_)));
#else // #if COMMS_IS_MSVC
            func_.template operator()<TIdx>(*(reinterpret_cast<const TField*>(storage_)));
#endif // #if COMMS_IS_MSVC
        }
    private:
        const void* storage_ = nullptr;
        TFunc func_;
    };

    template <typename TFunc>
    auto makeConstExecHelper(TFunc&& func) const -> ConstExecHelper<decltype(std::forward<TFunc>(func))>
    {
        using FuncType = decltype(std::forward<TFunc>(func));
        return ConstExecHelper<FuncType>(&storage_, std::forward<TFunc>(func));
    }

    template <typename TIter, typename TVerBase>
    class ReadHelper
    {
    public:
        ReadHelper(
            std::size_t& idx,
            comms::ErrorStatus& es,
            TIter& iter,
            std::size_t len,
            void* storage,
            TVerBase& verBase)
          : idx_(idx),
            es_(es),
            iter_(iter),
            len_(len),
            storage_(storage),
            verBase_(verBase)
        {
            using IterType = typename std::decay<decltype(iter)>::type;
            using IterCategory = typename std::iterator_traits<IterType>::iterator_category;
            static_assert(std::is_base_of<std::random_access_iterator_tag, IterCategory>::value,
                "Variant field only supports read with random access iterators");

            es_ = comms::ErrorStatus::NumOfErrorStatuses;
        }

        template <typename TField>
        void operator()()
        {
            if (readComplete_) {
                return;
            }

            auto* field = new (storage_) TField;
            updateVersion(*field, VersionTag());

            auto iterTmp = iter_;
            auto es = field->read(iterTmp, len_);
            if (es == comms::ErrorStatus::Success) {
                iter_ = iterTmp;
                es_ = es;
                readComplete_ = true;
                return;
            }

            field->~TField();

            if ((es_ == comms::ErrorStatus::NumOfErrorStatuses) ||
                (es == comms::ErrorStatus::NotEnoughData)) {
                es_ = es;
            }

            ++idx_;
        }

    private:
        template <typename TField>
        void updateVersion(TField& field, NoVersionDependencyTag)
        {
            static_cast<void>(field);
        }

        template <typename TField>
        void updateVersion(TField& field, VersionDependentTag)
        {
            field.setVersion(verBase_.getVersion());
        }

        std::size_t& idx_;
        comms::ErrorStatus& es_;
        TIter& iter_;
        std::size_t len_ = 0;
        void* storage_ = nullptr;
        TVerBase& verBase_;
        bool readComplete_ = false;
    };

    template <typename TIter, typename TVerBase>
    ReadHelper<TIter, TVerBase> makeReadHelper(
        comms::ErrorStatus& es,
        TIter& iter,
        std::size_t len,
        void* storage,
        TVerBase& verBase)
    {
        memIdx_ = 0;
        return ReadHelper<TIter, TVerBase>(memIdx_, es, iter, len, storage, verBase);
    }

    template <typename TIter>
    class WriteHelper
    {
    public:
        WriteHelper(ErrorStatus& es, TIter& iter, std::size_t len, const void* storage)
          : es_(es),
            iter_(iter),
            len_(len),
            storage_(storage)
        {
        }

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
            es_ = reinterpret_cast<const TField*>(storage_)->write(iter_, len_);
        }

    private:
        ErrorStatus& es_;
        TIter& iter_;
        std::size_t len_ = 0U;
        const void* storage_ = nullptr;
    };

    template <typename TIter>
    static WriteHelper<TIter> makeWriteHelper(comms::ErrorStatus& es, TIter& iter, std::size_t len, const void* storage)
    {
        return WriteHelper<TIter>(es, iter, len, storage);
    }

    template <typename TIter>
    class WriteNoStatusHelper
    {
    public:
        WriteNoStatusHelper(TIter& iter, const void* storage)
          : iter_(iter),
            storage_(storage)
        {
        }

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
            reinterpret_cast<const TField*>(storage_)->writeNoStatus(iter_);
        }

    private:
        TIter& iter_;
        const void* storage_ = nullptr;
    };

    template <typename TIter>
    static WriteNoStatusHelper<TIter> makeWriteNoStatusHelper(TIter& iter, const void* storage)
    {
        return WriteNoStatusHelper<TIter>(iter, storage);
    }

    class SetVersionHelper
    {
    public:
        SetVersionHelper(VersionType version, bool& updated, void* storage)
          :version_(version), updated_(updated), storage_(storage)
        {
        }

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
            updated_ = reinterpret_cast<TField*>(storage_)->setVersion(version_) || updated_;
        }

    private:
        VersionType version_ = VersionType();
        bool& updated_;
        void* storage_ = nullptr;
    };

    class CanWriteHelper
    {
    public:
        CanWriteHelper(bool& result, const void* storage)
          : result_(result),
            storage_(storage)
        {
        }

        template <std::size_t TIdx, typename TField>
        void operator()()
        {
            result_ = reinterpret_cast<const TField*>(storage_)->canWrite();
        }

    private:
        bool& result_;
        const void* storage_;
    };

    struct WriteNoStatusDetector
    {
        constexpr WriteNoStatusDetector() = default;

        template <typename TField>
        constexpr bool operator()(bool soFar) const
        {
            return TField::hasWriteNoStatus() && soFar;
        }
    };

    void checkDestruct()
    {
        if (currentFieldValid()) {
            comms::util::tupleForSelectedType<Members>(memIdx_, DestructHelper(&storage_));
            memIdx_ = MembersCount;
        }
    }

    static constexpr bool isIdxValid(std::size_t idx)
    {
        return idx < MembersCount;
    }

    bool setVersionInternal(VersionType version, NoVersionDependencyTag)
    {
        static_cast<void>(version);
        return false;
    }

    bool setVersionInternal(VersionType version, VersionDependentTag)
    {
        VersionBaseImpl::version_ = version;
        bool updated = false;
        if (currentFieldValid()) {
            comms::util::tupleForSelectedType<Members>(memIdx_, SetVersionHelper(version, updated, &storage_));
        }
        return updated;
    }

    VersionType getVersionInternal(VersionDependentTag) const
    {
        return VersionBaseImpl::version_;;
    }

    void updateVersionInternal(NoVersionDependencyTag)
    {
    }

    void updateVersionInternal(VersionDependentTag)
    {
        setVersion(VersionBaseImpl::version_);
    }

    ValueType storage_;
    std::size_t memIdx_ = MembersCount;

    static const std::size_t MembersCount = std::tuple_size<Members>::value;
    static_assert(comms::util::IsTuple<Members>::Value, "ValueType must be tuple");
    static_assert(0U < MembersCount, "ValueType must be non-empty tuple");
};

}  // namespace basic

}  // namespace field

}  // namespace comms
