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

#include "comms/Assert.h"
#include "details/AdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TNext>
class SequenceFixedSizeBase : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;

public:
    typedef typename Base::ValueType ValueType;
    typedef typename Base::ElementType ElementType;

    explicit SequenceFixedSizeBase(std::size_t maxSize)
      : fixedSize_(maxSize)
    {
        Base::value().resize(fixedSize_);
    }

    SequenceFixedSizeBase(std::size_t maxSize, const ValueType& val)
      : Base(val),
        fixedSize_(maxSize)
    {
        GASSERT(Base::value().size() == fixedSize_);
    }

    SequenceFixedSizeBase(std::size_t maxSize, ValueType&& val)
      : Base(std::move(val)),
        fixedSize_(maxSize)
    {
        GASSERT(Base::value().size() == fixedSize_);
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

        typedef typename Base::Next Next;
        ValueType copy(Base::value());
        copy.resize(fixedSize_);
        return Next(std::move(copy)).length();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        return Base::readN(fixedSize_, iter, len);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        auto writeCount = std::min(Base::value().size(), fixedSize_);
        auto es = Base::writeN(writeCount, iter, len);
        if (es != ErrorStatus::Success) {
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
        if (!Base::valid()) {
            return false;
        }

        if (fixedSize_ == Base::value().size()) {
            return true;
        }

        if (fixedSize_ < Base::value().size()) {
            return false;
        }

        auto valCopy = Base::value();
        auto sizeDiff = fixedSize_ - valCopy.size();
        std::fill_n(std::back_inserter(valCopy), sizeDiff, ElementType());

        typedef typename Base::Next Next;
        Next nextAdapter(std::move(valCopy));
        return nextAdapter.valid();
    }

private:
    std::size_t fixedSize_ = 0;
};

template <std::size_t TSize, typename TNext>
class SequenceFixedSize : public SequenceFixedSizeBase<TNext>
{
    typedef SequenceFixedSizeBase<TNext> Base;

public:
    typedef typename Base::ValueType ValueType;
    typedef typename Base::ElementType ElementType;

    explicit SequenceFixedSize()
      : Base(TSize)
    {
        GASSERT(Base::value().size() == TSize);
    }

    explicit SequenceFixedSize(const ValueType& val)
      : Base(TSize, val)
    {
        GASSERT(Base::value().size() == TSize);
    }

    SequenceFixedSize(ValueType&& val)
      : Base(TSize, std::move(val))
    {
        GASSERT(Base::value().size() == TSize);
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




