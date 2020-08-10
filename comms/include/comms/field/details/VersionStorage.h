//
// Copyright 2019 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace comms
{

namespace field
{

namespace details
{

template <typename TVersionType, bool TVersionDependent>
struct VersionStorage;

template <typename TVersionType>
struct VersionStorage<TVersionType, true>
{
    TVersionType getVersion() const
    {
        return version_;
    }
protected:
    TVersionType version_ = TVersionType();
};

template <typename TVersionType>
struct VersionStorage<TVersionType, false>
{
};

} // namespace details

} // namespace field

} // namespace comms
