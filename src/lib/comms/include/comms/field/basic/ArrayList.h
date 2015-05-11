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

#include <type_traits>
#include <vector>
#include <algorithm>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/field/category.h"
#include "comms/util/StaticQueue.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TFieldBase, typename TElem, typename TStorage = std::vector<TElem> >
class ArrayList : public TFieldBase
{
    typedef TFieldBase Base;
public:
    typedef comms::field::category::CollectionField Category;

    typedef TElem ElementType;
    typedef TStorage ValueType;

    typedef const ValueType& ParamValueType;
    typedef ValueType& ValueRefType;

    ArrayList() = default;

    explicit ArrayList(ParamValueType value)
      : value_(value)
    {
    }

    explicit ArrayList(ValueType&& value)
      : value_(std::move(value))
    {
    }

    ArrayList(const ArrayList&) = default;
    ArrayList(ArrayList&&) = default;
    ArrayList& operator=(const ArrayList&) = default;
    ArrayList& operator=(ArrayList&&) = default;
    ~ArrayList() = default;

    const ValueType& fields() const
    {
        return value_;
    }

    ValueType& fields()
    {
        return value_;
    }

    ParamValueType getValue() const
    {
        return value_;
    }

    ValueRefType getValue()
    {
        return value_;
    }

    void setValue(ParamValueType value)
    {
        value_ = value;
    }

    void setValue(ValueType&& value)
    {
        value_ = std::move(value);
    }

    constexpr std::size_t length() const
    {
        return lengthInternal(ElemLengthTag());
    }

    static constexpr std::size_t minLength()
    {
        return 0U;
    }

    static constexpr std::size_t maxLength()
    {
        return ValueType::max_size() * ElementType::maxLength();
    }

    constexpr bool valid() const
    {
        return std::all_of(
            value_.begin(), value_.end(),
            [](const ElementType& e) -> bool
            {
                return e.valid();
            });
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        value_.clear();
        while (0 < len) {
            ElementType field;
            auto es = field.read(iter, len);
            if (es != ErrorStatus::Success) {
                return es;
            }

            GASSERT(field.length() <= len);
            len -= field.length();
            value_.push_back(std::move(field));
        }

        return ErrorStatus::Success;
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
            es = fieldIter->write(iter, remainingLen);
            if (es != ErrorStatus::Success) {
                break;
            }
            remainingLen -= fieldIter->length();
        }

        return es;
    }

private:

    struct FixedLengthTag {};
    struct VarLengthTag {};
    typedef typename std::conditional<
        (ElementType::minLength() == ElementType::maxLength()),
        FixedLengthTag,
        VarLengthTag
    >::type ElemLengthTag;

    constexpr std::size_t lengthInternal(FixedLengthTag) const
    {
        return ElementType().length() * value_.size();
    }

    std::size_t lengthInternal(VarLengthTag) const
    {
        return
            std::accumulate(value_.begin(), value_.end(), std::size_t(0),
                [](std::size_t sum, typename ValueType::const_reference e) -> std::size_t
                {
                    return sum + e.length();
                });
    }

    ValueType value_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


