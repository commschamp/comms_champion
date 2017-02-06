//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <cstdint>
#include <cassert>
#include <memory>

#include "comms/field/IntValue.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class CC_API FloatValueWrapper : public NumericValueWrapper<double>
{
    typedef NumericValueWrapper<double> Base;
public:

    typedef Base::UnderlyingType UnderlyingType;
    typedef std::unique_ptr<FloatValueWrapper> Ptr;

    using Base::NumericValueWrapper;

    FloatValueWrapper();
    virtual ~FloatValueWrapper();

    Ptr clone();

protected:
    virtual Ptr cloneImpl() = 0;

    void dispatchImpl(FieldWrapperHandler& handler);

};

template <typename TField>
class FloatValueWrapperT : public NumericValueWrapperT<FloatValueWrapper, TField>
{
    using Base = NumericValueWrapperT<FloatValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isFloatValue<Field>(), "Must be of FloatValueField type");

public:

    typedef typename Base::UnderlyingType UnderlyingType;
    typedef typename Base::Ptr Ptr;

    explicit FloatValueWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    FloatValueWrapperT(const FloatValueWrapperT&) = default;
    FloatValueWrapperT(FloatValueWrapperT&&) = default;
    virtual ~FloatValueWrapperT() = default;

    FloatValueWrapperT& operator=(const FloatValueWrapperT&) = delete;

protected:
    virtual Ptr cloneImpl() override
    {
        return Ptr(new FloatValueWrapperT<TField>(Base::field()));
    }
};

using FloatValueWrapperPtr = FloatValueWrapper::Ptr;

template <typename TField>
FloatValueWrapperPtr
makeFloatValueWrapper(TField& field)
{
    return
        FloatValueWrapperPtr(
            new FloatValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
