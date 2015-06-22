//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include <memory>

#include "comms/field/Optional.h"

namespace comms_champion
{

namespace field_wrapper
{

class OptionalWrapper
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

protected:
    virtual Mode getModeImpl() const = 0;
    virtual void setModeImpl(Mode mode) = 0;
};

template <typename TField>
class OptionalWrapperT : public OptionalWrapper
{
    typedef OptionalWrapper Base;
public:

    typedef TField Field;
    OptionalWrapperT(Field& field)
      : m_field(field)
    {
    }

protected:
    virtual Mode getModeImpl() const override
    {
        return m_field.getMode();
    }

    virtual void setModeImpl(Mode mode) override
    {
        m_field.setMode(mode);
    }
private:

    Field& m_field;
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


