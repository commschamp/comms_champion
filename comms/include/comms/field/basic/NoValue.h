//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/ErrorStatus.h"

namespace comms
{

namespace field
{

namespace basic
{

template <typename TFieldBase>
class NoValue : public TFieldBase
{
    using BaseImpl = TFieldBase;
public:

    using ValueType = unsigned;
    using SerialisedType = ValueType;

    NoValue() = default;

    NoValue(const NoValue&) = default;
    NoValue(NoValue&&) = default;
    ~NoValue() noexcept = default;

    NoValue& operator=(const NoValue&) = default;
    NoValue& operator=(NoValue&&) = default;

    static ValueType& value()
    {
        static ValueType value = ValueType();
        return value;
    }

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

    static constexpr SerialisedType toSerialised(ValueType val)
    {
        return static_cast<SerialisedType>(val);
    }

    static constexpr ValueType fromSerialised(SerialisedType val)
    {
        return static_cast<ValueType>(val);
    }

    template <typename TIter>
    static ErrorStatus read(TIter& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    static void readNoStatus(TIter& iter)
    {
        static_cast<void>(iter);
    }

    template <typename TIter>
    static ErrorStatus write(TIter& iter, std::size_t size)
    {
        static_cast<void>(iter);
        static_cast<void>(size);
        return ErrorStatus::Success;
    }

    template <typename TIter>
    static void writeNoStatus(TIter& iter)
    {
        static_cast<void>(iter);
    }
};

}  // namespace basic

}  // namespace field

}  // namespace comms


