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
#include <algorithm>
#include <limits>
#include <numeric>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/access.h"
#include "comms/util/StaticVector.h"
#include "comms/util/StaticString.h"

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
    static const std::size_t Value = 0xffff;
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


}  // namespace details

template <typename TFieldBase, typename TStorage>
class ArrayList : public TFieldBase
{
    using Base = TFieldBase;
public:
    using Endian = typename Base::Endian;

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
    ~ArrayList() = default;

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

    void clear()
    {
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

    constexpr bool refresh()
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
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        value_.clear();
        auto remLen = len;
        while (0 < remLen) {
            auto elem = ElementType();
            auto es = readElement(elem, iter, remLen);
            if (es != ErrorStatus::Success) {
                return es;
            }

            value_.push_back(std::move(elem));
        }

        return ErrorStatus::Success;
    }

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len)
    {
        value_.clear();
        while (0 < count) {
            auto elem = ElementType();
            auto es = readElement(elem, iter, len);
            if (es != ErrorStatus::Success) {
                return es;
            }

            value_.push_back(std::move(elem));
            --count;
        }

        return ErrorStatus::Success;
    }

    template <typename TIter>
    static ErrorStatus writeElement(const ElementType& elem, TIter& iter, std::size_t& len)
    {
        return writeElementInternal(elem, iter, len, ElemTag());
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (len < length()) {
            return ErrorStatus::BufferOverflow;
        }

        auto es = ErrorStatus::Success;
        auto remainingLen = len;
        for (auto fieldIter = value_.begin(); fieldIter != value_.end(); ++fieldIter) {
            es = writeElement(*fieldIter, iter, remainingLen);
            if (es != ErrorStatus::Success) {
                break;
            }
        }

        return es;
    }

    template <typename TIter>
    ErrorStatus writeN(std::size_t count, TIter& iter, std::size_t& len) const
    {
        if ((value_.size() <= count) && (len < length())) {
            return ErrorStatus::BufferOverflow;
        }

        auto es = ErrorStatus::Success;
        for (auto fieldIter = value_.begin(); fieldIter != value_.end(); ++fieldIter) {
            if (count == 0) {
                break;
            }

            es = writeElement(*fieldIter, iter, len);
            if (es != ErrorStatus::Success) {
                break;
            }

            --count;
        }

        return es;
    }


    void forceReadElemCount(std::size_t)
    {
        GASSERT(!"Not supported, use SequenceSizeForcingEnabled option");
    }

    void clearReadElemCount()
    {
        GASSERT(!"Not supported, use SequenceSizeForcingEnabled option");
    }

private:
    struct FieldElemTag{};
    struct IntegralElemTag{};
    struct FixedLengthTag {};
    struct VarLengthTag {};

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
        auto es = elem.read(iter, len);
        if (es == ErrorStatus::Success) {
            GASSERT(elem.length() <= len);
            len -= elem.length();
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

        comms::util::writeData(elem, iter, Endian());
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

    constexpr bool refreshInternal(FieldElemTag)
    {
        return
            std::accumulate(
                value_.begin(), value_.end(), false,
                [](bool prev, typename ValueType::reference_type elem) -> bool
                {
                    return elem.refresh() || prev;
                });
    }

    static constexpr bool refreshInternal(IntegralElemTag)
    {
        return true;
    }

    static constexpr std::size_t minElemLengthInternal(IntegralElemTag)
    {
        return sizeof(ElementType);
    }

    static constexpr std::size_t minElemLengthInternal(FieldElemTag)
    {
        return sizeof(ElementType::minLength());
    }

    static constexpr std::size_t maxElemLengthInternal(IntegralElemTag)
    {
        return sizeof(ElementType);
    }

    static constexpr std::size_t maxElemLengthInternal(FieldElemTag)
    {
        return sizeof(ElementType::maxLength());
    }

    static constexpr std::size_t elementLengthInternal(const ElementType&, IntegralElemTag)
    {
        return sizeof(ElementType);
    }

    static constexpr std::size_t elementLengthInternal(const ElementType& elem, FieldElemTag)
    {
        return elem.length();
    }


    ValueType value_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


