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

#include "comms/Assert.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TTrailField, typename TBase>
class SequenceTrailingFieldSuffix : public TBase
{
    using Base = TBase;
    using TrailField = TTrailField;

public:
    using ValueType = typename Base::ValueType;
    using ElementType = typename Base::ElementType;

    SequenceTrailingFieldSuffix() = default;

    explicit SequenceTrailingFieldSuffix(const ValueType& val)
      : Base(val)
    {
    }

    explicit SequenceTrailingFieldSuffix(ValueType&& val)
      : Base(std::move(val))
    {
    }

    SequenceTrailingFieldSuffix(const SequenceTrailingFieldSuffix&) = default;
    SequenceTrailingFieldSuffix(SequenceTrailingFieldSuffix&&) = default;
    SequenceTrailingFieldSuffix& operator=(const SequenceTrailingFieldSuffix&) = default;
    SequenceTrailingFieldSuffix& operator=(SequenceTrailingFieldSuffix&&) = default;

    constexpr std::size_t length() const
    {
        return trailField_.length() + Base::length();
    }

    static constexpr std::size_t minLength()
    {
        return TrailField::minLength() + Base::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return TrailField::maxLength() + Base::maxLength();
    }

    bool valid() const
    {
        return trailField_.valid() && Base::valid();
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto es = Base::read(iter, len - TrailField::minLength());
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return trailField_.read(iter, len - Base::length());
    }

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        auto trailLen = trailField_.length();
        auto es = Base::write(iter, len - trailLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return trailField_.write(iter, trailLen);
    }

private:
    TrailField trailField_;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




