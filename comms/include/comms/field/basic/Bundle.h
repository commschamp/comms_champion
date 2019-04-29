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

#include <type_traits>
#include <algorithm>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"
#include "comms/util/Tuple.h"
#include "CommonFuncs.h"

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
    using ValueType = TMembers;
    using VersionType = typename TFieldBase::VersionType;

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
    ~Bundle() noexcept = default;

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

    template <std::size_t TFromIdx>
    constexpr std::size_t lengthFrom() const
    {
        return
            comms::util::tupleAccumulateFromUntil<TFromIdx, std::tuple_size<ValueType>::value>(
                value(),
                std::size_t(0),
                LengthCalcHelper());
    }

    template <std::size_t TUntilIdx>
    constexpr std::size_t lengthUntil() const
    {
        return
            comms::util::tupleAccumulateFromUntil<0, TUntilIdx>(
                value(),
                std::size_t(0),
                LengthCalcHelper());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    constexpr std::size_t lengthFromUntil() const
    {
        return
            comms::util::tupleAccumulateFromUntil<TFromIdx, TUntilIdx>(
                value(),
                std::size_t(0),
                LengthCalcHelper());
    }

    static constexpr std::size_t minLength()
    {
        return comms::util::tupleTypeAccumulate<ValueType>(static_cast<std::size_t>(0U), MinLengthCalcHelper());
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t minLengthFrom()
    {
        return
            comms::util::tupleTypeAccumulateFromUntil<TFromIdx, std::tuple_size<ValueType>::value, ValueType>(
                static_cast<std::size_t>(0U),
                MinLengthCalcHelper());
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t minLengthUntil()
    {
        return
            comms::util::tupleTypeAccumulateFromUntil<0, TUntilIdx, ValueType>(
                static_cast<std::size_t>(0U),
                MinLengthCalcHelper());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t minLengthFromUntil()
    {
        return
            comms::util::tupleTypeAccumulateFromUntil<TFromIdx, TUntilIdx, ValueType>(
                static_cast<std::size_t>(0U),
                MinLengthCalcHelper());
    }

    static constexpr std::size_t maxLength()
    {
        return comms::util::tupleTypeAccumulate<ValueType>(std::size_t(0), MaxLengthCalcHelper());
    }

    template <std::size_t TFromIdx>
    static constexpr std::size_t maxLengthFrom()
    {
        return
            comms::util::tupleTypeAccumulateFromUntil<TFromIdx, std::tuple_size<ValueType>::value, ValueType>(
                static_cast<std::size_t>(0U),
                MaxLengthCalcHelper());
    }

    template <std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthUntil()
    {
        return
            comms::util::tupleTypeAccumulateFromUntil<0, TUntilIdx, ValueType>(
                static_cast<std::size_t>(0U),
                MaxLengthCalcHelper());
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx>
    static constexpr std::size_t maxLengthFromUntil()
    {
        return
            comms::util::tupleTypeAccumulateFromUntil<TFromIdx, TUntilIdx, ValueType>(
                static_cast<std::size_t>(0U),
                MaxLengthCalcHelper());
    }

    constexpr bool valid() const
    {
        return comms::util::tupleAccumulate(value(), true, ValidCheckHelper());
    }

    bool refresh()
    {
        return comms::util::tupleAccumulate(value(), false, RefreshHelper());
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto es = ErrorStatus::Success;
        comms::util::tupleForEach(value(), makeReadHelper(es, iter, len));
        return es;
    }

    template <std::size_t TFromIdx, typename TIter>
    ErrorStatus readFrom(TIter& iter, std::size_t len)
    {
        return readFromAndUpdateLen<TFromIdx>(iter, len);
    }

    template <std::size_t TFromIdx, typename TIter>
    ErrorStatus readFromAndUpdateLen(TIter& iter, std::size_t& len)
    {
        auto es = ErrorStatus::Success;
        comms::util::template tupleForEachFrom<TFromIdx>(value(), makeReadHelper(es, iter, len));
        return es;
    }    

    template <std::size_t TUntilIdx, typename TIter>
    ErrorStatus readUntil(TIter& iter, std::size_t len)
    {
        return readUntilAndUpdateLen<TUntilIdx>(iter, len);
    }

    template <std::size_t TUntilIdx, typename TIter>
    ErrorStatus readUntilAndUpdateLen(TIter& iter, std::size_t& len)
    {
        auto es = ErrorStatus::Success;
        comms::util::template tupleForEachUntil<TUntilIdx>(value(), makeReadHelper(es, iter, len));
        return es;
    }    

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    ErrorStatus readFromUntil(TIter& iter, std::size_t len)
    {
        return readFromUntilAndUpdateLen<TFromIdx, TUntilIdx>(iter, len);
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    ErrorStatus readFromUntilAndUpdateLen(TIter& iter, std::size_t& len)
    {
        auto es = ErrorStatus::Success;
        comms::util::template tupleForEachFromUntil<TFromIdx, TUntilIdx>(value(), makeReadHelper(es, iter, len));
        return es;
    }    

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        comms::util::tupleForEach(value(), makeReadNoStatusHelper(iter));
    }

    template <std::size_t TFromIdx, typename TIter>
    void readFromNoStatus(TIter& iter)
    {
        comms::util::template tupleForEachFrom<TFromIdx>(value(), makeReadNoStatusHelper(iter));
    }

    template <std::size_t TUntilIdx, typename TIter>
    void readUntilNoStatus(TIter& iter)
    {
        comms::util::template tupleForEachUntil<TUntilIdx>(value(), makeReadNoStatusHelper(iter));
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void readFromUntilNoStatus(TIter& iter)
    {
        comms::util::template tupleForEachFromUntil<TFromIdx, TUntilIdx>(value(), makeReadNoStatusHelper(iter));
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        auto es = ErrorStatus::Success;
        comms::util::tupleForEach(value(), makeWriteHelper(es, iter, len));
        return es;
    }

    template <std::size_t TFromIdx, typename TIter>
    ErrorStatus writeFrom(TIter& iter, std::size_t len) const
    {
        auto es = ErrorStatus::Success;
        comms::util::template tupleForEachFrom<TFromIdx>(value(), makeWriteHelper(es, iter, len));
        return es;
    }

    template <std::size_t TUntilIdx, typename TIter>
    ErrorStatus writeUntil(TIter& iter, std::size_t len) const
    {
        auto es = ErrorStatus::Success;
        comms::util::template tupleForEachUntil<TUntilIdx>(value(), makeWriteHelper(es, iter, len));
        return es;
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    ErrorStatus writeFromUntil(TIter& iter, std::size_t len) const
    {
        auto es = ErrorStatus::Success;
        comms::util::template tupleForEachFromUntil<TFromIdx, TUntilIdx>(value(), makeWriteHelper(es, iter, len));
        return es;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        comms::util::tupleForEach(value(), makeWriteNoStatusHelper(iter));
    }

    template <std::size_t TFromIdx, typename TIter>
    void writeFromNoStatus(TIter& iter) const
    {
        comms::util::template tupleForEachFrom<TFromIdx>(value(), makeWriteNoStatusHelper(iter));
    }

    template <std::size_t TUntilIdx, typename TIter>
    void writeUntilNoStatus(TIter& iter) const
    {
        comms::util::template tupleForEachUntil<TUntilIdx>(value(), makeWriteNoStatusHelper(iter));
    }

    template <std::size_t TFromIdx, std::size_t TUntilIdx, typename TIter>
    void writeFromUntilNoStatus(TIter& iter) const
    {
        comms::util::template tupleForEachFromUntil<TFromIdx, TUntilIdx>(value(), makeWriteNoStatusHelper(iter));
    }

    static constexpr bool isVersionDependent()
    {
        return CommonFuncs::areMembersVersionDependent<ValueType>();
    }

    static constexpr bool hasNonDefaultRefresh()
    {
        return CommonFuncs::doMembersMembersHaveNonDefaultRefresh<ValueType>();
    }

    bool setVersion(VersionType version)
    {
        return CommonFuncs::setVersionForMembers(value(), version);
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

    struct RefreshHelper
    {
        template <typename TField>
        bool operator()(bool soFar, TField& field) const
        {
            return field.refresh() || soFar;
        }
    };

    template <typename TIter>
    class ReadHelper
    {
    public:
        ReadHelper(ErrorStatus& es, TIter& iter, std::size_t& len)
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

            auto fromIter = iter_;
            es_ = field.read(iter_, len_);
            if (es_ == comms::ErrorStatus::Success) {
                len_ -= static_cast<std::size_t>(std::distance(fromIter, iter_));
            }
        }

    private:
        ErrorStatus& es_;
        TIter& iter_;
        std::size_t& len_;
    };

    template <typename TIter>
    static ReadHelper<TIter> makeReadHelper(comms::ErrorStatus& es, TIter& iter, std::size_t& len)
    {
        return ReadHelper<TIter>(es, iter, len);
    }

    template <typename TIter>
    class ReadNoStatusHelper
    {
    public:
        ReadNoStatusHelper(TIter& iter)
          : iter_(iter)
        {
        }

        template <typename TField>
        void operator()(TField& field)
        {
            field.readNoStatus(iter_);
        }

    private:
        TIter& iter_;
    };

    template <typename TIter>
    static ReadNoStatusHelper<TIter> makeReadNoStatusHelper(TIter& iter)
    {
        return ReadNoStatusHelper<TIter>(iter);
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
    static WriteHelper<TIter> makeWriteHelper(ErrorStatus& es, TIter& iter, std::size_t len)
    {
        return WriteHelper<TIter>(es, iter, len);
    }

    template <typename TIter>
    class WriteNoStatusHelper
    {
    public:
        WriteNoStatusHelper(TIter& iter)
          : iter_(iter)
        {
        }

        template <typename TField>
        void operator()(const TField& field)
        {
            field.writeNoStatus(iter_);
        }

    private:
        TIter& iter_;
    };

    template <typename TIter>
    static WriteNoStatusHelper<TIter> makeWriteNoStatusHelper(TIter& iter)
    {
        return WriteNoStatusHelper<TIter>(iter);
    }

    static_assert(comms::util::IsTuple<ValueType>::Value, "ValueType must be tuple");
    ValueType members_;
};

}  // namespace basic

}  // namespace field

}  // namespace comms


