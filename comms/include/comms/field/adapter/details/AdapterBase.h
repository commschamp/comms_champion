//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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
#include "NumericFieldAdapterBase.h"
#include "CollectionFieldAdapterBase.h"
#include "BundleFieldAdapterBase.h"
#include "OptionalFieldAdapterBase.h"

namespace comms
{

namespace field
{

namespace adapter
{

namespace details
{

template <typename TNext>
struct AdapterBase
{
    static_assert(comms::field::category::isCategorised<TNext>(),
        "Unexpected type.");

    typedef typename std::conditional<
        std::is_base_of<comms::field::category::NumericValueField, typename TNext::Category>::value,
        NumericFieldAdapterBase<TNext>,
        typename std::conditional<
            std::is_base_of<comms::field::category::CollectionField, typename TNext::Category>::value,
            CollectionFieldAdapterBase<TNext>,
            typename std::conditional<
                std::is_base_of<comms::field::category::BundleField, typename TNext::Category>::value,
                BundleFieldAdapterBase<TNext>,
                typename std::conditional<
                    std::is_base_of<comms::field::category::OptionalField, typename TNext::Category>::value,
                    OptionalFieldAdapterBase<TNext>,
                    void
                >::type
            >::type
        >::type
    >::type Type;

    static_assert(!std::is_void<Type>::value, "Unknown category!");
};

template <typename TNext>
using AdapterBaseT = typename AdapterBase<TNext>::Type;

}  // namespace details

}  // namespace adapter

}  // namespace field

}  // namespace comms




