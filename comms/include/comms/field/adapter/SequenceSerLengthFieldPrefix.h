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

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TLenField, comms::ErrorStatus TStatus, typename TBase>
class SequenceSerLengthFieldPrefix : public TBase
{
    using BaseImpl = TBase;
    using LenField = TLenField;

    static_assert(!LenField::isVersionDependent(),
            "Prefix fields must not be version dependent");

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceSerLengthFieldPrefix() = default;

    explicit SequenceSerLengthFieldPrefix(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceSerLengthFieldPrefix(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceSerLengthFieldPrefix(const SequenceSerLengthFieldPrefix&) = default;
    SequenceSerLengthFieldPrefix(SequenceSerLengthFieldPrefix&&) = default;
    SequenceSerLengthFieldPrefix& operator=(const SequenceSerLengthFieldPrefix&) = default;
    SequenceSerLengthFieldPrefix& operator=(SequenceSerLengthFieldPrefix&&) = default;

    std::size_t length() const
    {
        using LenValueType = typename LenField::ValueType;
        auto valLength = BaseImpl::length();
        LenField lenField;
        lenField.value() = static_cast<LenValueType>(valLength);
        return lenField.length() + valLength;
    }

    static constexpr std::size_t minLength()
    {
        return LenField::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return LenField::maxLength() + BaseImpl::maxLength();
    }

    bool valid() const
    {
        LenField lenField;
        lenField.value() = static_cast<typename LenField::ValueType>(BaseImpl::length());
        return lenField.valid() && BaseImpl::valid();
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto fromIter = iter;
        LenField lenField;
        auto es = lenField.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto diff = static_cast<std::size_t>(std::distance(fromIter, iter));
        COMMS_ASSERT(diff <= len);
        len -= diff;
        auto remLen = static_cast<std::size_t>(lenField.value());
        if (len < remLen) {
            return TStatus;
        }

        es = BaseImpl::read(iter, remLen);
        if (es == comms::ErrorStatus::NotEnoughData) {
            return TStatus;
        }

        return es;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        using LenValueType = typename LenField::ValueType;
        auto lenVal = BaseImpl::length();
        LenField lenField;
        lenField.value() = static_cast<LenValueType>(lenVal);
        auto es = lenField.write(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(lenField.length() <= len);
        return BaseImpl::write(iter, lenVal);
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        using LenValueType = typename LenField::ValueType;
        auto lenVal = BaseImpl::length();
        LenField lenField;
        lenField.value() = static_cast<LenValueType>(lenVal);
        lenField.writeNoStatus(iter);
        BaseImpl::writeNoStatus(iter);
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




