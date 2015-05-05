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

template <typename TValidator, typename TNext>
class CustomValidator : public details::AdapterBaseT<TNext>
{
    typedef details::AdapterBaseT<TNext> Base;
    typedef TValidator Validator;

public:

    typedef typename Base::ParamValueType ParamValueType;

    CustomValidator() = default;

    explicit CustomValidator(ParamValueType value)
      : Base(value)
    {
    }

    CustomValidator(const CustomValidator&) = default;
    CustomValidator(CustomValidator&&) = default;
    CustomValidator& operator=(const CustomValidator&) = default;
    CustomValidator& operator=(CustomValidator&&) = default;

    bool valid() const
    {
        return Base::valid() && (Validator()(*this));
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




