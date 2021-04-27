//
// Copyright 2020 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "comms/util/type_traits.h"

namespace comms
{

namespace details
{

enum class DispatchMsgTypeEnum
{
    Multiple,
    Single,
    None
};

template <std::size_t TCount>
using DispatchMsgHelperIntType =
    typename comms::util::Conditional<
        TCount == 0U
    >::template Type<
        std::integral_constant<DispatchMsgTypeEnum, DispatchMsgTypeEnum::None>,
        typename comms::util::Conditional<
            TCount == 1U
        >::template Type<
            std::integral_constant<DispatchMsgTypeEnum, DispatchMsgTypeEnum::Single>,
            std::integral_constant<DispatchMsgTypeEnum, DispatchMsgTypeEnum::Multiple>
        >
    >;
    

} // namespace details

} // namespace comms
