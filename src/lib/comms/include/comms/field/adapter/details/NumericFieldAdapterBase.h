//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include "CommonBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

namespace details
{

template <typename TNext>
class NumericFieldAdapterBase : public CommonBase<TNext>
{
    typedef CommonBase<TNext> Base;
public:
    typedef typename Base::Next Next;
    typedef typename Base::ParamValueType ParamValueType;
    typedef typename Next::SerialisedType SerialisedType;

    constexpr SerialisedType toSerialised(ParamValueType value)
    {
        return Base::next().toSerialised(value);
    }

    static constexpr ParamValueType fromSerialised(SerialisedType value)
    {
        return Base::next().fromSerialised(value);
    }
};

}  // namespace details

}  // namespace adapter

}  // namespace field

}  // namespace comms




