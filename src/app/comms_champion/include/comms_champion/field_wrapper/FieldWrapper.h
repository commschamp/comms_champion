//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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
#include <cstddef>
#include <cassert>
#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"

namespace comms_champion
{

namespace field_wrapper
{

class FieldWrapper
{
public:
    typedef std::vector<std::uint8_t> SerialisedSeq;

    virtual ~FieldWrapper() {};

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

    SerialisedSeq getSerialisedValue() const
    {
        return getSerialisedValueImpl();
    }

    bool setSerialisedValue(const SerialisedSeq& value)
    {
        return setSerialisedValueImpl(value);
    }

    QString getSerialisedString() const;

    bool setSerialisedString(const QString& str);

protected:
    virtual std::size_t lengthImpl() const = 0;
    virtual bool validImpl() const = 0;
    virtual SerialisedSeq getSerialisedValueImpl() const = 0;
    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) = 0;
};

template <typename TBase, typename TField>
class FieldWrapperT : public TBase
{
    using Base = TBase;
    using Field = TField;
public:
    typedef typename Base::SerialisedSeq SerialisedSeq;
    virtual ~FieldWrapperT() = default;

protected:

    explicit FieldWrapperT(Field& fieldRef)
      : m_field(fieldRef)
    {
    }

    virtual std::size_t lengthImpl() const override
    {
        return m_field.length();
    }

    virtual bool validImpl() const override
    {
        return m_field.valid();
    }

    Field& field()
    {
        return m_field;
    }

    const Field& field() const
    {
        return m_field;
    }

    virtual SerialisedSeq getSerialisedValueImpl() const override
    {
        SerialisedSeq seq;
        seq.reserve(m_field.length());
        auto iter = std::back_inserter(seq);
        auto es = m_field.write(iter, seq.max_size());
        static_cast<void>(es);
        assert(es == comms::ErrorStatus::Success);
        assert(seq.size() == m_field.length());
        return seq;
    }

    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        if (value.empty()) {
            return false;
        }

        auto iter = &value[0];
        auto es = m_field.read(iter, value.size());
        return es == comms::ErrorStatus::Success;
    }

private:
    Field& m_field;
};

typedef std::unique_ptr<FieldWrapper> FieldWrapperPtr;

}  // namespace field_wrapper

}  // namespace comms_champion
