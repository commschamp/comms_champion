//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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

template <typename TLenField, comms::ErrorStatus TStatus, typename TNext>
class SequenceSerLengthFieldPrefix : public details::AdapterBaseT<TNext>
{
    using Base = details::AdapterBaseT<TNext>;
    using LenField = TLenField;

public:
    using ValueType = typename Base::ValueType;
    using ElementType = typename Base::ElementType;

    SequenceSerLengthFieldPrefix() = default;

    explicit SequenceSerLengthFieldPrefix(const ValueType& val)
      : Base(val)
    {
    }

    explicit SequenceSerLengthFieldPrefix(ValueType&& val)
      : Base(std::move(val))
    {
    }

    SequenceSerLengthFieldPrefix(const SequenceSerLengthFieldPrefix&) = default;
    SequenceSerLengthFieldPrefix(SequenceSerLengthFieldPrefix&&) = default;
    SequenceSerLengthFieldPrefix& operator=(const SequenceSerLengthFieldPrefix&) = default;
    SequenceSerLengthFieldPrefix& operator=(SequenceSerLengthFieldPrefix&&) = default;

    std::size_t length() const
    {
        using LenValueType = typename LenField::ValueType;
        auto valLength = Base::length();
        return LenField(static_cast<LenValueType>(valLength)).length() + valLength;
    }

    static constexpr std::size_t minLength()
    {
        return LenField::minLength() + Base::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return LenField::maxLength() + Base::maxLength();
    }

    bool valid() const
    {
        return
            LenField(Base::length()).valid() &&
            Base::valid();
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        LenField lenField;
        auto es = lenField.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        es = Base::read(iter, static_cast<std::size_t>(lenField.value()));
        if (es == comms::ErrorStatus::NotEnoughData) {
            return TStatus;
        }

        return es;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        using LenValueType = typename LenField::ValueType;
        auto lenVal = Base::length();
        LenField lenField(static_cast<LenValueType>(lenVal));
        auto es = lenField.write(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        GASSERT(lenField.length() <= len);
        return Base::write(iter, lenVal);
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




