//
// Copyright 2019 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace comms
{

namespace field
{

namespace adapter
{

template <typename TBase>
class CustomRefreshWrap : public TBase
{
public:
    static constexpr bool hasNonDefaultRefresh()
    {
        return true;
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms




