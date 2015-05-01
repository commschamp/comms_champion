//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

namespace field
{

namespace details
{

template <typename... TOptions>
struct OptionsParser;

template <>
struct OptionsParser<>
{
    static const bool HasLengthLimit = false;
};

template <std::size_t TLen, typename... TOptions>
class OptionsParser<
    comms::option::FixedLength<TLen>,
    TOptions...> : public OptionsParser<TOptions...>
{
    typedef comms::option::FixedLength<TLen> Option;
public:
    static const bool HasLengthLimit = true;
    static const std::size_t Length = Option::Value;
};

}  // namespace details

}  // namespace field

}  // namespace comms


