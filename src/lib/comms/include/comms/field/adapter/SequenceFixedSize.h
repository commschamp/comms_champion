//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

    SequenceFixedSizeBase(std::size_t maxSize, const ValueType& value)
      : Base(value),
        fixedSize_(maxSize)
    {
        GASSERT(Base::value().size() == fixedSize_);
    }

    SequenceFixedSizeBase(std::size_t maxSize, ValueType&& value)
      : Base(std::move(value)),
        fixedSize_(maxSize)
    {
        GASSERT(Base::value().size() == fixedSize_);
    }

    SequenceFixedSizeBase(const SequenceFixedSizeBase&) = default;
    SequenceFixedSizeBase(SequenceFixedSizeBase&&) = default;
    SequenceFixedSizeBase& operator=(const SequenceFixedSizeBase&) = default;
    SequenceFixedSizeBase& operator=(SequenceFixedSizeBase&&) = default;

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        return Base::readN(fixedSize_, iter, len);
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        GASSERT(Base::value().size() == fixedSize_);
        return Base::write(iter, len);
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

    explicit SequenceFixedSize(const ValueType& value)
      : Base(TSize, value)
    {
        GASSERT(Base::value().size() == TSize);
    }

    SequenceFixedSize(ValueType&& value)
      : Base(TSize, std::move(value))
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

    bool valid() const
    {
        return (Base::value().size() == TSize) && Base::valid();
    }

private:

};





}  // namespace adapter

}  // namespace field

}  // namespace comms




