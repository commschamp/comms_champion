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

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TSizeField, typename TBase>
class SequenceSizeFieldPrefix : public TBase
{
    using BaseImpl = TBase;
    using SizeField = TSizeField;

    static_assert(!SizeField::isVersionDependent(),
            "Prefix fields must not be version dependent");

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceSizeFieldPrefix() = default;

    explicit SequenceSizeFieldPrefix(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceSizeFieldPrefix(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceSizeFieldPrefix(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix(SequenceSizeFieldPrefix&&) = default;
    SequenceSizeFieldPrefix& operator=(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix& operator=(SequenceSizeFieldPrefix&&) = default;

    std::size_t length() const
    {
        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField;
        sizeField.value() = static_cast<SizeValueType>(BaseImpl::value().size());
        return sizeField.length() + BaseImpl::length();
    }

    static constexpr std::size_t minLength()
    {
        return SizeField::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return SizeField::maxLength() + BaseImpl::maxLength();
    }

    bool valid() const
    {
        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField;
        sizeField.value() = static_cast<SizeValueType>(BaseImpl::value().size());
        return sizeField.valid() && BaseImpl::valid();
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto fromIter = iter;
        SizeField sizeField;
        auto es = sizeField.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto diff = static_cast<std::size_t>(std::distance(fromIter, iter));
        COMMS_ASSERT(diff <= len);
        len -= diff;

        auto count = static_cast<std::size_t>(sizeField.value());
        return BaseImpl::readN(count, iter, len);
    }

    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        SizeField sizeField;
        sizeField.readNoStatus(iter);
        auto count = static_cast<std::size_t>(sizeField.value());
        BaseImpl::readNoStatusN(count, iter);
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField;
        sizeField.value() = static_cast<SizeValueType>(BaseImpl::value().size());
        auto es = sizeField.write(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        COMMS_ASSERT(sizeField.length() <= len);
        return BaseImpl::write(iter, len - sizeField.length());
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField;
        sizeField.value() = static_cast<SizeValueType>(BaseImpl::value().size());
        sizeField.writeNoStatus(iter);
        BaseImpl::writeNoStatus(iter);
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




