//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
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
#include <cstddef>

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
class VariantFieldAdapterBase : public CommonBase<TNext>
{
    using Base = CommonBase<TNext>;
public:
    using Category = typename Base::Category;
    using Next = typename Base::Next;
    using ValueType = typename Base::ValueType;

    static_assert(
        std::is_base_of<comms::field::category::VariantField, Category>::value,
        "This adapter base class is expected to wrap variant field.");

    std::size_t currentField() const
    {
        return Base::next().currentField();
    }

    void selectField(std::size_t idx)
    {
        Base::next().selectField(idx);
    }

    template <typename TFunc>
    void currentFieldExec(TFunc&& func)
    {
        Base::next().currentFieldExec(std::forward<TFunc>(func));
    }

    template <typename TFunc>
    void currentFieldExec(TFunc&& func) const
    {
        Base::next().currentFieldExec(std::forward<TFunc>(func));
    }


protected:
    VariantFieldAdapterBase() = default;
    VariantFieldAdapterBase(const VariantFieldAdapterBase&) = default;
    VariantFieldAdapterBase(VariantFieldAdapterBase&&) = default;
    VariantFieldAdapterBase& operator=(const VariantFieldAdapterBase&) = default;
    VariantFieldAdapterBase& operator=(VariantFieldAdapterBase&&) = default;
};

}  // namespace details

}  // namespace adapter

}  // namespace field

}  // namespace comms




