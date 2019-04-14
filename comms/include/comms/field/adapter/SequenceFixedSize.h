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

#include <cstddef>
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/details/detect.h"
#include "comms/field/basic/CommonFuncs.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class SequenceFixedSizeBase : public TBase
{
    using BaseImpl = TBase;

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    explicit SequenceFixedSizeBase(std::size_t maxSize)
      : fixedSize_(maxSize)
    {
    }

    SequenceFixedSizeBase(std::size_t maxSize, const ValueType& val)
      : BaseImpl(val),
        fixedSize_(maxSize)
    {
    }

    SequenceFixedSizeBase(std::size_t maxSize, ValueType&& val)
      : BaseImpl(std::move(val)),
        fixedSize_(maxSize)
    {
    }

    SequenceFixedSizeBase(const SequenceFixedSizeBase&) = default;
    SequenceFixedSizeBase(SequenceFixedSizeBase&&) = default;
    SequenceFixedSizeBase& operator=(const SequenceFixedSizeBase&) = default;
    SequenceFixedSizeBase& operator=(SequenceFixedSizeBase&&) = default;

    std::size_t length() const
    {
        auto currSize = BaseImpl::value().size();
        if (currSize == fixedSize_) {
            return BaseImpl::length();
        }

        if (currSize < fixedSize_) {
            auto remSize = fixedSize_ - currSize;
            auto dummyElem = ElementType();
            return BaseImpl::length() + (remSize * BaseImpl::elementLength(dummyElem));
        }

        using Tag =
            typename std::conditional<
                std::is_integral<ElementType>::value && (sizeof(ElementType) == sizeof(std::uint8_t)),
                HasRawDataTag,
                HasFieldsTag
            >::type;

        return recalcLen(Tag());
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        return BaseImpl::readN(fixedSize_, iter, len);
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        return BaseImpl::readNoStatusN(fixedSize_, iter);
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        auto writeCount = std::min(BaseImpl::value().size(), fixedSize_);
        auto es = BaseImpl::writeN(writeCount, iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto remCount = fixedSize_ - writeCount;
        if (remCount == 0) {
            return es;
        }

        auto dummyElem = ElementType();
        while (0 < remCount) {
            es = BaseImpl::writeElement(dummyElem, iter, len);
            if (es != ErrorStatus::Success) {
                break;
            }

            --remCount;
        }

        return es;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        auto writeCount = std::min(BaseImpl::value().size(), fixedSize_);
        BaseImpl::writeNoStatusN(writeCount, iter);

        auto remCount = fixedSize_ - writeCount;
        if (remCount == 0) {
            return;
        }

        auto dummyElem = ElementType();
        while (0 < remCount) {
            BaseImpl::writeElementNoStatus(dummyElem, iter);
            --remCount;
        }
    }

    bool valid() const
    {
        return BaseImpl::valid() && (BaseImpl::value().size() <= fixedSize_);
    }

    bool refresh()
    {
        if (!BaseImpl::refresh()) {
            return false;
        }

        using Tag =
            typename std::conditional<
                comms::details::hasResizeFunc<ElementType>(),
                HasResizeTag,
                NoResizeTag
            >::type;


        return doRefresh(Tag());
    }

private:
    struct HasRawDataTag {};
    struct HasFieldsTag {};
    struct HasFixedLengthElemsTag {};
    struct HasVarLengthElemsTag {};
    struct HasResizeTag {};
    struct NoResizeTag {};

    std::size_t recalcLen(HasFieldsTag) const
    {
        using Tag =
            typename std::conditional<
                ElementType::minLength() == ElementType::maxLength(),
                HasFixedLengthElemsTag,
                HasVarLengthElemsTag
            >::type;
        return recalcLen(Tag());
    }

    std::size_t recalcLen(HasRawDataTag) const
    {
        return fixedSize_;
    }

    std::size_t recalcLen(HasFixedLengthElemsTag) const
    {
        return fixedSize_ * ElementType::minLength();
    }

    std::size_t recalcLen(HasVarLengthElemsTag) const
    {
        std::size_t result = 0U;
        auto count = fixedSize_;
        for (auto& elem : BaseImpl::value()) {
            if (count == 0U) {
                break;
            }

            result += BaseImpl::elementLength(elem);
            --count;
        }
        return result;
    }

    bool doRefresh(HasResizeTag)
    {
        if (BaseImpl::value() == fixedSize_) {
            return false;
        }

        BaseImpl::value().resize(fixedSize_);
        return true;
    }

    static constexpr bool doRefresh(NoResizeTag)
    {
        return false;
    }

    std::size_t fixedSize_ = 0;
};

template <std::size_t TSize, typename TBase>
class SequenceFixedSize : public SequenceFixedSizeBase<TBase>
{
    using BaseImpl = SequenceFixedSizeBase<TBase>;

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    explicit SequenceFixedSize()
      : BaseImpl(TSize)
    {
    }

    explicit SequenceFixedSize(const ValueType& val)
      : BaseImpl(TSize, val)
    {
    }

    SequenceFixedSize(ValueType&& val)
      : BaseImpl(TSize, std::move(val))
    {
    }

    SequenceFixedSize(const SequenceFixedSize&) = default;
    SequenceFixedSize(SequenceFixedSize&&) = default;
    SequenceFixedSize& operator=(const SequenceFixedSize&) = default;
    SequenceFixedSize& operator=(SequenceFixedSize&&) = default;

    static constexpr std::size_t minLength()
    {
        return BaseImpl::minLength() + BaseImpl::minElementLength() * TSize;
    }

    static constexpr std::size_t maxLength()
    {
        return BaseImpl::minLength() + BaseImpl::maxElementLength() * TSize;
    }
};





}  // namespace adapter

}  // namespace field

}  // namespace comms




