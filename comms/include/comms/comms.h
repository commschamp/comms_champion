//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// Aggregates all the includes of the @b COMMS library interface

#pragma once

#include "comms/CompileControl.h"
#include "comms/fields.h"
#include "comms/protocols.h"
#include "comms/units.h"
#include "comms/version.h"
#include "comms/dispatch.h"
#include "comms/field_cast.h"
#include "comms/iterator.h"
#include "process.h"

#include "comms/Message.h"
#include "comms/EmptyHandler.h"
#include "comms/GenericHandler.h"
#include "comms/MessageBase.h"
#include "comms/MsgFactory.h"
#include "comms/MsgDispatcher.h"
#include "comms/GenericMessage.h"

#include "comms/util/detect.h"
#include "comms/util/assign.h"
