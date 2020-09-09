//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
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

template <typename...>
struct FieldVersionDependentCheckHelper
{
    template <typename TField>
    constexpr bool operator()() const
    {
        return TField::isVersionDependent();
    }

    template <typename TField>
    constexpr bool operator()(bool soFar) const
    {
        return TField::isVersionDependent() || soFar;
    }    
};

template <typename...>
struct FieldMaxLengthCalcHelper
{
    template <typename TField>
    constexpr std::size_t operator()(std::size_t val) const
    {
        return val >= TField::maxLength() ? val : TField::maxLength();
    }
};

template <typename...>
struct FieldHasWriteNoStatusHelper
{
    constexpr FieldHasWriteNoStatusHelper() = default;

    template <typename TField>
    constexpr bool operator()(bool soFar) const
    {
        return TField::hasWriteNoStatus() && soFar;
    }
};
    
} // namespace details

} // namespace field

    
} // namespace comms
