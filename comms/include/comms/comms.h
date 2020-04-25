//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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
