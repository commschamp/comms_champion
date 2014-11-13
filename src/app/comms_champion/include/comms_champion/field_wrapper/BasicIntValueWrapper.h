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

#include "comms/field/BasicIntValue.h"

namespace comms_champion
{

namespace field_wrapper
{

class BasicIntValueWrapper
{
public:
    virtual ~BasicIntValueWrapper() {}

    int value() const
    {
        return valueImpl();
    }

    void setValue(int value)
    {
        setValueImpl(value);
    }

    std::size_t length() const
    {
        return lengthImpl();
    }

private:
    virtual int valueImpl() const = 0;
    virtual void setValueImpl(int value) = 0;
    virtual std::size_t lengthImpl() const = 0;
};

template <typename TField>
class BasicIntValueWrapperT : public BasicIntValueWrapper
{
    using Base = BasicIntValueWrapper;
    using Field = TField;
    using ValueType = typename Field::ValueType;

    static_assert(sizeof(ValueType) <= sizeof(int), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(int)),
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
    virtual int valueImpl() const override
    {
        return static_cast<int>(m_field.getValue());
    }

    virtual void setValueImpl(int value) override
    {
        setValueImplInternal(value, UpdateTag());
    }

    virtual std::size_t lengthImpl() const {
        return m_field.length();
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

    void setValueImplInternal(int value, Writable)
    {
        m_field.setValue(static_cast<ValueType>(value));
    }

    void setValueImplInternal(int value, ReadOnly)
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
