//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms/field/category.h"
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
class OptionalFieldAdapterBase : public CommonBase<TNext>
{
    using Base = CommonBase<TNext>;
public:
    using Category = typename Base::Category;
    using Next = typename Base::Next;
    using ValueType = typename Base::ValueType;
    using Field = typename Next::Field;
    using Mode = typename Next::Mode;

    static_assert(
        std::is_base_of<comms::field::category::OptionalField, Category>::value,
        "This adapter base class is expected to wrap optional field.");

    OptionalFieldAdapterBase(const OptionalFieldAdapterBase&) = default;
    OptionalFieldAdapterBase(OptionalFieldAdapterBase&&) = default;
    OptionalFieldAdapterBase& operator=(const OptionalFieldAdapterBase&) = default;
    OptionalFieldAdapterBase& operator=(OptionalFieldAdapterBase&&) = default;

    Field& field()
    {
        return Base::next().field();
    }

    const Field& field() const
    {
        return Base::next().field();
    }

    Mode getMode() const
    {
        return Base::next().getMode();
    }

    void setMode(Mode val)
    {
        Base::next().setMode(val);
    }

protected:
    OptionalFieldAdapterBase() = default;
    explicit OptionalFieldAdapterBase(const ValueType& val)
      : Base(val)
    {
    }
};

}  // namespace details

}  // namespace adapter

}  // namespace field

}  // namespace comms




