//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "comms/field/EnumValue.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class CC_API EnumValueWrapper : public NumericValueWrapper<long long int>
{
    using Base = NumericValueWrapper<long long int>;
public:
    using UnderlyingType = Base::UnderlyingType;
    typedef std::unique_ptr<EnumValueWrapper> Ptr;

    EnumValueWrapper();
    virtual ~EnumValueWrapper() noexcept;

    Ptr clone();

protected:
    virtual Ptr cloneImpl() = 0;

    void dispatchImpl(FieldWrapperHandler& handler);
};

template <typename TField>
class EnumValueWrapperT : public NumericValueWrapperT<EnumValueWrapper, TField>
{
    using Base = NumericValueWrapperT<EnumValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isEnumValue<Field>(), "Must be of EnumValueField type");

    using ValueType = typename Field::ValueType;
    using UnderlyingType = typename Base::UnderlyingType;
    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
//    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(UnderlyingType)),
//        "This wrapper cannot handle provided field.");

public:
    typedef typename Base::Ptr Ptr;

    explicit EnumValueWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    EnumValueWrapperT(const EnumValueWrapperT&) = default;
    EnumValueWrapperT(EnumValueWrapperT&&) = default;
    virtual ~EnumValueWrapperT() noexcept = default;

    EnumValueWrapperT& operator=(const EnumValueWrapperT&) = delete;

protected:
    virtual Ptr cloneImpl() override
    {
        return Ptr(new EnumValueWrapperT<TField>(Base::field()));
    }

};

using EnumValueWrapperPtr = std::unique_ptr<EnumValueWrapper>;

template <typename TField>
EnumValueWrapperPtr
makeEnumValueWrapper(TField& field)
{
    return
        EnumValueWrapperPtr(
            new EnumValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
