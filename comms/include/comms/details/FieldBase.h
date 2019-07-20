//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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

#include "comms/traits.h"
#include "comms/options.h"

namespace comms
{

namespace details
{

template <typename... TOptions>
class FieldBase;

template <>
class FieldBase<>
{
protected:
    // Use big endian by default
    using Endian = comms::traits::endian::Big;

    // Use unsigned type by default for versioning
    using VersionType = unsigned;
};

template <typename TEndian, typename... TOptions>
class FieldBase<comms::option::def::Endian<TEndian>, TOptions...> : public FieldBase<TOptions...>
{
protected:
    using Endian = TEndian;
};

template <typename T, typename... TOptions>
class FieldBase<comms::option::def::VersionType<T>, TOptions...> : public FieldBase<TOptions...>
{
protected:
    using VersionType = T;
};

template <typename... TOptions>
class FieldBase<comms::option::app::EmptyOption, TOptions...> : public FieldBase<TOptions...>
{
};

template <typename... TTuple, typename... TOptions>
class FieldBase<std::tuple<TTuple...>, TOptions...> : public FieldBase<TTuple..., TOptions...>
{
};


}  // namespace details

}  // namespace comms


