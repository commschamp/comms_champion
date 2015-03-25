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
#include <limits>

#include "comms/field/BitmaskValue.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class BitmaskValueWrapper : public NumericValueWrapper<unsigned long long>
{
public:
    bool bitValue(unsigned idx) const
    {
        return bitValueImpl(idx);
    }

    void setBitValue(unsigned idx, bool value)
    {
        setBitValueImpl(idx, value);
    }

    unsigned bitIdxLimit() const
    {
        return bitIdxLimitImpl();
    }

protected:
    virtual bool bitValueImpl(unsigned idx) const = 0;
    virtual void setBitValueImpl(unsigned idx, bool value) = 0;
    virtual unsigned bitIdxLimitImpl() const = 0;
};

template <typename TField>
class BitmaskValueWrapperT : public NumericValueWrapperT<BitmaskValueWrapper, TField>
{
    using Base = NumericValueWrapperT<BitmaskValueWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isBitmaskValue<Field>(), "Must be of BitmaskValueField type");

    using ValueType = typename Field::ValueType;
    using MaskType = typename Base::UnderlyingType;
    static_assert(sizeof(ValueType) <= sizeof(MaskType), "This wrapper cannot handle provided field.");

public:
    BitmaskValueWrapperT(Field& field)
      : Base(field)
    {
    }

    BitmaskValueWrapperT(const BitmaskValueWrapperT&) = default;
    BitmaskValueWrapperT(BitmaskValueWrapperT&&) = default;
    virtual ~BitmaskValueWrapperT() = default;

    BitmaskValueWrapperT& operator=(const BitmaskValueWrapperT&) = delete;

protected:
    virtual bool bitValueImpl(unsigned idx) const override
    {
        return Base::field().getBitValue(idx);
    }

    virtual void setBitValueImpl(unsigned idx, bool value) override
    {
        Base::field().setBitValue(idx, value);
    }

    virtual unsigned bitIdxLimitImpl() const override
    {
        return std::numeric_limits<ValueType>::digits;
    }
};

using BitmaskValueWrapperPtr = std::unique_ptr<BitmaskValueWrapper>;

template <typename TField>
BitmaskValueWrapperPtr
makeBitmaskValueWrapper(TField& field)
{
    return
        BitmaskValueWrapperPtr(
            new BitmaskValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
