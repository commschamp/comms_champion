//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/field/adapter/CustomValueReader.h"
#include "comms/field/adapter/SerOffset.h"
#include "comms/field/adapter/FixedLength.h"
#include "comms/field/adapter/FixedBitLength.h"
#include "comms/field/adapter/VarLength.h"
#include "comms/field/adapter/SequenceElemLengthForcing.h"
#include "comms/field/adapter/SequenceSizeForcing.h"
#include "comms/field/adapter/SequenceLengthForcing.h"
#include "comms/field/adapter/SequenceFixedSize.h"
#include "comms/field/adapter/SequenceSizeFieldPrefix.h"
#include "comms/field/adapter/SequenceSerLengthFieldPrefix.h"
#include "comms/field/adapter/SequenceElemSerLengthFieldPrefix.h"
#include "comms/field/adapter/SequenceElemFixedSerLengthFieldPrefix.h"
#include "comms/field/adapter/SequenceTrailingFieldSuffix.h"
#include "comms/field/adapter/SequenceTerminationFieldSuffix.h"
#include "comms/field/adapter/DefaultValueInitialiser.h"
#include "comms/field/adapter/NumValueMultiRangeValidator.h"
#include "comms/field/adapter/CustomValidator.h"
#include "comms/field/adapter/CustomRefreshWrap.h"
#include "comms/field/adapter/CustomRefresher.h"
#include "comms/field/adapter/FailOnInvalid.h"
#include "comms/field/adapter/IgnoreInvalid.h"
#include "comms/field/adapter/EmptySerialization.h"
#include "comms/field/adapter/ExistsBetweenVersions.h"
#include "comms/field/adapter/InvalidByDefault.h"
#include "comms/field/adapter/VersionStorage.h"
#include "comms/field/adapter/RemLengthMemberField.h"
