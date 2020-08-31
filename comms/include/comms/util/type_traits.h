//
// Copyright 2013 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Replacement to some types from standard type_traits

#pragma once

#include "details/type_traits.h"

namespace comms
{

namespace util
{

/// @brief Replacement to std::conditional_t
template <bool TCond, typename TTrue, typename TFalse>
using ConditionalT = typename details::Conditional<TCond>::template Type<TTrue, TFalse>;

} // namespace util

} // namespace comms
