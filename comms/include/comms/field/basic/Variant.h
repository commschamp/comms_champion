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
class Variant : public TFieldBase
{
public:
    using Category = comms::field::category::BundleField;
    using Members = TMembers;
    using ValueType = comms::util::TupleAsAlignedUnionT<Members>;

    Variant() = default;
    explicit Variant(const ValueType& val)
      : storage_(val)
    {
    }

    explicit Variant(ValueType&& val)
      : storage_(std::move(val))
    {
    }

    Variant(const Variant&) = default;
    Variant(Variant&&) = default;
    ~Variant()
    {
        checkDestruct();
    }

    Variant& operator=(const Variant&) = default;
    Variant& operator=(Variant&&) = default;

    const ValueType& value() const
    {
        return storage_;
    }

    ValueType& value()
    {
        return storage_;
    }

    std::size_t length() const
    {
        std::size_t len = std::numeric_limits<std::size_t>::max();
        comms::util::tupleForSelectedType<Members>(memIdx_, LengthCalcHelper(len, &storage_));
        return len;
    }

    static constexpr std::size_t minLength()
    {
        return comms::util::tupleTypeAccumulate<ValueType>(std::numeric_limits<std::size_t>::max(), MinLengthCalcHelper());
    }

    static constexpr std::size_t maxLength()
    {
        return comms::util::tupleTypeAccumulate<ValueType>(std::size_t(0), MaxLengthCalcHelper());
    }

    constexpr bool valid() const
    {
        bool val = false;
        comms::util::tupleForSelectedType<Members>(memIdx_, ValidCheckHelper(val, storage_));
        return val;
    }

    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t len)
    {
        checkDestruct();
        auto es = comms::ErrorStatus::NumOfErrorStatuses;
        comms::util::tupleForEachType<Members>(makeReadHelper(es, iter, len, storage_));
        GASSERT((es == comms::ErrorStatus::Success) || (memIdx_ < MembersCount));
        GASSERT((es != comms::ErrorStatus::Success) || (MembersCount <= memIdx_));

        return es;
    }

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (MembersCount <= memIdx_) {
            return comms::ErrorStatus::NotSupported;
        }

        auto es = ErrorStatus::NumOfErrorStatuses;
        comms::util::tupleForSelectedType<Members>(memIdx_, makeWriteHelper(es, iter, len, storage_));
        return es;
    }


private:
    class DestructHelper
    {
    public:
        DestructHelper(void* storage) : storage_(storage) {}

        template <typename TField>
        void operator()() const
        {
            reinterpret_cast<TField*>(storage_)->~TField();
        }
    private:
        void* storage_ = nullptr;
    };

    class LengthCalcHelper
    {
    public:
        LengthCalcHelper(std::size_t& len, void* storage)
          : len_(len),
            storage_(storage)
        {
        }

        template <typename TField>
        void operator()() const
        {
            len_ = reinterpret_cast<const TField*>(storage_)->length();
        }

    private:
        std::size_t& len_;
        void* storage_;
    };

    struct MinLengthCalcHelper
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t val) const
        {
            return std::min(val, TField::minLength());
        }
    };

    struct MaxLengthCalcHelper
    {
        template <typename TField>
        constexpr std::size_t operator()(std::size_t val) const
        {
            return std::max(val, TField::maxLength());
        }
    };

    class ValidCheckHelper
    {
    public:
        ValidCheckHelper(bool& result, void* storage)
          : result_(result),
            storage_(storage)
        {
        }

        template <typename TField>
        void operator()() const
        {
            result_ = reinterpret_cast<const TField*>(storage_)->valid();
        }

    private:
        bool& result_;
        void* storage_;
    };


    template <typename TIter>
    class ReadHelper
    {
    public:
        ReadHelper(
            std::size_t& idx,
            comms::ErrorStatus& es,
            TIter& iter,
            std::size_t len,
            void* storage)
          : idx_(idx),
            es_(es),
            iter_(iter),
            len_(len),
            storage_(storage)
        {
            using IterType = typename std::decay<decltype(iter)>::type;
            using IterCategory = typename std::iterator_traits<IterType>::iterator_category;
            static_assert(std::is_base_of<std::random_access_iterator_tag, IterCategory>::value,
                "Variant field only supports read with random access iterators");

            es_ == comms::ErrorStatus::NumOfErrorStatuses;
        }

        template <typename TField>
        void operator()()
        {
            if (readComplete_) {
                return;
            }

            auto* field = new (storage_) TField;

            auto iterTmp = iter_;
            auto es = field->read(iterTmp, len_);
            if (es == comms::ErrorStatus::Success) {
                iter_ = iterTmp;
                es_ = es;
                readComplete_ = true;
                return;
            }

            field->~TField();

            if ((es_ == comms::ErrorStatus::NumOfErrorStatuses) ||
                (es == comms::ErrorStatus::NotEnoughData)) {
                es_ = es;
            }

            ++idx_;
        }

    private:
        std::size_t& idx_;
        comms::ErrorStatus& es_;
        TIter& iter_;
        std::size_t len_ = 0;
        void* storage_ = nullptr;
        bool readComplete_ = false;
    };

    template <typename TIter>
    ReadHelper<TIter> makeReadHelper(comms::ErrorStatus& es, TIter& iter, std::size_t len, void* storage)
    {
        memIdx_ = 0;
        return ReadHelper<TIter>(memIdx_, es, iter, len, storage);
    }

    template <typename TIter>
    class WriteHelper
    {
    public:
        WriteHelper(ErrorStatus& es, TIter& iter, std::size_t len, void* storage)
          : es_(es),
            iter_(iter),
            len_(len),
            storage_(storage)
        {
        }

        template <typename TField>
        void operator()()
        {
            es_ = reinterpret_cast<const TField*>(storage_)->write(iter_, len_);
        }

    private:
        ErrorStatus& es_;
        TIter& iter_;
        std::size_t len_ = 0U;
        void* storage_ = nullptr;
    };

    template <typename TIter>
    static WriteHelper<TIter> makeWriteHelper(comms::ErrorStatus& es, TIter& iter, std::size_t len, void* storage)
    {
        return WriteHelper<TIter>(es, iter, len, storage);
    }

    void checkDestruct()
    {
        if (memIdx_ < MembersCount) {
            comms::util::tupleForSelectedType<Members>(memIdx_, DestructHelper(&storage_));
        }
    }

    ValueType storage_;
    std::size_t memIdx_ = MembersCount;

    static const std::size_t MembersCount = std::tuple_size<Members>::value;
    static_assert(comms::util::IsTuple<Members>::Value, "ValueType must be tuple");
    static_assert(0U < MembersCount, "ValueType must be non-empty tuple");
};

}  // namespace basic

}  // namespace field

}  // namespace comms


