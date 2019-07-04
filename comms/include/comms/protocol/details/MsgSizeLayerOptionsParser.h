//
// Copyright 2019 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <tuple>
#include "comms/options.h"

namespace comms
{

namespace protocol
{

namespace details
{


template <typename... TOptions>
class MsgSizeLayerOptionsParser;

template <>
class MsgSizeLayerOptionsParser<>
{
public:
    static const bool HasExtendingClass = false;
};

template <typename T, typename... TOptions>
class MsgSizeLayerOptionsParser<comms::option::def::ExtendingClass<T>, TOptions...> :
        public MsgSizeLayerOptionsParser<TOptions...>
{
public:
    static const bool HasExtendingClass = true;
    using ExtendingClass = T;
};

template <typename... TOptions>
class MsgSizeLayerOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public MsgSizeLayerOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class MsgSizeLayerOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public MsgSizeLayerOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace protocol

} // namespace comms
