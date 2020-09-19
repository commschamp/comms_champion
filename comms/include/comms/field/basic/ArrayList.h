//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <algorithm>
#include <limits>
#include <numeric>

#include "comms/CompileControl.h"
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/access.h"
#include "comms/util/assign.h"
#include "comms/util/StaticVector.h"
#include "comms/util/StaticString.h"
#include "comms/util/detect.h"
#include "comms/util/type_traits.h"
#include "comms/details/detect.h"
#include "comms/details/tag.h"
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

template <typename TStorage>
struct ArrayListMaxLengthRetrieveHelper
{
    static const std::size_t Value = CommonFuncs::maxSupportedLength();
};

template <typename T, std::size_t TSize>
struct ArrayListMaxLengthRetrieveHelper<comms::util::StaticVector<T, TSize> >
{
    static const std::size_t Value = TSize;
};

template <std::size_t TSize>
struct ArrayListMaxLengthRetrieveHelper<comms::util::StaticString<TSize> >
{
    static const std::size_t Value = TSize - 1;
};

template <typename TElem>
using ArrayListFieldHasVarLengthBoolType = 
    typename comms::util::LazyDeepConditional<
        std::is_integral<TElem>::value
    >::template Type<
        comms::util::FalseType,
        comms::util::FieldCheckVarLength,
        TElem
    >;

template <typename TElem>
using HasArrayListElemNonDefaultRefreshBoolType = 
    typename comms::util::LazyDeepConditional<
        std::is_integral<TElem>::value
    >::template Type<
        comms::util::FalseType,
        comms::util::FieldCheckNonDefaultRefresh,
        TElem
    >;

template <typename TElem>
using IsArrayListElemVersionDependentBoolType = 
    typename comms::util::LazyDeepConditional<
        std::is_integral<TElem>::value
    >::template Type<
        comms::util::FalseType,
        comms::util::FieldCheckVersionDependent,
        TElem
    >;

template <typename TFieldBase, typename TStorage>
using ArrayListVersionStorageBase = 
    typename comms::util::LazyShallowConditional<
        IsArrayListElemVersionDependentBoolType<typename TStorage::value_type>::value
    >::template Type<
        comms::field::details::VersionStorage,
        comms::util::EmptyStruct,
        typename TFieldBase::VersionType
    >;

}  // namespace details

template <typename TFieldBase, typename TStorage>
class ArrayList :
        public TFieldBase,
        public details::ArrayListVersionStorageBase<TFieldBase, TStorage>
{
    using BaseImpl = TFieldBase;
    using VersionBaseImpl = details::ArrayListVersionStorageBase<TFieldBase, TStorage>;

public:
    using Endian = typename BaseImpl::Endian;
    using VersionType = typename BaseImpl::VersionType;
    using ElementType = typename TStorage::value_type;
    using ValueType = TStorage;

    ArrayList() = default;

    explicit ArrayList(const ValueType& val)
      : value_(val)
    {
    }

    explicit ArrayList(ValueType&& val)
      : value_(std::move(val))
    {
    }

    ArrayList(const ArrayList&) = default;
    ArrayList(ArrayList&&) = default;
    ArrayList& operator=(const ArrayList&) = default;
    ArrayList& operator=(ArrayList&&) = default;
    ~ArrayList() noexcept = default;

    const ValueType& value() const
    {
        return value_;
    }

    ValueType& value()
    {
        return value_;
    }

    ElementType& createBack()
    {
        value_.emplace_back();
        updateElemVersion(value_.back(), VersionTag<>());
        return value_.back();
    }

    void clear()
    {
        static_assert(comms::util::detect::hasClearFunc<ValueType>(),
            "The used storage type for ArrayList must have clear() member function");

        value_.clear();
    }

    constexpr std::size_t length() const
    {
        return lengthInternal(ElemTag<>());
    }

    static constexpr std::size_t minLength()
    {
        return 0U;
    }

    static constexpr std::size_t maxLength()
    {
        return
            details::ArrayListMaxLengthRetrieveHelper<TStorage>::Value *
            maxLengthInternal(ElemTag<>());
    }

    constexpr bool valid() const
    {
        return validInternal(ElemTag<>());
    }

    bool refresh()
    {
        return refreshInternal(ElemTag<>());
    }

    static constexpr std::size_t minElementLength()
    {
        return minElemLengthInternal(ElemTag<>());
    }

    static constexpr std::size_t maxElementLength()
    {
        return maxElemLengthInternal(ElemTag<>());
    }

    static constexpr std::size_t elementLength(const ElementType& elem)
    {
        return elementLengthInternal(elem, ElemTag<>());
    }

    template <typename TIter>
    static ErrorStatus readElement(ElementType& elem, TIter& iter, std::size_t& len)
    {
        return readElementInternal(elem, iter, len, ElemTag<>());
    }

    template <typename TIter>
    static void readElementNoStatus(ElementType& elem, TIter& iter)
    {
        return readElementNoStatusInternal(elem, iter, ElemTag<>());
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterCategory =
            typename std::iterator_traits<IterType>::iterator_category;
        static const bool IsRandomAccessIter =
            std::is_base_of<std::random_access_iterator_tag, IterCategory>::value;
        static const bool IsRawData =
            std::is_integral<ElementType>::value && (sizeof(ElementType) == sizeof(std::uint8_t));

        using Tag =
            typename comms::util::LazyShallowConditional<
                IsRandomAccessIter && IsRawData
            >::template Type<
                RawDataTag,
                FieldElemTag
            >;
        return readInternal(iter, len, Tag());
    }

    static constexpr bool hasReadNoStatus()
    {
        return false;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterCategory =
            typename std::iterator_traits<IterType>::iterator_category;
        static const bool IsRandomAccessIter =
            std::is_base_of<std::random_access_iterator_tag, IterCategory>::value;
        static const bool IsRawData =
            std::is_integral<ElementType>::value && (sizeof(ElementType) == sizeof(std::uint8_t));

        using Tag =
            typename comms::util::LazyShallowConditional<
                IsRandomAccessIter && IsRawData
            >::template Type<
                RawDataTag,
                FieldElemTag
            >;

        return readInternalN(count, iter, len, Tag());
    }

    template <typename TIter>
    void readNoStatusN(std::size_t count, TIter& iter)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterCategory =
            typename std::iterator_traits<IterType>::iterator_category;
        static const bool IsRandomAccessIter =
            std::is_base_of<std::random_access_iterator_tag, IterCategory>::value;
        static const bool IsRawData =
            std::is_integral<ElementType>::value && (sizeof(ElementType) == sizeof(std::uint8_t));

        using Tag =
            typename comms::util::LazyShallowConditional<
                IsRandomAccessIter && IsRawData
            >::template Type<
                RawDataTag,
                FieldElemTag
            >;

        return readNoStatusInternalN(count, iter, Tag());
    }

    static bool canWriteElement(const ElementType& elem)
    {
        return canWriteElementInternal(elem, ElemTag<>());
    }

    template <typename TIter>
    static ErrorStatus writeElement(const ElementType& elem, TIter& iter, std::size_t& len)
    {
        return writeElementInternal(elem, iter, len, ElemTag<>());
    }

    template <typename TIter>
    static void writeElementNoStatus(const ElementType& elem, TIter& iter)
    {
        return writeElementNoStatusInternal(elem, iter, ElemTag<>());
    }

    bool canWrite() const
    {
        return CommonFuncs::canWriteSequence(*this);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return CommonFuncs::writeSequence(*this, iter, len);
    }

    static constexpr bool hasWriteNoStatus()
    {
        return hasWriteNoStatusInternal(ElemTag<>());
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        CommonFuncs::writeSequenceNoStatus(*this, iter);
    }

    template <typename TIter>
    ErrorStatus writeN(std::size_t count, TIter& iter, std::size_t& len) const
    {
        return CommonFuncs::writeSequenceN(*this, count, iter, len);
    }

    template <typename TIter>
    void writeNoStatusN(std::size_t count, TIter& iter) const
    {
        CommonFuncs::writeSequenceNoStatusN(*this, count, iter);
    }

    static constexpr bool isVersionDependent()
    {
        return details::IsArrayListElemVersionDependentBoolType<ElementType>::value;
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return details::HasArrayListElemNonDefaultRefreshBoolType<ElementType>::value;
    }

    bool setVersion(VersionType version)
    {
        return setVersionInternal(version, VersionTag<>());
    }

private:
    template <typename... TParams>
    using FieldElemTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using IntegralElemTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using FixedLengthTag = comms::details::tag::Tag3<>;

    template <typename... TParams>
    using VarLengthTag = comms::details::tag::Tag4<>;

    template <typename... TParams>
    using RawDataTag = comms::details::tag::Tag5<>;

    template <typename... TParams>
    using VersionDependentTag = comms::details::tag::Tag6<>;    

    template <typename... TParams>
    using NoVersionDependencyTag = comms::details::tag::Tag7<>;

    template <typename... TParams>
    using ElemTag = 
        typename comms::util::Conditional<
            std::is_integral<ElementType>::value
        >::template Type<
            IntegralElemTag<TParams...>,
            FieldElemTag<TParams...>
        >;

    template <typename... TParams>
    using VersionTag =
        typename comms::util::Conditional<
            details::IsArrayListElemVersionDependentBoolType<ElementType>::value
        >::template Type<
            VersionDependentTag<TParams...>,
            NoVersionDependencyTag<TParams...>
        >;

    template <typename... TParams>
    constexpr std::size_t lengthInternal(FieldElemTag<TParams...>) const
    {
        using Tag = 
            typename comms::util::LazyShallowConditional<
                details::ArrayListFieldHasVarLengthBoolType<ElementType>::value
            >::template Type<
                VarLengthTag,
                FixedLengthTag
            >;

        return fieldLength(Tag());
    }

    template <typename... TParams>
    constexpr std::size_t lengthInternal(IntegralElemTag<TParams...>) const
    {
        return value_.size() * sizeof(ElementType);
    }

    template <typename... TParams>
    constexpr std::size_t fieldLength(FixedLengthTag<TParams...>) const
    {
        return ElementType().length() * value_.size();
    }

    template <typename... TParams>
    std::size_t fieldLength(VarLengthTag<TParams...>) const
    {
        return
            std::accumulate(value_.begin(), value_.end(), std::size_t(0),
                [](std::size_t sum, typename ValueType::const_reference e) -> std::size_t
                {
                    return sum + e.length();
                });
    }

    template <typename... TParams>
    static constexpr std::size_t maxLengthInternal(FieldElemTag<TParams...>)
    {
        return ElementType::maxLength();
    }

    template <typename... TParams>
    static constexpr std::size_t maxLengthInternal(IntegralElemTag<TParams...>)
    {
        return sizeof(ElementType);
    }

    template <typename TIter>
    static ErrorStatus readFieldElement(ElementType& elem, TIter& iter, std::size_t& len)
    {
        auto fromIter = iter;
        auto es = elem.read(iter, len);
        if (es == ErrorStatus::Success) {
            auto diff = static_cast<std::size_t>(std::distance(fromIter, iter));
            COMMS_ASSERT(diff <= len);
            len -= diff;
        }
        return es;
    }

    template <typename TIter>
    static ErrorStatus readIntegralElement(ElementType& elem, TIter& iter, std::size_t& len)
    {
        if (len < sizeof(ElementType)) {
            return ErrorStatus::NotEnoughData;
        }

        elem = comms::util::readData<ElementType>(iter,  Endian());
        len -= sizeof(ElementType);
        return ErrorStatus::Success;
    }

    template <typename TIter, typename... TParams>
    static ErrorStatus readElementInternal(ElementType& elem, TIter& iter, std::size_t& len, FieldElemTag<TParams...>)
    {
        return readFieldElement(elem, iter, len);
    }

    template <typename TIter, typename... TParams>
    static ErrorStatus readElementInternal(ElementType& elem, TIter& iter, std::size_t& len, IntegralElemTag<TParams...>)
    {
        return readIntegralElement(elem, iter, len);
    }

    template <typename TIter>
    static void readNoStatusFieldElement(ElementType& elem, TIter& iter)
    {
        elem.readNoStatus(iter);
    }

    template <typename TIter>
    static void readNoStatusIntegralElement(ElementType& elem, TIter& iter)
    {
        elem = comms::util::readData<ElementType>(iter, Endian());
    }

    template <typename TIter, typename... TParams>
    static void readElementNoStatusInternal(ElementType& elem, TIter& iter, FieldElemTag<TParams...>)
    {
        readNoStatusFieldElement(elem, iter);
    }

    template <typename TIter, typename... TParams>
    static void readElementNoStatusInternal(ElementType& elem, TIter& iter, IntegralElemTag<TParams...>)
    {
        readElementNoStatusInternal(elem, iter);
    }

    template <typename TIter>
    static ErrorStatus writeFieldElement(const ElementType& elem, TIter& iter, std::size_t& len)
    {
        auto es = elem.write(iter, len);
        if (es == ErrorStatus::Success) {
            len -= elem.length();
        }
        return es;
    }

    template <typename TIter>
    static ErrorStatus writeIntegralElement(const ElementType& elem, TIter& iter, std::size_t& len)
    {
        if (len < sizeof(ElementType)) {
            return ErrorStatus::BufferOverflow;
        }

        BaseImpl::writeData(elem, iter);
        len -= sizeof(ElementType);
        return ErrorStatus::Success;
    }

    template <typename TIter, typename... TParams>
    static ErrorStatus writeElementInternal(const ElementType& elem, TIter& iter, std::size_t& len, FieldElemTag<TParams...>)
    {
        return writeFieldElement(elem, iter, len);
    }

    template <typename TIter, typename... TParams>
    static ErrorStatus writeElementInternal(const ElementType& elem, TIter& iter, std::size_t& len, IntegralElemTag<TParams...>)
    {
        return writeIntegralElement(elem, iter, len);
    }

    template <typename TIter>
    static void writeNoStatusFieldElement(const ElementType& elem, TIter& iter)
    {
        elem.writeNoStatus(iter);
    }

    template <typename TIter>
    static void writeNoStatusIntegralElement(const ElementType& elem, TIter& iter)
    {
        BaseImpl::writeData(elem, iter);
    }

    template <typename TIter, typename... TParams>
    static void writeElementNoStatusInternal(const ElementType& elem, TIter& iter, FieldElemTag<TParams...>)
    {
        return writeNoStatusFieldElement(elem, iter);
    }

    template <typename TIter, typename... TParams>
    static void writeElementNoStatusInternal(const ElementType& elem, TIter& iter, IntegralElemTag<TParams...>)
    {
        return writeNoStatusIntegralElement(elem, iter);
    }

    template <typename... TParams>
    constexpr bool validInternal(FieldElemTag<TParams...>) const
    {
        return std::all_of(
            value_.begin(), value_.end(),
            [](const ElementType& e) -> bool
            {
                return e.valid();
            });
    }

    template <typename... TParams>
    static constexpr bool validInternal(IntegralElemTag<TParams...>)
    {
        return true;
    }

    template <typename... TParams>
    bool refreshInternal(FieldElemTag<TParams...>)
    {
        return
            std::accumulate(
                value_.begin(), value_.end(), false,
                [](bool prev, typename ValueType::reference elem) -> bool
                {
                    return elem.refresh() || prev;
                });
    }

    template <typename... TParams>
    static constexpr bool refreshInternal(IntegralElemTag<TParams...>)
    {
        return false;
    }

    template <typename... TParams>
    static constexpr std::size_t minElemLengthInternal(IntegralElemTag<TParams...>)
    {
        return sizeof(ElementType);
    }

    template <typename... TParams>
    static constexpr std::size_t minElemLengthInternal(FieldElemTag<TParams...>)
    {
        return ElementType::minLength();
    }

    template <typename... TParams>
    static constexpr std::size_t maxElemLengthInternal(IntegralElemTag<TParams...>)
    {
        return sizeof(ElementType);
    }

    template <typename... TParams>
    static constexpr std::size_t maxElemLengthInternal(FieldElemTag<TParams...>)
    {
        return ElementType::maxLength();
    }

    template <typename... TParams>
    static constexpr std::size_t elementLengthInternal(const ElementType&, IntegralElemTag<TParams...>)
    {
        return sizeof(ElementType);
    }

    template <typename... TParams>
    static constexpr std::size_t elementLengthInternal(const ElementType& elem, FieldElemTag<TParams...>)
    {
        return elem.length();
    }

    template <typename TIter, typename... TParams>
    ErrorStatus readInternal(TIter& iter, std::size_t len, FieldElemTag<TParams...>)
    {
        static_assert(comms::util::detect::hasClearFunc<ValueType>(),
            "The used storage type for ArrayList must have clear() member function");
        value_.clear();
        auto remLen = len;
        while (0 < remLen) {
            ElementType& elem = createBack();
            auto es = readElement(elem, iter, remLen);
            if (es != ErrorStatus::Success) {
                value_.pop_back();
                return es;
            }
        }

        return ErrorStatus::Success;
    }

    template <typename TIter, typename... TParams>
    ErrorStatus readInternal(TIter& iter, std::size_t len, RawDataTag<TParams...>)
    {
        comms::util::assign(value(), iter, iter + len);
        std::advance(iter, len);
        return ErrorStatus::Success;
    }

    template <typename TIter, typename... TParams>
    ErrorStatus readInternalN(std::size_t count, TIter& iter, std::size_t len, FieldElemTag<TParams...>)
    {
        clear();
        while (0 < count) {
            auto& elem = createBack();
            auto es = readElement(elem, iter, len);
            if (es != ErrorStatus::Success) {
                value_.pop_back();
                return es;
            }

            --count;
        }

        return ErrorStatus::Success;
    }

    template <typename TIter, typename... TParams>
    ErrorStatus readInternalN(std::size_t count, TIter& iter, std::size_t len, RawDataTag<TParams...>)
    {
        if (len < count) {
            return comms::ErrorStatus::NotEnoughData;
        }

        return readInternal(iter, count, RawDataTag<>());
    }

    template <typename TIter, typename... TParams>
    void readNoStatusInternalN(std::size_t count, TIter& iter, FieldElemTag<TParams...>)
    {
        clear();
        while (0 < count) {
            auto& elem = createBack();
            readElementNoStatus(elem, iter);
            --count;
        }
    }

    template <typename TIter, typename... TParams>
    void readNoStatusInternalN(std::size_t count, TIter& iter, RawDataTag<TParams...>)
    {
        readInternal(iter, count, RawDataTag<>());
    }

    template <typename... TParams>
    bool updateElemVersion(ElementType& elem, VersionDependentTag<TParams...>)
    {
        return elem.setVersion(VersionBaseImpl::version_);
    }

    template <typename... TParams>
    static constexpr bool updateElemVersion(ElementType&, NoVersionDependencyTag<TParams...>)
    {
        return false;
    }

    template <typename... TParams>
    bool setVersionInternal(VersionType version, VersionDependentTag<TParams...>)
    {
        VersionBaseImpl::version_ = version;
        bool updated = false;
        for (auto& elem : value()) {
            updated = elem.setVersion(version) || updated;
        }

        return updated;
    }

    template <typename... TParams>
    static constexpr bool setVersionInternal(VersionType, NoVersionDependencyTag<TParams...>)
    {
        return false;
    }

    template <typename... TParams>
    static bool canWriteElementInternal(const ElementType& elem, FieldElemTag<TParams...>)
    {
        return elem.canWrite();
    }

    template <typename... TParams>
    static bool canWriteElementInternal(const ElementType& elem, IntegralElemTag<TParams...>)
    {
        static_cast<void>(elem);
        return true;
    }

    template <typename... TParams>
    static constexpr bool hasWriteNoStatusInternal(FieldElemTag<TParams...>)
    {
        return ElementType::hasWriteNoStatus();
    }

    template <typename... TParams>
    static constexpr bool hasWriteNoStatusInternal(IntegralElemTag<TParams...>)
    {
        return true;
    }

    ValueType value_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


