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

#include "details/AdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TInitialiser, typename TNext>
class DefaultValueInitialiser : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
    typedef TInitialiser Initialiser;
public:
    typedef typename Base::ParamValueType ParamValueType;

    DefaultValueInitialiser()
    {
        Initialiser()(*this);
    }

    explicit DefaultValueInitialiser(ParamValueType value)
      : Base(value)
    {
    }

    DefaultValueInitialiser(const DefaultValueInitialiser&) = default;
    DefaultValueInitialiser(DefaultValueInitialiser&&) = default;
    DefaultValueInitialiser& operator=(const DefaultValueInitialiser&) = default;
    DefaultValueInitialiser& operator=(DefaultValueInitialiser&&) = default;
};

}  // namespace adapter

}  // namespace field

}  // namespace comms



