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

#include <type_traits>
#include <algorithm>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/field/category.h"
#include "comms/util/Tuple.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TFieldBase, typename TMembers>
class Bundle : public TFieldBase
{
public:
    using Category = comms::field::category::BundleField;
    using ValueType = TMembers;

    Bundle() = default;
    explicit Bundle(const ValueType& val)
      : members_(val)
    {
    }

    explicit Bundle(ValueType&& val)
      : members_(std::move(val))
    {
    }

    Bundle(const Bundle&) = default;
    Bundle(Bundle&&) = default;
    ~Bundle() = default;

    Bundle& operator=(const Bundle&) = default;
    Bundle& operator=(Bundle&&) = default;

    const ValueType& value() const
    {
        return members_;
    }

    ValueType& value()
    {
        return members_;
    }

    constexpr std::size_t length() const
    {
        return comms::util::tupleAccumulate(value(), std::size_t(0), LengthCalcHelper());
    }

    static constexpr std::size_t minLength()
    {
        return comms::util::tupleTypeAccumulate<ValueType>(std::size_t(0), MinLengthCalcHelper());
    }

    static constexpr std::size_t maxLength()
    {
        return comms::util::tupleTypeAccumulate<ValueType>(std::size_t(0), MaxLengthCalcHelper());
    }

    constexpr bool valid() const
    {
        return comms::util::tupleAccumulate(value(), true, ValidCheckHelper());
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto es = ErrorStatus::Success;
        comms::util::tupleForEach(value(), makeReadHelper(es, iter, len));
        return es;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        auto es = ErrorStatus::Success;
        comms::util::tupleForEach(value(), makeWriteHelper(es, iter, len));
        return es;
    }

private:
    struct LengthCalcHelper
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t sum, const TField& field) const
        {
            return sum + field.length();
        }
    };

    struct MinLengthCalcHelper
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t sum) const
        {
            return sum + TField::minLength();
        }
    };

    struct MaxLengthCalcHelper
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t sum) const
        {
            return sum + TField::maxLength();
        }
    };

    struct ValidCheckHelper
    {
        template <typename TField>
        constexpr bool operator()(bool soFar, const TField& field) const
        {
            return soFar && field.valid();
        }
    };

    template <typename TIter>
    class ReadHelper
    {
    public:
        ReadHelper(ErrorStatus& es, TIter& iter, std::size_t len)
          : es_(es),
            iter_(iter),
            len_(len)
        {
        }

        template <typename TField>
        void operator()(TField& field)
        {
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            es_ = field.read(iter_, len_);
            if (es_ == comms::ErrorStatus::Success) {
                len_ -= field.length();
            }
        }

    private:
        ErrorStatus& es_;
        TIter& iter_;
        std::size_t len_;
    };

    template <typename TIter>
    static ReadHelper<TIter> makeReadHelper(comms::ErrorStatus& es, TIter& iter, std::size_t len)
    {
        return ReadHelper<TIter>(es, iter, len);
    }

    template <typename TIter>
    class WriteHelper
    {
    public:
        WriteHelper(ErrorStatus& es, TIter& iter, std::size_t len)
          : es_(es),
            iter_(iter),
            len_(len)
        {
        }

        template <typename TField>
        void operator()(const TField& field)
        {
            if (es_ != comms::ErrorStatus::Success) {
                return;
            }

            es_ = field.write(iter_, len_);
            if (es_ == comms::ErrorStatus::Success) {
                len_ -= field.length();
            }
        }

    private:
        ErrorStatus& es_;
        TIter& iter_;
        std::size_t len_;
    };

    template <typename TIter>
    static WriteHelper<TIter> makeWriteHelper(comms::ErrorStatus& es, TIter& iter, std::size_t len)
    {
        return WriteHelper<TIter>(es, iter, len);
    }

    static_assert(comms::util::IsTuple<ValueType>::Value, "ValueType must be tuple");
    ValueType members_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


