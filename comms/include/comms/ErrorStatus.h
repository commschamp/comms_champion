//
// Copyright 2014 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file comms/ErrorStatus.h
/// This file contain definition of error statuses used by comms module.

#pragma once

namespace comms
{

/// @brief Error statuses reported by the Communication module.
enum class ErrorStatus {
    Success, ///< Used to indicate successful outcome of the operation.
    UpdateRequired, ///< Used to indicate that write operation wasn't complete,
                    /// call to update(...) is required.
    NotEnoughData, ///< Used to indicate that stream buffer didn't contain
                   /// enough data to complete read operation.
    ProtocolError, ///< Used to indicate that any of the used protocol layers
                   /// encountered an error while processing the data.
    BufferOverflow, ///< Used to indicate that stream buffer was overflowed
                    /// when attempting to write data.
    InvalidMsgId, ///< Used to indicate that received message has unknown id
    InvalidMsgData, ///<Used to indicate that a message has invalid data.
    MsgAllocFailure, ///<Used to indicate that message allocation has failed.
    NotSupported, ///< The operation is not supported.
    NumOfErrorStatuses ///< Number of supported error statuses, must be last.
};

}  // namespace comms

