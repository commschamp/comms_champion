//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

#include <iterator>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/field/basic/CommonFuncs.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TLenField, comms::ErrorStatus TStatus, typename TBase>
class SequenceElemSerLengthFieldPrefix : public TBase
{
    using BaseImpl = TBase;
    using LenField = TLenField;

    static_assert(!LenField::isVersionDependent(),
            "Prefix fields must not be version dependent");

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceElemSerLengthFieldPrefix() = default;

    explicit SequenceElemSerLengthFieldPrefix(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceElemSerLengthFieldPrefix(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceElemSerLengthFieldPrefix(const SequenceElemSerLengthFieldPrefix&) = default;
    SequenceElemSerLengthFieldPrefix(SequenceElemSerLengthFieldPrefix&&) = default;
    SequenceElemSerLengthFieldPrefix& operator=(const SequenceElemSerLengthFieldPrefix&) = default;
    SequenceElemSerLengthFieldPrefix& operator=(SequenceElemSerLengthFieldPrefix&&) = default;

    std::size_t length() const
    {
        return lengthInternal(LenFieldLengthTag(), ElemLengthTag());
    }

    std::size_t elementLength(const ElementType& elem) const
    {
        return elementLengthInternal(elem, LenFieldLengthTag());
    }

    static constexpr std::size_t minLength()
    {
        return LenField::minLength();
    }    

    static constexpr std::size_t maxLength()
    {
        return basic::CommonFuncs::maxSupportedLength();
    }

    static constexpr std::size_t minElementLength()
    {
        return LenField::minLength() + BaseImpl::minElementLength();
    }

    static constexpr std::size_t maxElementLength()
    {
        return LenField::maxLength() + BaseImpl::maxElementLength();
    }

    template <typename TIter>
    ErrorStatus readElement(ElementType& elem, TIter& iter, std::size_t& len) const
    {
        auto fromIter = iter;
        LenField lenField;
        auto es = lenField.read(iter, len);
        if (es != ErrorStatus::Success) {
            return es;
        }

        auto diff = static_cast<std::size_t>(std::distance(fromIter, iter));
        COMMS_ASSERT(diff <= len);
        len -= diff;
        if (len < lenField.value()) {
            return comms::ErrorStatus::NotEnoughData;
        }

        const auto reqLen = static_cast<std::size_t>(lenField.value());
        std::size_t elemLen = reqLen;
        es = BaseImpl::readElement(elem, iter, elemLen);
        if (es == ErrorStatus::NotEnoughData) {
            return TStatus;
        }

        if (es != ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(elemLen <= reqLen);
        std::advance(iter, elemLen);
        len -= reqLen;
        return ErrorStatus::Success;
    }

    template <typename TIter>
    void readElementNoStatus(ElementType& elem, TIter& iter) const = delete;

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        return basic::CommonFuncs::readSequence(*this, iter, len);
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len)
    {
        return basic::CommonFuncs::readSequenceN(*this, count, iter, len);
    }

    template <typename TIter>
    void readNoStatusN(std::size_t count, TIter& iter) = delete;

    template <typename TIter>
    ErrorStatus writeElement(const ElementType& elem, TIter& iter, std::size_t& len) const
    {
        auto elemLength = BaseImpl::elementLength(elem);
        LenField lenField;
        lenField.value() = elemLength;
        auto es = lenField.write(iter, len);
        if (es != ErrorStatus::Success) {
            return es;
        }

        len -= lenField.length();
        return BaseImpl::writeElement(elem, iter, len);
    }

    template <typename TIter>
    static void writeElementNoStatus(const ElementType& elem, TIter& iter)
    {
        auto elemLength = BaseImpl::elementLength(elem);
        LenField lenField;
        lenField.value() = elemLength;
        lenField.writeNoStatus(iter);
        BaseImpl::writeElementNoStatus(elem, iter);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        return basic::CommonFuncs::writeSequence(*this, iter, len);
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        basic::CommonFuncs::writeSequenceNoStatus(*this, iter);
    }

    template <typename TIter>
    ErrorStatus writeN(std::size_t count, TIter& iter, std::size_t& len) const
    {
        return basic::CommonFuncs::writeSequenceN(*this, count, iter, len);
    }

    template <typename TIter>
    void writeNoStatusN(std::size_t count, TIter& iter) const
    {
        basic::CommonFuncs::writeSequenceNoStatusN(*this, count, iter);
    }

private:

    struct FixedLengthLenFieldTag {};
    struct VarLengthLenFieldTag {};
    struct FixedLengthElemTag {};
    struct VarLengthElemTag {};

    using LenFieldLengthTag =
        typename std::conditional<
            LenField::minLength() == LenField::maxLength(),
            FixedLengthLenFieldTag,
            VarLengthLenFieldTag
        >::type;

    using ElemLengthTag =
        typename std::conditional<
            BaseImpl::minElementLength() == BaseImpl::maxElementLength(),
            FixedLengthElemTag,
            VarLengthElemTag
        >::type;

    std::size_t lengthInternal(FixedLengthLenFieldTag, FixedLengthElemTag) const
    {
        return (LenField::minLength() + BaseImpl::minElementLength()) * BaseImpl::value().size();
    }

    std::size_t lengthInternal(FixedLengthLenFieldTag, VarLengthElemTag) const
    {
        std::size_t result = 0U;
        for (auto& elem : BaseImpl::value()) {
            result += (LenField::minLength() + BaseImpl::elementLength(elem));
        }
        return result;
    }

    std::size_t lengthInternal(VarLengthLenFieldTag, FixedLengthElemTag) const
    {
        LenField lenField;
        lenField.value() = BaseImpl::minElementLength();
        return (lenField.length() + BaseImpl::minElementLength()) * BaseImpl::value().size();
    }

    std::size_t lengthInternal(VarLengthLenFieldTag, VarLengthElemTag) const
    {
        std::size_t result = 0U;
        for (auto& elem : BaseImpl::value()) {
            LenField lenField;
            auto elemLength = BaseImpl::elementLength(elem);
            lenField.value() = elemLength;
            result += (lenField.length() + elemLength);
        }
        return result;
    }

    std::size_t elementLengthInternal(const ElementType& elem, FixedLengthLenFieldTag) const
    {
        return LenField::minLength() + BaseImpl::elementLength(elem);
    }

    std::size_t elementLengthInternal(const VarLengthLenFieldTag& elem, FixedLengthLenFieldTag) const
    {
        LenField lenField;
        auto elemLength = BaseImpl::elementLength(elem);
        lenField.value() = elemLength;
        return lenField.length() + elemLength;
    }

    template <typename TIter>
    static void advanceWriteIterator(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using ByteType = typename std::iterator_traits<IterType>::value_type;
        while (len > 0U) {
            *iter = ByteType();
            ++iter;
            --len;
        }
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




