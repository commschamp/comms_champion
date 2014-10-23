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

#include "comms/field/options.h"
#include "comms/util/SizeToType.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TField, std::size_t TLen, typename... TOptions>
class BitmaskValueBase;

template <typename TField, std::size_t TLen>
class BitmaskValueBase<TField, TLen> : public TField
{
protected:
    typedef typename util::SizeToType<TLen>::Type ValueType;

    static const auto DefaultValue = static_cast<ValueType>(0);
    static const std::size_t SerialisedLen = TLen;
    static const auto ReservedMask = static_cast<ValueType>(0);
    static const bool ReservedValue = false;
};

template <typename TField, std::size_t TLen, long long int TValue, typename... TOptions>
class BitmaskValueBase<
    TField,
    TLen,
    comms::field::option::DefaultValueImpl<TValue>,
    TOptions...> : public BitmaskValueBase<TField, TLen, TOptions...>
{
    typedef BitmaskValueBase<TField, TLen, TOptions...> Base;

protected:
    using Base::BitmaskValueBase;

    static const auto DefaultValue = static_cast<decltype(Base::DefaultValue)>(TValue);
};

template <typename TField, std::size_t TLen, long long unsigned TMask, bool TValue, typename... TOptions>
class BitmaskValueBase<
    TField,
    TLen,
    comms::field::option::ReservedBitsImpl<TMask, TValue>,
    TOptions...> : public BitmaskValueBase<TField, TLen, TOptions...>
{
    typedef BitmaskValueBase<TField, TLen, TOptions...> Base;

protected:
    using Base::BitmaskValueBase;

    static const auto ReservedMask = static_cast<decltype(Base::ReservedMask)>(TMask);
    static const auto ReservedValue = static_cast<decltype(Base::ReservedValue)>(TValue);
};


}  // namespace details

}  // namespace field

}  // namespace comms


