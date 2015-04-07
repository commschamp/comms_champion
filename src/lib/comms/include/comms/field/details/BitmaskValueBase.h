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

#include "comms/options.h"
#include "comms/util/SizeToType.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TField, typename... TOptions>
class BitmaskValueBase;

template <typename TField>
class BitmaskValueBase<TField> : public TField
{
protected:
    typedef long long unsigned ValueType;

    static const auto DefaultValue = static_cast<ValueType>(0);
    static const std::size_t SerialisedLen = sizeof(ValueType);
    static const auto ReservedMask = static_cast<ValueType>(0);
    static const bool ReservedValue = false;
    static const bool BitZeroIsMsb = false;
};

template <typename TField, long long int TValue, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::DefaultNumValue<TValue>,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{
    typedef BitmaskValueBase<TField, TOptions...> Base;
    typedef comms::option::DefaultNumValue<TValue> Option;

protected:
    using Base::BitmaskValueBase;

    static const auto DefaultValue = static_cast<decltype(Base::DefaultValue)>(Option::Value);
};

template <typename TField, long long unsigned TMask, bool TValue, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::BitmaskReservedBits<TMask, TValue>,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{
    typedef BitmaskValueBase<TField, TOptions...> Base;
    typedef comms::option::BitmaskReservedBits<TMask, TValue> Option;

protected:
    using Base::BitmaskValueBase;

    static const auto ReservedMask = static_cast<decltype(Base::ReservedMask)>(Option::Mask);
    static const auto ReservedValue = static_cast<decltype(Base::ReservedValue)>(Option::Value);
};

template <typename TField, std::size_t TLen, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::FixedLength<TLen>,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{

    typedef BitmaskValueBase<TField, TOptions...> Base;
    typedef comms::option::FixedLength<TLen> Option;

protected:
    using Base::BitmaskValueBase;

    using ValueType = typename comms::util::SizeToType<TLen, false>::Type;

    static const auto DefaultValue = static_cast<ValueType>(Base::DefaultValue);
    static const std::size_t SerialisedLen = Option::Value;
    static const auto ReservedMask = static_cast<ValueType>(Base::ReservedMask);
};

template <typename TField, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::BitIndexingStartsFromMsb,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{
    typedef BitmaskValueBase<TField, TOptions...> Base;

protected:
    using Base::BitmaskValueBase;

    static const bool BitZeroIsMsb = true;
};

}  // namespace details

}  // namespace field

}  // namespace comms


