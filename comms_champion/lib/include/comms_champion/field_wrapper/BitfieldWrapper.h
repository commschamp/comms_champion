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
#include <vector>

#include "comms/field/Bitfield.h"
#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class CC_API BitfieldWrapper : public FieldWrapper
{
    using Base = FieldWrapper;
public:
    typedef std::vector<FieldWrapperPtr> Members;

    typedef unsigned long long UnderlyingType;
    typedef std::unique_ptr<BitfieldWrapper> Ptr;

    BitfieldWrapper();
    BitfieldWrapper(const BitfieldWrapper&) = delete;
    BitfieldWrapper& operator=(const BitfieldWrapper&) = delete;

    virtual ~BitfieldWrapper() noexcept;
    
    Members& getMembers();

    const Members& getMembers() const;

    void setMembers(Members&& members);

    Ptr clone();

protected:
    virtual Ptr cloneImpl() = 0;

    void dispatchImpl(FieldWrapperHandler& handler);

private:
    Members m_members;
};

template <typename TField>
class BitfieldWrapperT : public FieldWrapperT<BitfieldWrapper, TField>
{
    using Base = FieldWrapperT<BitfieldWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isBitfield<Field>(), "Must be of Bitfield field type");

    using UnderlyingType = typename Base::UnderlyingType;
public:
    typedef typename Base::Ptr Ptr;

    explicit BitfieldWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    BitfieldWrapperT(const BitfieldWrapperT&) = default;
    BitfieldWrapperT(BitfieldWrapperT&&) = default;
    virtual ~BitfieldWrapperT() noexcept = default;

    BitfieldWrapperT& operator=(const BitfieldWrapperT&) = delete;

protected:
    virtual Ptr cloneImpl() override
    {
        return Ptr(new BitfieldWrapperT<TField>(Base::field()));
    }

};

using BitfieldWrapperPtr = BitfieldWrapper::Ptr;

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
