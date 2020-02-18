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
#include <cmath>
#include <limits>

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

    FloatValueWrapper();
    virtual ~FloatValueWrapper() noexcept;

    Ptr clone();
    bool isNan() const;
    void setNan();
    bool isInf() const;
    void setInf();
    bool isMinusInf() const;
    void setMinusInf();
    double getEpsilon() const;

protected:
    virtual Ptr cloneImpl() = 0;
    virtual bool isNanImpl() const = 0;
    virtual void setNanImpl() = 0;
    virtual bool isInfImpl() const = 0;
    virtual void setInfImpl() = 0;
    virtual bool isMinusInfImpl() const = 0;
    virtual void setMinusInfImpl() = 0;
    virtual double getEpsilonImpl() const = 0;

    virtual void dispatchImpl(FieldWrapperHandler& handler) override;
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
    virtual ~FloatValueWrapperT() noexcept = default;

    FloatValueWrapperT& operator=(const FloatValueWrapperT&) = delete;

protected:
    virtual Ptr cloneImpl() override
    {
        return Ptr(new FloatValueWrapperT<TField>(Base::field()));
    }

    virtual bool isNanImpl() const override
    {
        return std::isnan(Base::field().value());
    }

    virtual void setNanImpl() override
    {
        Base::field().value() = std::numeric_limits<typename TField::ValueType>::quiet_NaN();
    }

    virtual bool isInfImpl() const override
    {
        return std::isinf(Base::field().value()) && (0 < Base::field().value());
    }

    virtual void setInfImpl() override
    {
        Base::field().value() = std::numeric_limits<typename TField::ValueType>::infinity();
    }

    virtual bool isMinusInfImpl() const override
    {
        return std::isinf(Base::field().value()) && (Base::field().value() < 0);
    }

    virtual void setMinusInfImpl() override
    {
        Base::field().value() = -std::numeric_limits<typename TField::ValueType>::infinity();
    }

    virtual double getEpsilonImpl() const override
    {
        return static_cast<double>(std::numeric_limits<typename TField::ValueType>::epsilon());
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
