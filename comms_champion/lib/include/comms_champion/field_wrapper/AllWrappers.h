//
// Copyright 2016 - 2017 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <tuple>

#include "IntValueWrapper.h"
#include "UnsignedLongValueWrapper.h"
#include "BitmaskValueWrapper.h"
#include "EnumValueWrapper.h"
#include "StringWrapper.h"
#include "BitfieldWrapper.h"
#include "OptionalWrapper.h"
#include "BundleWrapper.h"
#include "ArrayListRawDataWrapper.h"
#include "ArrayListWrapper.h"
#include "FloatValueWrapper.h"
#include "VariantWrapper.h"
#include "UnknownValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

typedef std::tuple<
    IntValueWrapper,
    UnsignedLongValueWrapper,
    BitmaskValueWrapper,
    EnumValueWrapper,
    StringWrapper,
    BitfieldWrapper,
    OptionalWrapper,
    BundleWrapper,
    ArrayListRawDataWrapper,
    ArrayListWrapper,
    FloatValueWrapper,
    VariantWrapper,
    UnknownValueWrapper
> AllWrappers;

}  // namespace field_wrapper

}  // namespace comms_champion


