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

#include "comms/field/Bitfield.h"
#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class BitfieldWrapper : public FieldWrapper
{
    using Base = FieldWrapper;
public:
    using Base::FieldWrapper;
protected:
    typedef unsigned long long UnderlyingType;
};

template <typename TField>
class BitfieldWrapperT : public FieldWrapperT<BitfieldWrapper, TField>
{
    using Base = FieldWrapperT<BitfieldWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isBitfield<Field>(), "Must be of Bitfield field type");

    using UnderlyingType = typename Base::UnderlyingType;
public:
    BitfieldWrapperT(Field& field)
      : Base(field)
    {
    }

    BitfieldWrapperT(const BitfieldWrapperT&) = default;
    BitfieldWrapperT(BitfieldWrapperT&&) = default;
    virtual ~BitfieldWrapperT() = default;

    BitfieldWrapperT& operator=(const BitfieldWrapperT&) = delete;

};

using BitfieldWrapperPtr = std::unique_ptr<BitfieldWrapper>;

template <typename TField>
BitfieldWrapperPtr
makeBitfieldWrapper(TField& field)
{
    return
        BitfieldWrapperPtr(
            new BitfieldWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
