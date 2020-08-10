//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
#include "comms/field/adapter/CustomReadWrap.h"
#include "comms/field/adapter/CustomWriteWrap.h"
