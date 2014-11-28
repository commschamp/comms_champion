//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

namespace option
{

template<long long int TMinValue, long long int TMaxValue>
struct ValidRangeImpl
{
    static const auto MinValue = TMinValue;
    static const auto MaxValue = TMaxValue;
};

template<std::size_t TLen>
struct LengthLimitImpl
{
    static const std::size_t Value = TLen;
};

template<long long int TOffset>
struct SerOffsetImpl
{
    static const auto Value = TOffset;
};

template<long long int TVal>
struct DefaultValueImpl
{
    static const auto Value = TVal;
};

template<long long unsigned TMask, bool TValue>
struct BitmaskReservedBitsImpl
{
    static const auto Mask = TMask;
    static const auto Value = TValue;
};

struct BitmaskBitOrderLsbFirstImpl {};


}  // namespace option

}  // namespace field

}  // namespace comms


