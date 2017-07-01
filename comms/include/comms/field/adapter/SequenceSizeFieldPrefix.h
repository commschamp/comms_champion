//
// Copyright 2015 - 2017 (C). Alex Robenko. All rights reserved.
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
    using Base = TBase;
    using SizeField = TSizeField;

public:
    using ValueType = typename Base::ValueType;
    using ElementType = typename Base::ElementType;

    SequenceSizeFieldPrefix() = default;

    explicit SequenceSizeFieldPrefix(const ValueType& val)
      : Base(val)
    {
    }

    explicit SequenceSizeFieldPrefix(ValueType&& val)
      : Base(std::move(val))
    {
    }

    SequenceSizeFieldPrefix(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix(SequenceSizeFieldPrefix&&) = default;
    SequenceSizeFieldPrefix& operator=(const SequenceSizeFieldPrefix&) = default;
    SequenceSizeFieldPrefix& operator=(SequenceSizeFieldPrefix&&) = default;

    constexpr std::size_t length() const
    {
        using SizeValueType = typename SizeField::ValueType;
        return
            SizeField(static_cast<SizeValueType>(Base::value().size())).length() +
            Base::length();
    }

    static constexpr std::size_t minLength()
    {
        return SizeField::minLength() + Base::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return SizeField::maxLength() + Base::maxLength();
    }

    bool valid() const
    {
        using SizeValueType = typename SizeField::ValueType;
        return
            SizeField(static_cast<SizeValueType>(Base::value().size())).valid() &&
            Base::valid();
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        SizeField sizeField;
        auto es = sizeField.read(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        auto count = static_cast<std::size_t>(sizeField.value());
        len -= sizeField.length();

        return Base::readN(count, iter, len);
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        using SizeValueType = typename SizeField::ValueType;
        SizeField sizeField(static_cast<SizeValueType>(Base::value().size()));
        auto es = sizeField.write(iter, len);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        GASSERT(sizeField.length() <= len);
        return Base::write(iter, len - sizeField.length());
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




