//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class CC_API UnknownValueWrapper : public FieldWrapper
{
public:
    typedef std::unique_ptr<UnknownValueWrapper> Ptr;

    UnknownValueWrapper();
    virtual ~UnknownValueWrapper() noexcept;

    Ptr clone();

protected:
    virtual Ptr cloneImpl() = 0;

    void dispatchImpl(FieldWrapperHandler& handler);
};

template <typename TField>
class UnknownValueWrapperT : public FieldWrapperT<UnknownValueWrapper, TField>
{
    using Base = FieldWrapperT<UnknownValueWrapper, TField>;
    using Field = TField;
    using SerialisedSeq = typename Base::SerialisedSeq;

public:
    typedef typename Base::Ptr Ptr;

    explicit UnknownValueWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

    UnknownValueWrapperT(const UnknownValueWrapperT&) = default;
    UnknownValueWrapperT(UnknownValueWrapperT&&) = default;
    virtual ~UnknownValueWrapperT() noexcept = default;

    UnknownValueWrapperT& operator=(const UnknownValueWrapperT&) = delete;

protected:
    virtual Ptr cloneImpl() override
    {
        return Ptr(new UnknownValueWrapperT<TField>(Base::field()));
    }
};

using UnknownValueWrapperPtr = UnknownValueWrapper::Ptr;

template <typename TField>
UnknownValueWrapperPtr
makeUnknownValueWrapper(TField& field)
{
    return
        UnknownValueWrapperPtr(
            new UnknownValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion
