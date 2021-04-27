//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstddef>
#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/detect.h"
#include "comms/field/basic/CommonFuncs.h"
#include "comms/util/type_traits.h"
#include "comms/details/tag.h"

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
            typename comms::util::LazyShallowConditional<
                std::is_integral<ElementType>::value && (sizeof(ElementType) == sizeof(std::uint8_t))
            >::template Type<
                HasRawDataTag,
                HasFieldsTag
            >;

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
            typename comms::util::LazyShallowConditional<
                comms::util::detect::hasResizeFunc<ElementType>()
            >::template Type<
                HasResizeTag,
                NoResizeTag
            >;

        return doRefresh(Tag());
    }

private:
    template <typename... TParams>
    using HasRawDataTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using HasFieldsTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using HasFixedLengthElemsTag = comms::details::tag::Tag3<>;

    template <typename... TParams>
    using HasVarLengthElemsTag = comms::details::tag::Tag4<>;    

    template <typename... TParams>
    using HasResizeTag = comms::details::tag::Tag5<>;

    template <typename... TParams>
    using NoResizeTag = comms::details::tag::Tag6<>;     

    template <typename... TParams>
    std::size_t recalcLen(HasFieldsTag<TParams...>) const
    {
        using Tag =
            typename comms::util::LazyShallowConditional<
                ElementType::minLength() == ElementType::maxLength()
            >::template Type<
                HasFixedLengthElemsTag,
                HasVarLengthElemsTag
            >;
        return recalcLen(Tag());
    }

    template <typename... TParams>
    std::size_t recalcLen(HasRawDataTag<TParams...>) const
    {
        return fixedSize_;
    }

    template <typename... TParams>
    std::size_t recalcLen(HasFixedLengthElemsTag<TParams...>) const
    {
        return fixedSize_ * ElementType::minLength();
    }

    template <typename... TParams>
    std::size_t recalcLen(HasVarLengthElemsTag<TParams...>) const
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

    template <typename... TParams>
    bool doRefresh(HasResizeTag<TParams...>)
    {
        if (BaseImpl::value() == fixedSize_) {
            return false;
        }

        BaseImpl::value().resize(fixedSize_);
        return true;
    }

    template <typename... TParams>
    static constexpr bool doRefresh(NoResizeTag<TParams...>)
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




