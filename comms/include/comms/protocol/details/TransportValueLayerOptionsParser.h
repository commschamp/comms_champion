//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/options.h"

namespace comms
{

namespace protocol
{

namespace details
{


template <typename... TOptions>
class TransportValueLayerOptionsParser;

template <>
class TransportValueLayerOptionsParser<>
{
public:
    static const bool HasPseudoValue = false;
};

template <typename... TOptions>
class TransportValueLayerOptionsParser<comms::option::def::PseudoValue, TOptions...> :
        public TransportValueLayerOptionsParser<TOptions...>
{
public:
    static const bool HasPseudoValue = true;
};

template <typename... TOptions>
class TransportValueLayerOptionsParser<
    comms::option::app::EmptyOption,
    TOptions...> : public TransportValueLayerOptionsParser<TOptions...>
{
};

template <typename... TBundledOptions, typename... TOptions>
class TransportValueLayerOptionsParser<
    std::tuple<TBundledOptions...>,
    TOptions...> : public TransportValueLayerOptionsParser<TBundledOptions..., TOptions...>
{
};

} // namespace details

} // namespace protocol

} // namespace comms
