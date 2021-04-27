//
// Copyright 2017 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Contains definition of various tag classes

#pragma once

#include "comms/details/tag.h"

namespace comms
{

namespace field
{

namespace tag
{

using RawArrayList = comms::details::tag::Tag1<>;
using ArrayList = comms::details::tag::Tag2<>;
using Bitfield = comms::details::tag::Tag3<>;
using Bitmask = comms::details::tag::Tag4<>;
using Bundle = comms::details::tag::Tag5<>;
using Enum = comms::details::tag::Tag6<>;
using Float = comms::details::tag::Tag7<>;
using Int = comms::details::tag::Tag8<>;
using NoValue = comms::details::tag::Tag9<>;
using Optional = comms::details::tag::Tag10<>;
using String = comms::details::tag::Tag11<>;
using Variant = comms::details::tag::Tag12<>;

}  // namespace tag

}  // namespace field

}  // namespace comms


