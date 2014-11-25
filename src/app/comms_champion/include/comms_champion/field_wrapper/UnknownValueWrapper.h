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

#include "comms/comms.h"

namespace comms_champion
{

namespace field_wrapper
{

class UnknownValueWrapper
{
public:
    typedef std::vector<std::uint8_t> SerializedType;

    virtual ~UnknownValueWrapper() {}

    SerializedType serialisedValue() const
    {
        return serialisedValueImpl();
    }

    bool setSerialisedValue(const SerializedType& value)
    {
        return setSerialisedValueImpl(value);
    }

    std::size_t length() const
    {
        return lengthImpl();
    }

    int width() const
    {
        return static_cast<int>(length()) * 2;
    }

    bool valid() const
    {
        return validImpl();
    }

private:
    virtual SerializedType serialisedValueImpl() const = 0;
    virtual bool setSerialisedValueImpl(const SerializedType& value) = 0;
    virtual std::size_t lengthImpl() const = 0;
    virtual bool validImpl() const = 0;
};

template <typename TField>
class UnknownValueWrapperT : public UnknownValueWrapper
{
    using Base = UnknownValueWrapper;
    using Field = TField;

public:
    UnknownValueWrapperT(Field& field)
      : m_field(field)
    {
    }

    UnknownValueWrapperT(const UnknownValueWrapperT&) = default;
    UnknownValueWrapperT(UnknownValueWrapperT&&) = default;
    virtual ~UnknownValueWrapperT() = default;

    UnknownValueWrapperT& operator=(const UnknownValueWrapperT&) = delete;

protected:
    virtual SerializedType serialisedValueImpl() const override
    {
        SerializedType value;
        value.reserve(m_field.length());
        auto iter = std::back_inserter(value);
        m_field.write(iter, value.max_size());
        return value;
    }

    virtual bool setSerialisedValueImpl(const SerializedType& value) override
    {
        return setSerialisedValueImplInternal(value, UpdateTag());
    }

    virtual std::size_t lengthImpl() const {
        return m_field.length();
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

    bool setSerialisedValueImplInternal(const SerializedType& value, Writable)
    {
        if (value.empty()) {
            return false;
        }

        const std::uint8_t* iter = &value[0];
        auto result = m_field.read(iter, value.size());
        return result == comms::ErrorStatus::Success;
    }

    bool setSerialisedValueImplInternal(const SerializedType& value, ReadOnly)
    {
        static_cast<void>(value);
        assert(!"Attempt to update readonly field");
        return false;
    }

    Field& m_field;
};

using UnknownValueWrapperPtr = std::unique_ptr<UnknownValueWrapper>;

template <typename TField>
UnknownValueWrapperPtr
makeUnknownValueWrapper(TField& field)
{
    return
        std::unique_ptr<UnknownValueWrapper>(
            new UnknownValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
