//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace comms
{

namespace details
{

template <bool THasMsgIdType>
struct MessageIdTypeRetriever;

template <>
struct MessageIdTypeRetriever<true>
{
    template <typename TOpt, typename TDefaultType>
    using Type = typename TOpt::MsgIdType;
};

template <>
struct MessageIdTypeRetriever<false>
{
    template <typename TOpt, typename TDefaultType>
    using Type = TDefaultType;
};

} // namespace details

} // namespace comms
