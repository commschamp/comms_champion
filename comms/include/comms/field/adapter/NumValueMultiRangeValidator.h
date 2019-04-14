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

#include "comms/util/Tuple.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TRanges, typename TBase>
class NumValueMultiRangeValidator : public TBase
{
    using BaseImpl = TBase;

    static_assert(comms::util::isTuple<TRanges>(), "TRanges must be a tuple");

public:

    using ValueType = typename BaseImpl::ValueType;

    static_assert(
        std::is_integral<ValueType>::value || std::is_enum<ValueType>::value || std::is_floating_point<ValueType>::value,
        "Only numeric fields are supported for multi range validation.");

    NumValueMultiRangeValidator() = default;

    explicit NumValueMultiRangeValidator(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit NumValueMultiRangeValidator(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    NumValueMultiRangeValidator(const NumValueMultiRangeValidator&) = default;
    NumValueMultiRangeValidator(NumValueMultiRangeValidator&&) = default;
    NumValueMultiRangeValidator& operator=(const NumValueMultiRangeValidator&) = default;
    NumValueMultiRangeValidator& operator=(NumValueMultiRangeValidator&&) = default;

    bool valid() const
    {
        return BaseImpl::valid() &&
               comms::util::tupleTypeAccumulate<TRanges>(false, Validator(BaseImpl::value()));
    }

private:
    class Validator
    {
    public:
        Validator(ValueType val) : m_val(val) {}

        template <typename TRange>
        bool operator()(bool val) const
        {
            static_cast<void>(val);
            static_assert(comms::util::isTuple<TRange>(), "TRange must be a tuple");
            static_assert(std::tuple_size<TRange>::value == 2, "Tuple with 2 elements is expected");
            using MinVal = typename std::tuple_element<0, TRange>::type;
            using MaxVal = typename std::tuple_element<1, TRange>::type;
            static_assert(MinVal::value <= MaxVal::value, "Invalid range");
            return
                val  ||
                ((static_cast<ValueType>(MinVal::value) <= m_val) &&
                (m_val <= static_cast<ValueType>(MaxVal::value)));
        }

    private:
        ValueType m_val;
    };
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




