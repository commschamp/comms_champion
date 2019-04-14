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

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class InvalidByDefault : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    InvalidByDefault() = default;

    explicit InvalidByDefault(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit InvalidByDefault(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    InvalidByDefault(const InvalidByDefault&) = default;
    InvalidByDefault(InvalidByDefault&&) = default;
    InvalidByDefault& operator=(const InvalidByDefault&) = default;
    InvalidByDefault& operator=(InvalidByDefault&&) = default;

    static constexpr bool valid()
    {
        return false;
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

