//
// Copyright 2014 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// This file provides all the definitions from comms::protocol namespace.

#pragma once

#include "protocol/MsgDataLayer.h"
#include "protocol/MsgIdLayer.h"
#include "protocol/MsgSizeLayer.h"
#include "protocol/SyncPrefixLayer.h"
#include "protocol/ChecksumLayer.h"
#include "protocol/ChecksumPrefixLayer.h"
#include "protocol/TransportValueLayer.h"

#include "protocol/checksum/BasicSum.h"
#include "protocol/checksum/BasicXor.h"
#include "protocol/checksum/Crc.h"
