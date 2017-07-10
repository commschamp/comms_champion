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

#include <cstddef>
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/details/detect.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class SequenceFixedSizeBase : public TBase
{
    using Base = TBase;

public:
    using ValueType = typename Base::ValueType;
    using ElementType = typename Base::ElementType;

    explicit SequenceFixedSizeBase(std::size_t maxSize)
      : fixedSize_(maxSize)
    {
    }

    SequenceFixedSizeBase(std::size_t maxSize, const ValueType& val)
      : Base(val),
        fixedSize_(maxSize)
    {
    }

    SequenceFixedSizeBase(std::size_t maxSize, ValueType&& val)
      : Base(std::move(val)),
        fixedSize_(maxSize)
    {
    }

    SequenceFixedSizeBase(const SequenceFixedSizeBase&) = default;
    SequenceFixedSizeBase(SequenceFixedSizeBase&&) = default;
    SequenceFixedSizeBase& operator=(const SequenceFixedSizeBase&) = default;
    SequenceFixedSizeBase& operator=(SequenceFixedSizeBase&&) = default;

    std::size_t length() const
    {
        auto currSize = Base::value().size();
        if (currSize == fixedSize_) {
            return Base::length();
        }

        if (currSize < fixedSize_) {
            auto remSize = fixedSize_ - currSize;
            auto dummyElem = ElementType();
            return Base::length() + (remSize * Base::elementLength(dummyElem));
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
        return Base::readN(fixedSize_, iter, len);
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        auto writeCount = std::min(Base::value().size(), fixedSize_);
        auto es = Base::writeN(writeCount, iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto remCount = fixedSize_ - writeCount;
        if (remCount == 0) {
            return es;
        }

        auto dummyElem = ElementType();
        while (0 < remCount) {
            es = Base::writeElement(dummyElem, iter, len);
            if (es != ErrorStatus::Success) {
                break;
            }

            --remCount;
        }

        return es;
    }

    bool valid() const
    {
        return Base::valid() && (Base::value().size() <= fixedSize_);
    }

    bool refresh()
    {
        if (!Base::refresh()) {
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
        ValueType copy(Base::value().begin(), Base::value().begin() + fixedSize_);
        return Base(std::move(copy)).length();
    }

    bool doRefresh(HasResizeTag)
    {
        if (Base::value() == fixedSize_) {
            return false;
        }

        Base::value().resize(fixedSize_);
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
    using Base = SequenceFixedSizeBase<TBase>;

public:
    using ValueType = typename Base::ValueType;
    using ElementType = typename Base::ElementType;

    explicit SequenceFixedSize()
      : Base(TSize)
    {
    }

    explicit SequenceFixedSize(const ValueType& val)
      : Base(TSize, val)
    {
    }

    SequenceFixedSize(ValueType&& val)
      : Base(TSize, std::move(val))
    {
    }

    SequenceFixedSize(const SequenceFixedSize&) = default;
    SequenceFixedSize(SequenceFixedSize&&) = default;
    SequenceFixedSize& operator=(const SequenceFixedSize&) = default;
    SequenceFixedSize& operator=(SequenceFixedSize&&) = default;

    static constexpr std::size_t minLength()
    {
        return Base::minElementLength() * TSize;
    }

    static constexpr std::size_t maxLength()
    {
        return Base::maxElementLength() * TSize;
    }
};





}  // namespace adapter

}  // namespace field

}  // namespace comms




