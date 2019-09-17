//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/field_wrapper/FloatValueWrapper.h"

#include "comms_champion/field_wrapper/FieldWrapperHandler.h"

namespace comms_champion
{

namespace field_wrapper
{

FloatValueWrapper::FloatValueWrapper() {}

FloatValueWrapper::~FloatValueWrapper() noexcept = default;

FloatValueWrapper::Ptr FloatValueWrapper::clone()
{
    return cloneImpl();
}

bool FloatValueWrapper::isNan() const
{
    return isNanImpl();
}

void FloatValueWrapper::setNan()
{
    setNanImpl();
}

bool FloatValueWrapper::isInf() const
{
    return isInfImpl();
}

void FloatValueWrapper::setInf()
{
    setInfImpl();
}

bool FloatValueWrapper::isMinusInf() const
{
    return isMinusInfImpl();
}

void FloatValueWrapper::setMinusInf()
{
    setMinusInfImpl();
}

void FloatValueWrapper::dispatchImpl(FieldWrapperHandler& handler)
{
    handler.handle(*this);
}

double FloatValueWrapper::getEpsilon() const
{
    return getEpsilonImpl();
}

}  // namespace field_wrapper

}  // namespace comms_champion


