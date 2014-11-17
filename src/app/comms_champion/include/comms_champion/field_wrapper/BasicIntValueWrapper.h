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
#include <memory>

#include "comms/field/BasicIntValue.h"

namespace comms_champion
{

namespace field_wrapper
{

class BasicIntValueWrapper
{
public:
    typedef int IntType;

    virtual ~BasicIntValueWrapper() {}

    IntType value() const
    {
        return valueImpl();
    }

    void setValue(IntType value)
    {
        setValueImpl(value);
    }

    IntType serialisedValue() const
    {
        return serialisedValueImpl();
    }

    void setSerialisedValue(IntType value)
    {
        setSerialisedValueImpl(value);
    }

    std::size_t length() const
    {
        return lengthImpl();
    }

    int width() const
    {
        return static_cast<int>(length()) * 2;
    }

    IntType minValue() const
    {
        return minValueImpl();
    }

    IntType maxValue() const
    {
        return maxValueImpl();
    }

    bool valid() const
    {
        return validImpl();
    }

private:
    virtual IntType valueImpl() const = 0;
    virtual IntType serialisedValueImpl() const = 0;
    virtual void setValueImpl(IntType value) = 0;
    virtual void setSerialisedValueImpl(IntType value) = 0;
    virtual std::size_t lengthImpl() const = 0;
    virtual IntType minValueImpl() const = 0;
    virtual IntType maxValueImpl() const = 0;
    virtual bool validImpl() const = 0;
};

template <typename TField>
class BasicIntValueWrapperT : public BasicIntValueWrapper
{
    using Base = BasicIntValueWrapper;
    using Field = TField;
    using ValueType = typename Field::ValueType;
    using IntType = typename Base::IntType;

    static_assert(sizeof(ValueType) <= sizeof(IntType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(IntType)),
        "This wrapper cannot handle provided field.");

public:
    BasicIntValueWrapperT(Field& field)
      : m_field(field)
    {
    }

    BasicIntValueWrapperT(const BasicIntValueWrapperT&) = default;
    BasicIntValueWrapperT(BasicIntValueWrapperT&&) = default;
    virtual ~BasicIntValueWrapperT() = default;

    BasicIntValueWrapperT& operator=(const BasicIntValueWrapperT&) = delete;

protected:
    virtual IntType valueImpl() const override
    {
        return static_cast<IntType>(m_field.getValue());
    }

    virtual IntType serialisedValueImpl() const override
    {
        return static_cast<IntType>(m_field.getSerialisedValue());
    }

    virtual void setValueImpl(IntType value) override
    {
        setValueImplInternal(value, UpdateTag());
    }

    virtual void setSerialisedValueImpl(IntType value) override
    {
        setSerialisedValueImplInternal(value, UpdateTag());
    }

    virtual std::size_t lengthImpl() const {
        return m_field.length();
    }

    virtual IntType minValueImpl() const override
    {
        return static_cast<IntType>(std::numeric_limits<ValueType>::min());
    }

    virtual IntType maxValueImpl() const override
    {
        return static_cast<IntType>(std::numeric_limits<ValueType>::max());
    }

    virtual bool validImpl() const override
    {
        return m_field.valid();
    }

private:
    struct Writable {};
    struct ReadOnly {};

    using UpdateTag =
        typename std::conditional<
            std::is_const<TField>::value,
            ReadOnly,
            Writable
        >::type;

    void setValueImplInternal(IntType value, Writable)
    {
        m_field.setValue(static_cast<ValueType>(value));
    }

    void setValueImplInternal(IntType value, ReadOnly)
    {
        static_cast<void>(value);
        assert(!"Attempt to update writable field");
    }

    void setSerialisedValueImplInternal(IntType value, Writable)
    {
        m_field.setSerialisedValue(static_cast<ValueType>(value));
    }

    void setSerialisedValueImplInternal(IntType value, ReadOnly)
    {
        static_cast<void>(value);
        assert(!"Attempt to update writable field");
    }


    Field& m_field;
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
