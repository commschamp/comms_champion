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

#include <cstdint>
#include <cassert>
#include <memory>

#include "comms/field/BasicIntValue.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

using BasicIntValueWrapper = NumericValueWrapper<int>;

template <typename TField>
class BasicIntValueWrapperT : public NumericValueWrapperT<BasicIntValueWrapper, TField>
{
    using Base = NumericValueWrapperT<BasicIntValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isBasicIntValue<Field>(), "Must be of BasicIntValueField type");

    using ValueType = typename Field::ValueType;
    using IntType = typename Base::UnderlyingType;
    static_assert(sizeof(ValueType) <= sizeof(IntType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(IntType)),
        "This wrapper cannot handle provided field.");

public:
    BasicIntValueWrapperT(Field& field)
      : Base(field)
    {
    }

    BasicIntValueWrapperT(const BasicIntValueWrapperT&) = default;
    BasicIntValueWrapperT(BasicIntValueWrapperT&&) = default;
    virtual ~BasicIntValueWrapperT() = default;

    BasicIntValueWrapperT& operator=(const BasicIntValueWrapperT&) = delete;
};

using BasicIntValueWrapperPtr = std::unique_ptr<BasicIntValueWrapper>;

template <typename TField>
BasicIntValueWrapperPtr
makeBasicIntValueWrapper(TField& field)
{
    return
        std::unique_ptr<BasicIntValueWrapper>(
            new BasicIntValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
