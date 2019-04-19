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

template <typename TTrailField, typename TBase>
class SequenceTrailingFieldSuffix : public TBase
{
    using BaseImpl = TBase;
    using TrailField = TTrailField;

    static_assert(!TrailField::isVersionDependent(),
            "Suffix fields must not be version dependent");

public:
    using ValueType = typename BaseImpl::ValueType;
    using ElementType = typename BaseImpl::ElementType;

    SequenceTrailingFieldSuffix() = default;

    explicit SequenceTrailingFieldSuffix(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit SequenceTrailingFieldSuffix(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    SequenceTrailingFieldSuffix(const SequenceTrailingFieldSuffix&) = default;
    SequenceTrailingFieldSuffix(SequenceTrailingFieldSuffix&&) = default;
    SequenceTrailingFieldSuffix& operator=(const SequenceTrailingFieldSuffix&) = default;
    SequenceTrailingFieldSuffix& operator=(SequenceTrailingFieldSuffix&&) = default;

    constexpr std::size_t length() const
    {
        return trailField_.length() + BaseImpl::length();
    }

    static constexpr std::size_t minLength()
    {
        return TrailField::minLength() + BaseImpl::minLength();
    }

    static constexpr std::size_t maxLength()
    {
        return TrailField::maxLength() + BaseImpl::maxLength();
    }

    bool valid() const
    {
        return trailField_.valid() && BaseImpl::valid();
    }

    template <typename TIter>
    comms::ErrorStatus read(TIter& iter, std::size_t len)
    {
        auto es = BaseImpl::read(iter, len - TrailField::minLength());
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return trailField_.read(iter, len - BaseImpl::length());
    }

    template <typename TIter>
    void readNoStatus(TIter& iter) = delete;

    template <typename TIter>
    comms::ErrorStatus write(TIter& iter, std::size_t len) const
    {
        auto trailLen = trailField_.length();
        auto es = BaseImpl::write(iter, len - trailLen);
        if (es != comms::ErrorStatus::Success) {
            return es;
        }

        return trailField_.write(iter, trailLen);
    }

    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        BaseImpl::writeNoStatus(iter);
        trailField_.writeNoStatus(iter);
    }

private:
    TrailField trailField_;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




