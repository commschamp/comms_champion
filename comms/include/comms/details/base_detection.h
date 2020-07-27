//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/CompileControl.h"

#if (!COMMS_IS_CPP14)

#if COMMS_IS_CLANG
#define COMMS_MUST_DEFINE_BASE
#endif

#if !defined(COMMS_MUST_DEFINE_BASE) && COMMS_IS_GCC
#if __GNUC__ < 5
#define COMMS_MUST_DEFINE_BASE
#endif // #if __GNUC__ < 5
#endif // #if !defined(COMMS_MUST_DEFINE_BASE) && defined(__GNUC__)

#endif // #if (!COMMS_IS_CPP14)

