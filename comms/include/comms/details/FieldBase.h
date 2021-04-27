//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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


