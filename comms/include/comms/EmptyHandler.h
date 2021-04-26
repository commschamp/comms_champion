//
// Copyright 2014 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// Contains definition of @ref comms::EmptyHandler class

#pragma once

namespace comms
{

/// @brief Empty message handler, does nothing.
/// @details May be used in @ref comms::option::app::Handler option to force
///     existence of "comms::Message::dispatch()" member function.
class EmptyHandler
{
public:
    template <typename TMessage>
    void handle(TMessage&) {}
};

}  // namespace comms


