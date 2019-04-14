//
// Copyright    2017 - 2019 (C). Alex Robenko. All rights reserved.
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

template <typename TBase>
class EmptySerialization : public TBase
{
    using BaseImpl = TBase;
public:

    using ValueType = typename BaseImpl::ValueType;

    EmptySerialization() = default;

    explicit EmptySerialization(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit EmptySerialization(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    EmptySerialization(const EmptySerialization&) = default;
    EmptySerialization(EmptySerialization&&) = default;
    EmptySerialization& operator=(const EmptySerialization&) = default;
    EmptySerialization& operator=(EmptySerialization&&) = default;

    static constexpr std::size_t length()
    {
        return 0U;
    }

    static constexpr std::size_t minLength()
    {
        return length();
    }

    static constexpr std::size_t maxLength()
    {
        return length();
    }

    template <typename TIter>
    static comms::ErrorStatus read(TIter&, std::size_t)
    {
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    static void readNoStatus(TIter&)
    {
    }

    template <typename TIter>
    static comms::ErrorStatus write(TIter&, std::size_t)
    {
        return comms::ErrorStatus::Success;
    }

    template <typename TIter>
    static void writeNoStatus(TIter&)
    {
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

