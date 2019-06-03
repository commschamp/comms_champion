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
#include <algorithm>
#include <limits>
#include <numeric>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/access.h"
#include "comms/util/StaticVector.h"
#include "comms/util/StaticString.h"
#include "comms/details/detect.h"
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

template <typename TElemType, bool TIntegral>
struct ArrayListFieldHasVarLengthHelper;

template <typename TElemType>
struct ArrayListFieldHasVarLengthHelper<TElemType, true>
{
    static const bool Value = false;
};

template <typename TElemType>
struct ArrayListFieldHasVarLengthHelper<TElemType, false>
{
    static const bool Value = TElemType::minLength() != TElemType::maxLength();
};

template <typename TElemType>
struct ArrayListFieldHasVarLength
{
    static const bool Value =
        ArrayListFieldHasVarLengthHelper<
            TElemType,
            std::is_integral<TElemType>::value
        >::Value;
};

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

template <typename T>
class VectorHasAssign
{
protected:
  typedef char Yes;
  typedef unsigned No;

  template <typename U, U>
  struct ReallyHas;

  template <typename C, typename TIt>
  using Func = void (C::*)(TIt, TIt);

  template <typename C, typename TIt>
  static Yes test(ReallyHas<Func<C, TIt>, &C::assign>*);

  template <typename, typename>
  static No test(...);

public:
    static const bool Value =
        (sizeof(test<T, typename T::const_pointer>(nullptr)) == sizeof(Yes));
};


template <typename T>
constexpr bool vectorHasAssign()
{
    return VectorHasAssign<T>::Value;
}

template <typename TElem, bool TIsIntegral>
struct ArrayListElemVersionDependencyHelper;

template <typename TElem>
struct ArrayListElemVersionDependencyHelper<TElem, true>
{
    static const bool Value = false;
};

template <typename TElem>
struct ArrayListElemVersionDependencyHelper<TElem, false>
{
    static const bool Value = TElem::isVersionDependent();
};

template <typename TElem>
constexpr bool arrayListElementIsVersionDependent()
{
    return ArrayListElemVersionDependencyHelper<TElem, std::is_integral<TElem>::value>::Value;
}

template <typename TElem, bool TIsIntegral>
struct ArrayListElemHasNonDefaultRefreshHelper;

template <typename TElem>
struct ArrayListElemHasNonDefaultRefreshHelper<TElem, true>
{
    static const bool Value = false;
};

template <typename TElem>
struct ArrayListElemHasNonDefaultRefreshHelper<TElem, false>
{
    static const bool Value = TElem::hasNonDefaultRefresh();
};

template <typename TElem>
constexpr bool arrayListElementHasNonDefaultRefresh()
{
    return ArrayListElemHasNonDefaultRefreshHelper<TElem, std::is_integral<TElem>::value>::Value;
}


}  // namespace details

template <typename TFieldBase, typename TStorage>
class ArrayList :
        public TFieldBase,
        public comms::field::details::VersionStorage<
            typename TFieldBase::VersionType,
            details::arrayListElementIsVersionDependent<typename TStorage::value_type>()
        >
{
    using BaseImpl = TFieldBase;
    using VersionBaseImpl =
        comms::field::details::VersionStorage<
            typename TFieldBase::VersionType,
            details::arrayListElementIsVersionDependent<typename TStorage::value_type>()
        >;

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

    template <typename U>
    void pushBack(U&& val)
    {
        value_.push_back(std::forward<U>(val));
    }

    ElementType& createBack()
    {
        value_.emplace_back();
        updateElemVersion(value_.back(), VersionTag());
        return value_.back();
    }

    void clear()
    {
        static_assert(comms::details::hasClearFunc<ValueType>(),
            "The used storage type for ArrayList must have clear() member function");

        value_.clear();
    }

    constexpr std::size_t length() const
    {
        return lengthInternal(ElemTag());
    }

    static constexpr std::size_t minLength()
    {
        return 0U;
    }

    static constexpr std::size_t maxLength()
    {
        return
            details::ArrayListMaxLengthRetrieveHelper<TStorage>::Value *
            maxLengthInternal(ElemTag());
    }

    constexpr bool valid() const
    {
        return validInternal(ElemTag());
    }

    bool refresh()
    {
        return refreshInternal(ElemTag());
    }

    static constexpr std::size_t minElementLength()
    {
        return minElemLengthInternal(ElemTag());
    }

    static constexpr std::size_t maxElementLength()
    {
        return maxElemLengthInternal(ElemTag());
    }

    static constexpr std::size_t elementLength(const ElementType& elem)
    {
        return elementLengthInternal(elem, ElemTag());
    }

    template <typename TIter>
    static ErrorStatus readElement(ElementType& elem, TIter& iter, std::size_t& len)
    {
        return readElementInternal(elem, iter, len, ElemTag());
    }

    template <typename TIter>
    static void readElementNoStatus(ElementType& elem, TIter& iter)
    {
        return readElementNoStatusInternal(elem, iter, ElemTag());
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
            typename std::conditional<
                IsRandomAccessIter && IsRawData,
                RawDataTag,
                FieldElemTag
            >::type;

        return readInternal(iter, len, Tag());
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
            typename std::conditional<
                IsRandomAccessIter && IsRawData,
                RawDataTag,
                FieldElemTag
            >::type;

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
            typename std::conditional<
                IsRandomAccessIter && IsRawData,
                RawDataTag,
                FieldElemTag
            >::type;

        return readNoStatusInternalN(count, iter, Tag());
    }

    template <typename TIter>
    static ErrorStatus writeElement(const ElementType& elem, TIter& iter, std::size_t& len)
    {
        return writeElementInternal(elem, iter, len, ElemTag());
    }

    template <typename TIter>
    static void writeElementNoStatus(const ElementType& elem, TIter& iter)
    {
        return writeElementNoStatusInternal(elem, iter, ElemTag());
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return CommonFuncs::writeSequence(*this, iter, len);
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
        return details::arrayListElementIsVersionDependent<ElementType>();
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return details::arrayListElementHasNonDefaultRefresh<ElementType>();
    }

    bool setVersion(VersionType version)
    {
        return setVersionInternal(version, VersionTag());
    }

private:
    struct FieldElemTag{};
    struct IntegralElemTag{};
    struct FixedLengthTag {};
    struct VarLengthTag {};
    struct RawDataTag {};
    struct AssignExistsTag {};
    struct AssignMissingTag {};
    struct VersionDependentTag {};
    struct NoVersionDependencyTag {};

    using ElemTag = typename std::conditional<
        std::is_integral<ElementType>::value,
        IntegralElemTag,
        FieldElemTag
    >::type;

    using FieldLengthTag = typename std::conditional<
        details::ArrayListFieldHasVarLength<ElementType>::Value,
        VarLengthTag,
        FixedLengthTag
    >::type;

    using VersionTag =
        typename std::conditional<
            details::arrayListElementIsVersionDependent<ElementType>(),
            VersionDependentTag,
            NoVersionDependencyTag
        >::type;

    constexpr std::size_t lengthInternal(FieldElemTag) const
    {
        return fieldLength(FieldLengthTag());
    }

    constexpr std::size_t lengthInternal(IntegralElemTag) const
    {
        return value_.size() * sizeof(ElementType);
    }

    constexpr std::size_t fieldLength(FixedLengthTag) const
    {
        return ElementType().length() * value_.size();
    }

    std::size_t fieldLength(VarLengthTag) const
    {
        return
            std::accumulate(value_.begin(), value_.end(), std::size_t(0),
                [](std::size_t sum, typename ValueType::const_reference e) -> std::size_t
                {
                    return sum + e.length();
                });
    }

    static constexpr std::size_t maxLengthInternal(FieldElemTag)
    {
        return ElementType::maxLength();
    }

    static constexpr std::size_t maxLengthInternal(IntegralElemTag)
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

    template <typename TIter>
    static ErrorStatus readElementInternal(ElementType& elem, TIter& iter, std::size_t& len, FieldElemTag)
    {
        return readFieldElement(elem, iter, len);
    }

    template <typename TIter>
    static ErrorStatus readElementInternal(ElementType& elem, TIter& iter, std::size_t& len, IntegralElemTag)
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

    template <typename TIter>
    static void readElementNoStatusInternal(ElementType& elem, TIter& iter, FieldElemTag)
    {
        readNoStatusFieldElement(elem, iter);
    }

    template <typename TIter>
    static void readElementNoStatusInternal(ElementType& elem, TIter& iter, IntegralElemTag)
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

    template <typename TIter>
    static ErrorStatus writeElementInternal(const ElementType& elem, TIter& iter, std::size_t& len, FieldElemTag)
    {
        return writeFieldElement(elem, iter, len);
    }

    template <typename TIter>
    static ErrorStatus writeElementInternal(const ElementType& elem, TIter& iter, std::size_t& len, IntegralElemTag)
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

    template <typename TIter>
    static void writeElementNoStatusInternal(const ElementType& elem, TIter& iter, FieldElemTag)
    {
        return writeNoStatusFieldElement(elem, iter);
    }

    template <typename TIter>
    static void writeElementNoStatusInternal(const ElementType& elem, TIter& iter, IntegralElemTag)
    {
        return writeNoStatusIntegralElement(elem, iter);
    }

    constexpr bool validInternal(FieldElemTag) const
    {
        return std::all_of(
            value_.begin(), value_.end(),
            [](const ElementType& e) -> bool
            {
                return e.valid();
            });
    }

    static constexpr bool validInternal(IntegralElemTag)
    {
        return true;
    }

    bool refreshInternal(FieldElemTag)
    {
        return
            std::accumulate(
                value_.begin(), value_.end(), false,
                [](bool prev, typename ValueType::reference elem) -> bool
                {
                    return elem.refresh() || prev;
                });
    }

    static constexpr bool refreshInternal(IntegralElemTag)
    {
        return false;
    }

    static constexpr std::size_t minElemLengthInternal(IntegralElemTag)
    {
        return sizeof(ElementType);
    }

    static constexpr std::size_t minElemLengthInternal(FieldElemTag)
    {
        return ElementType::minLength();
    }

    static constexpr std::size_t maxElemLengthInternal(IntegralElemTag)
    {
        return sizeof(ElementType);
    }

    static constexpr std::size_t maxElemLengthInternal(FieldElemTag)
    {
        return ElementType::maxLength();
    }

    static constexpr std::size_t elementLengthInternal(const ElementType&, IntegralElemTag)
    {
        return sizeof(ElementType);
    }

    static constexpr std::size_t elementLengthInternal(const ElementType& elem, FieldElemTag)
    {
        return elem.length();
    }

    template <typename TIter>
    ErrorStatus readInternal(TIter& iter, std::size_t len, FieldElemTag)
    {
        static_assert(comms::details::hasClearFunc<ValueType>(),
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

    template <typename TIter>
    ErrorStatus readInternal(TIter& iter, std::size_t len, RawDataTag)
    {
        using Tag =
            typename std::conditional<
                details::vectorHasAssign<ValueType>(),
                AssignExistsTag,
                AssignMissingTag
            >::type;
        doAssign(iter, len, Tag());
        std::advance(iter, len);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    void doAssign(TIter& iter, std::size_t len, AssignExistsTag) {
        value_.assign(iter, iter + len);
    }

    template <typename TIter>
    void doAssign(TIter& iter, std::size_t len, AssignMissingTag) {
        auto* data = reinterpret_cast<typename ValueType::const_pointer>(&(*iter));
        value_ = ValueType(data, len);
    }

    template <typename TIter>
    ErrorStatus readInternalN(std::size_t count, TIter& iter, std::size_t len, FieldElemTag)
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

    template <typename TIter>
    ErrorStatus readInternalN(std::size_t count, TIter& iter, std::size_t len, RawDataTag)
    {
        if (len < count) {
            return comms::ErrorStatus::NotEnoughData;
        }

        return readInternal(iter, count, RawDataTag());
    }

    template <typename TIter>
    void readNoStatusInternalN(std::size_t count, TIter& iter, FieldElemTag)
    {
        clear();
        while (0 < count) {
            auto& elem = createBack();
            readElementNoStatus(elem, iter);
            --count;
        }
    }

    template <typename TIter>
    void readNoStatusInternalN(std::size_t count, TIter& iter, RawDataTag)
    {
        readInternal(iter, count, RawDataTag());
    }

    bool updateElemVersion(ElementType& elem, VersionDependentTag)
    {
        return elem.setVersion(VersionBaseImpl::version_);
    }

    static constexpr bool updateElemVersion(ElementType&, NoVersionDependencyTag)
    {
        return false;
    }

    bool setVersionInternal(VersionType version, VersionDependentTag)
    {
        VersionBaseImpl::version_ = version;
        bool updated = false;
        for (auto& elem : value()) {
            updated = elem.setVersion(version) || updated;
        }

        return updated;
    }


    static constexpr bool setVersionInternal(VersionType, NoVersionDependencyTag)
    {
        return false;
    }

    ValueType value_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


