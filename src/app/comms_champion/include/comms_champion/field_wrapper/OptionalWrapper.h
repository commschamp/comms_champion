//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include <memory>
#include <cassert>

#include "comms/field/Optional.h"
#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class OptionalWrapper : public FieldWrapper
{
public:
    typedef comms::field::OptionalMode Mode;

    virtual ~OptionalWrapper() = default;

    Mode getMode() const
    {
        return getModeImpl();
    }

    void setMode(Mode mode) {
        setModeImpl(mode);
    }

    bool hasFieldWrapper() const
    {
        return static_cast<bool>(m_fieldWrapper);
    }

    FieldWrapper& getFieldWrapper()
    {
        assert(hasFieldWrapper());
        return *m_fieldWrapper;
    }

    const FieldWrapper& getFieldWrapper() const
    {
        assert(hasFieldWrapper());
        return *m_fieldWrapper;
    }

    void setFieldWrapper(FieldWrapperPtr fieldWrapper)
    {
        m_fieldWrapper = std::move(fieldWrapper);
    }

protected:
    virtual Mode getModeImpl() const = 0;
    virtual void setModeImpl(Mode mode) = 0;

private:
    FieldWrapperPtr m_fieldWrapper;
};

template <typename TField>
class OptionalWrapperT : public FieldWrapperT<OptionalWrapper, TField>
{
    typedef FieldWrapperT<OptionalWrapper, TField> Base;
public:

    typedef typename Base::Mode Mode;

    typedef TField Field;
    explicit OptionalWrapperT(Field& fieldRef)
      : Base(fieldRef)
    {
    }

protected:
    virtual Mode getModeImpl() const override
    {
        return Base::field().getMode();
    }

    virtual void setModeImpl(Mode mode) override
    {
        Base::field().setMode(mode);
    }
};

using OptionalWrapperPtr = std::unique_ptr<OptionalWrapper>;

template <typename TField>
OptionalWrapperPtr
makeOptionalWrapper(TField& field)
{
    return
        OptionalWrapperPtr(
            new OptionalWrapperT<TField>(field));
}


}  // namespace field_wrapper

}  // namespace comms_champion


