//
// Copyright 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace comms
{

namespace details
{

namespace tag
{

template<typename...>
struct Tag1 {};

template<typename...>
struct Tag2 {};
    
} // namespace tag
    
} // namespace details

} // namespace comms
