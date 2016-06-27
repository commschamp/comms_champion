//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "adapter/CustomValueReader.h"
#include "adapter/SerOffset.h"
#include "adapter/FixedLength.h"
#include "adapter/FixedBitLength.h"
#include "adapter/VarLength.h"
#include "adapter/SequenceSizeForcing.h"
#include "adapter/SequenceFixedSize.h"
#include "adapter/SequenceSizeFieldPrefix.h"
#include "adapter/SequenceTrailingFieldSuffix.h"
#include "adapter/SequenceTerminationFieldSuffix.h"
#include "adapter/DefaultValueInitialiser.h"
#include "adapter/CustomValidator.h"
#include "adapter/FailOnInvalid.h"
#include "adapter/IgnoreInvalid.h"
#include "adapter/Scaling.h"
