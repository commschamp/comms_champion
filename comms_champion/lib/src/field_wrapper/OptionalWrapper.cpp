//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
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

#include "comms_champion/field_wrapper/OptionalWrapper.h"

#include "comms_champion/field_wrapper/FieldWrapperHandler.h"

namespace comms_champion
{

namespace field_wrapper
{

OptionalWrapper::OptionalWrapper() = default;
OptionalWrapper::~OptionalWrapper() = default;

OptionalWrapper::Mode OptionalWrapper::getMode() const
{
    return getModeImpl();
}

void OptionalWrapper::setMode(Mode mode) {
    setModeImpl(mode);
}

bool OptionalWrapper::hasFieldWrapper() const
{
    return static_cast<bool>(m_fieldWrapper);
}

FieldWrapper& OptionalWrapper::getFieldWrapper()
{
    assert(hasFieldWrapper());
    return *m_fieldWrapper;
}

const FieldWrapper& OptionalWrapper::getFieldWrapper() const
{
    assert(hasFieldWrapper());
    return *m_fieldWrapper;
}

void OptionalWrapper::setFieldWrapper(FieldWrapperPtr fieldWrapper)
{
    m_fieldWrapper = std::move(fieldWrapper);
}

OptionalWrapper::Ptr OptionalWrapper::clone()
{
    auto ptr = cloneImpl();
    ptr->setFieldWrapper(m_fieldWrapper->upClone());
    return std::move(ptr);
}

void OptionalWrapper::dispatchImpl(FieldWrapperHandler& handler)
{
    handler.handle(*this);
}

}  // namespace field_wrapper

}  // namespace comms_champion


