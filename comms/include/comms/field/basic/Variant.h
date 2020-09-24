//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <algorithm>

#include "comms/CompileControl.h"
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/Tuple.h"
#include "comms/util/type_traits.h"
#include "comms/field/details/VersionStorage.h"
#include "comms/field/details/FieldOpHelpers.h"
#include "comms/details/tag.h"
#include "CommonFuncs.h"

namespace comms
{

namespace field
{

namespace basic
{

namespace details
{

template<typename...>
class VariantFieldConstructHelper
{
public:
    VariantFieldConstructHelper(void* storage) : storage_(storage) {}

    template <std::size_t TIdx, typename TField>
    void operator()() const
    {
        new (storage_) TField;
    }
private:
    void* storage_ = nullptr;
};

template<typename...>
class VariantLengthCalcHelper
{
public:
    VariantLengthCalcHelper(std::size_t& len, const void* storage)
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

template<typename...>
class VariantFieldCopyConstructHelper
{
public:
    VariantFieldCopyConstructHelper(void* storage, const void* other) : storage_(storage), other_(other) {}

    template <std::size_t TIdx, typename TField>
    void operator()() const
    {
        new (storage_) TField(*(reinterpret_cast<const TField*>(other_)));
    }

private:
    void* storage_ = nullptr;
    const void* other_ = nullptr;
};

template<typename...>
class VariantFieldMoveConstructHelper
{
public:
    VariantFieldMoveConstructHelper(void* storage, void* other) : storage_(storage), other_(other) {}

    template <std::size_t TIdx, typename TField>
    void operator()() const
    {
        new (storage_) TField(std::move(*(reinterpret_cast<const TField*>(other_))));
    }

private:
    void* storage_ = nullptr;
    void* other_ = nullptr;
};

template<typename...>
class VariantFieldDestructHelper
{
public:
    VariantFieldDestructHelper(void* storage) : storage_(storage) {}

    template <std::size_t TIdx, typename TField>
    void operator()() const
    {
        reinterpret_cast<TField*>(storage_)->~TField();
    }
private:
    void* storage_ = nullptr;
};

template <typename...>
class VariantFieldValidCheckHelper
{
public:
    VariantFieldValidCheckHelper(bool& result, const void* storage)
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

template <typename...>
class VariantFieldRefreshHelper
{
public:
    VariantFieldRefreshHelper(bool& result, void* storage)
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
class VariantExecHelper
{
    static_assert(std::is_lvalue_reference<TFunc>::value || std::is_rvalue_reference<TFunc>::value,
        "Wrong type of template parameter");
public:
    template <typename U>
    VariantExecHelper(void* storage, U&& func) : storage_(storage), func_(std::forward<U>(func)) {}

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
class VariantConstExecHelper
{
    static_assert(std::is_lvalue_reference<TFunc>::value || std::is_rvalue_reference<TFunc>::value,
        "Wrong type of template parameter");
public:
    template <typename U>
    VariantConstExecHelper(const void* storage, U&& func) : storage_(storage), func_(std::forward<U>(func)) {}

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


template <typename TIter, typename TVerBase, bool TVerDependent>
class VariantReadHelper
{
    template <typename... TParams>
    using VersionDependentTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using NoVersionDependencyTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using VersionTag =
        typename comms::util::LazyShallowConditional<
            TVerDependent
        >::template Type<
            VersionDependentTag,
            NoVersionDependencyTag
        >;    
public:
    VariantReadHelper(
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
        updateVersion(*field, VersionTag<>());

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
    template <typename TField, typename... TParams>
    void updateVersion(TField& field, NoVersionDependencyTag<TParams...>)
    {
        static_cast<void>(field);
    }

    template <typename TField, typename... TParams>
    void updateVersion(TField& field, VersionDependentTag<TParams...>)
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

template <typename TIter>
class VariantFieldWriteHelper
{
public:
    VariantFieldWriteHelper(ErrorStatus& es, TIter& iter, std::size_t len, const void* storage)
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
class VariantWriteNoStatusHelper
{
public:
    VariantWriteNoStatusHelper(TIter& iter, const void* storage)
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

template <typename TVersionType>
class VariantSetVersionHelper
{
public:
    VariantSetVersionHelper(TVersionType version, bool& updated, void* storage)
      : version_(version), 
        updated_(updated), 
        storage_(storage)
    {
    }

    template <std::size_t TIdx, typename TField>
    void operator()()
    {
        updated_ = reinterpret_cast<TField*>(storage_)->setVersion(version_) || updated_;
    }

private:
    TVersionType version_ = TVersionType();
    bool& updated_;
    void* storage_ = nullptr;
};

template<typename...>
class VariantCanWriteHelper
{
public:
    VariantCanWriteHelper(bool& result, const void* storage)
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

template <typename TFieldBase, typename... TMembers>
using VariantVersionStorageBase = 
    typename comms::util::LazyShallowConditional<
        CommonFuncs::IsAnyFieldVersionDependentBoolType<TMembers...>::value
    >::template Type<
        comms::field::details::VersionStorage,
        comms::util::EmptyStruct,
        typename TFieldBase::VersionType
    >;

} // namespace details

template <typename TFieldBase, typename TMembers>
class Variant;

template <typename TFieldBase, typename... TMembers>
class Variant<TFieldBase, std::tuple<TMembers...> > :
        public TFieldBase,
        public details::VariantVersionStorageBase<TFieldBase, TMembers...>
{
    using BaseImpl = TFieldBase;
    using VersionBaseImpl = details::VariantVersionStorageBase<TFieldBase, TMembers...>;

public:
    using Members = std::tuple<TMembers...>;
    using ValueType = comms::util::TupleAsAlignedUnionT<Members>;
    using VersionType = typename BaseImpl::VersionType;

    static const std::size_t MembersCount = std::tuple_size<Members>::value;
    static_assert(0U < MembersCount, "ValueType must be non-empty tuple");

    Variant() = default;
    Variant(const ValueType& val)  : storage_(val) {}
    Variant(ValueType&& val)  : storage_(std::move(val)) {}

    Variant(const Variant& other)
    {
        if (!other.currentFieldValid()) {
            return;
        }

        comms::util::tupleForSelectedType<Members>(
            other.memIdx_, details::VariantFieldCopyConstructHelper<>(&storage_, &other.storage_));

        memIdx_ = other.memIdx_;
    }

    Variant(Variant&& other)
    {
        if (!other.currentFieldValid()) {
            return;
        }

        comms::util::tupleForSelectedType<Members>(
            other.memIdx_, details::VariantFieldMoveConstructHelper<>(&storage_, &other.storage_));

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
            other.memIdx_, details::VariantFieldCopyConstructHelper<>(&storage_, &other.storage_));

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
            other.memIdx_, details::VariantFieldMoveConstructHelper<>(&storage_, &other.storage_));

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
        comms::util::tupleForSelectedType<Members>(memIdx_, details::VariantLengthCalcHelper<>(len, &storage_));
        return len;
    }

    static constexpr std::size_t minLength()
    {
        return 0U;
    }

    static constexpr std::size_t maxLength()
    {
        return CommonFuncs::FieldSelectMaxLengthIntType<TMembers...>::value;
    }

    bool valid() const
    {
        if (!currentFieldValid()) {
            return false;
        }

        bool val = false;
        comms::util::tupleForSelectedType<Members>(
            memIdx_, details::VariantFieldValidCheckHelper<>(val, &storage_));
        return val;
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return CommonFuncs::AnyFieldHasNonDefaultRefreshBoolType<TMembers...>::value;
    }

    bool refresh()
    {
        if (!currentFieldValid()) {
            return false;
        }

        bool val = false;
        comms::util::tupleForSelectedType<Members>(
            memIdx_, details::VariantFieldRefreshHelper<>(val, &storage_));
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
        comms::util::tupleForSelectedType<Members>(
            memIdx_, details::VariantCanWriteHelper<>(val, &storage_));
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
        return comms::util::tupleTypeAccumulate<Members>(
            true, comms::field::details::FieldHasWriteNoStatusHelper<>());
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

        comms::util::tupleForSelectedType<Members>(
            idx, details::VariantFieldConstructHelper<>(&storage_));
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
        updateVersionInternal(VersionTag<>());
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
        return CommonFuncs::IsAnyFieldVersionDependentBoolType<TMembers...>::value;
    }

    bool setVersion(VersionType version)
    {
        return setVersionInternal(version, VersionTag<>());
    }

    VersionType getVersion() const
    {
        return getVersionInternal(VersionTag<>());
    }

private:
    template <typename... TParams>
    using VersionDependentTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using NoVersionDependencyTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using VersionTag =
        typename comms::util::LazyShallowConditional<
            CommonFuncs::IsAnyFieldVersionDependentBoolType<TMembers...>::value
        >::template Type<
            VersionDependentTag,
            NoVersionDependencyTag
        >;


    template <typename TFunc>
    auto makeExecHelper(TFunc&& func) -> details::VariantExecHelper<decltype(std::forward<TFunc>(func))>
    {
        using FuncType = decltype(std::forward<TFunc>(func));
        return details::VariantExecHelper<FuncType>(&storage_, std::forward<TFunc>(func));
    }

    template <typename TFunc>
    auto makeConstExecHelper(TFunc&& func) const -> details::VariantConstExecHelper<decltype(std::forward<TFunc>(func))>
    {
        using FuncType = decltype(std::forward<TFunc>(func));
        return details::VariantConstExecHelper<FuncType>(&storage_, std::forward<TFunc>(func));
    }

    template <typename TIter, typename TVerBase>
    details::VariantReadHelper<TIter, TVerBase, CommonFuncs::IsAnyFieldVersionDependentBoolType<TMembers...>::value> 
    makeReadHelper(
        comms::ErrorStatus& es,
        TIter& iter,
        std::size_t len,
        void* storage,
        TVerBase& verBase)
    {
        memIdx_ = 0;
        static constexpr bool VerDependent = isVersionDependent();
        return 
            details::VariantReadHelper<TIter, TVerBase, VerDependent>(
                memIdx_, es, iter, len, storage, verBase);
    }

    template <typename TIter>
    static details::VariantFieldWriteHelper<TIter> makeWriteHelper(comms::ErrorStatus& es, TIter& iter, std::size_t len, const void* storage)
    {
        return details::VariantFieldWriteHelper<TIter>(es, iter, len, storage);
    }

    template <typename TIter>
    static details::VariantWriteNoStatusHelper<TIter> makeWriteNoStatusHelper(TIter& iter, const void* storage)
    {
        return details::VariantWriteNoStatusHelper<TIter>(iter, storage);
    }

    void checkDestruct()
    {
        if (currentFieldValid()) {
            comms::util::tupleForSelectedType<Members>(
                memIdx_, details::VariantFieldDestructHelper<>(&storage_));
            memIdx_ = MembersCount;
        }
    }

    static constexpr bool isIdxValid(std::size_t idx)
    {
        return idx < MembersCount;
    }

    template <typename... TParams>
    bool setVersionInternal(VersionType version, NoVersionDependencyTag<TParams...>)
    {
        static_cast<void>(version);
        return false;
    }

    template <typename... TParams>
    bool setVersionInternal(VersionType version, VersionDependentTag<TParams...>)
    {
        VersionBaseImpl::version_ = version;
        bool updated = false;
        if (currentFieldValid()) {
            comms::util::tupleForSelectedType<Members>(
                memIdx_, details::VariantSetVersionHelper<VersionType>(version, updated, &storage_));
        }
        return updated;
    }

    template <typename... TParams>
    VersionType getVersionInternal(VersionDependentTag<TParams...>) const
    {
        return VersionBaseImpl::version_;;
    }

    template <typename... TParams>
    void updateVersionInternal(NoVersionDependencyTag<TParams...>)
    {
    }

    template <typename... TParams>
    void updateVersionInternal(VersionDependentTag<TParams...>)
    {
        setVersion(VersionBaseImpl::version_);
    }

    ValueType storage_;
    std::size_t memIdx_ = MembersCount;
};

}  // namespace basic

}  // namespace field

}  // namespace comms
