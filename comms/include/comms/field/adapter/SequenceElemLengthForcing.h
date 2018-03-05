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

#include <iterator>

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

#include <limits>

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class SequenceElemLengthForcing : public TBase
{
    using BaseImpl = TBase;
public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceElemLengthForcing() = default;

    explicit SequenceElemLengthForcing(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceElemLengthForcing(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceElemLengthForcing(const SequenceElemLengthForcing&) = default;
    SequenceElemLengthForcing(SequenceElemLengthForcing&&) = default;
    SequenceElemLengthForcing& operator=(const SequenceElemLengthForcing&) = default;
    SequenceElemLengthForcing& operator=(SequenceElemLengthForcing&&) = default;

    void forceElemLength(std::size_t val)
    {
        GASSERT(val != Cleared);
        forced_ = val;
    }

    void clearElemLengthForcing()
    {
        forced_ = Cleared;
    }

    std::size_t length() const
    {
        if (forced_ != Cleared) {
            return BaseImpl::value().size() * forced_;
        }

        return BaseImpl::length();
    }

    std::size_t elementLength(const ElementType& elem) const
    {
        if (forced_ != Cleared) {
            return forced_;
        }
        return BaseImpl::elementLength(elem);
    }

    static constexpr std::size_t maxElementLength()
    {
        return 0xffff;
    }

    template <typename TIter>
    ErrorStatus readElement(ElementType& elem, TIter& iter, std::size_t& len) const
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(std::is_base_of<std::random_access_iterator_tag, IterTag>::value,
            "Only random access iterator for reading is supported with comms::option::SequenceElemLengthForcingEnabled option");

        if (forced_ == Cleared) {
            return BaseImpl::readElement(elem, iter, len);
        }

        if (len < forced_) {
            return comms::ErrorStatus::NotEnoughData;
        }

        auto iterTmp = iter;
        auto remLen = forced_;
        std::advance(iter, forced_);
        len -= forced_;
        return BaseImpl::readElement(elem, iterTmp, remLen);
    }

    template <typename TIter>
    void readElementNoStatus(ElementType& elem, TIter& iter) const
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using IterTag = typename std::iterator_traits<IterType>::iterator_category;
        static_assert(std::is_base_of<std::random_access_iterator_tag, IterTag>::value,
            "Only random access iterator for reading is supported with comms::option::SequenceElemLengthForcingEnabled option");

        if (forced_ == Cleared) {
            return BaseImpl::readElementNoStatus(elem, iter);
        }

        auto fromIter = iter;
        auto es = BaseImpl::readElementNoStatus(elem, iter);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto consumed = std::distance(fromIter, iter);
        if (consumed < forced_) {
            std::advance(iter, forced_ - consumed);
        }
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        BaseImpl::clear();
        auto remLen = len;
        while (0 < remLen) {
            auto& elem = BaseImpl::createBack();
            auto es = readElement(elem, iter, remLen);
            if (es != ErrorStatus::Success) {
                BaseImpl::value().pop_back();
                return es;
            }
        }

        return ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    ErrorStatus readN(std::size_t count, TIter& iter, std::size_t& len)
    {
        BaseImpl::clear();
        while (0 < count) {
            auto& elem = BaseImpl::createBack();
            auto es = readElement(elem, iter, len);
            if (es != comms::ErrorStatus::Success) {
                BaseImpl::value().pop_back();
                return es;
            }
            --count;
        }
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    void readNoStatusN(std::size_t count, TIter& iter)
    {
        BaseImpl::clear();
        while (0 < count) {
            auto& elem = BaseImpl::createBack();
            readElementNoStatus(elem, iter);
            --count;
        }
    }

    template <typename TIter>
    ErrorStatus writeElement(const ElementType& elem, TIter& iter, std::size_t& len) const
    {
        if (forced_ == Cleared) {
            return BaseImpl::writeElement(elem, iter, len);
        }

        auto realLength = BaseImpl::elementLength(elem);
        auto remLen = forced_;
        len -= forced_;
        auto es = BaseImpl::writeElement(elem, iter, remLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto padLength = forced_ - realLength;
        advanceWriteIterator(iter, padLength);
        return es;
    }

    template <typename TIter>
    static void writeElementNoStatus(const ElementType& elem, TIter& iter);

    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t len) const
    {
        if (len < length()) {
            return ErrorStatus::BufferOverflow;
        }

        auto es = ErrorStatus::Success;
        auto remainingLen = len;
        auto& val = BaseImpl::value();
        for (auto fieldIter = val.begin(); fieldIter != val.end(); ++fieldIter) {
            es = writeElement(*fieldIter, iter, remainingLen);
            if (es != ErrorStatus::Success) {
                break;
            }
        }

        return es;
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const = delete;

    template <typename TIter>
    ErrorStatus writeN(std::size_t count, TIter& iter, std::size_t& len) const
    {
        auto& val = BaseImpl::value();
        if ((val.size() <= count) && (len < length())) {
            return ErrorStatus::BufferOverflow;
        }

        auto es = ErrorStatus::Success;
        for (auto fieldIter = val.begin(); fieldIter != val.end(); ++fieldIter) {
            if (count == 0) {
                break;
            }

            es = writeElement(*fieldIter, iter, len);
            if (es != ErrorStatus::Success) {
                break;
            }

            --count;
        }

        return es;
    }

    template <typename TIter>
    void writeNoStatusN(std::size_t count, TIter& iter) const = delete;

private:

    template <typename TIter>
    static void advanceWriteIterator(TIter& iter, std::size_t len)
    {
        using IterType = typename std::decay<decltype(iter)>::type;
        using ByteType = typename std::iterator_traits<IterType>::value_type;
        while (len > 0U) {
            *iter = ByteType();
            ++iter;
            --len;
        }
    }


    static const std::size_t Cleared = std::numeric_limits<std::size_t>::max();
    std::size_t forced_ = Cleared;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms


