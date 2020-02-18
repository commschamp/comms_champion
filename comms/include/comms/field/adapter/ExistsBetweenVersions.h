//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
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

#include <cstdint>
#include <type_traits>
#include <limits>
#include <algorithm>
#include "comms/Assert.h"
#include "comms/field/OptionalMode.h"

namespace comms
{

namespace field
{

namespace adapter
{

template <std::uintmax_t TFrom, std::uintmax_t TUntil, typename TBase>
class ExistsBetweenVersions : public TBase
{
    using BaseImpl = TBase;
    static_assert(TFrom <= TUntil, "Invalid parameters");
public:

    using ValueType = typename BaseImpl::ValueType;
    using VersionType = typename BaseImpl::VersionType;

    ExistsBetweenVersions() = default;

    explicit ExistsBetweenVersions(const ValueType& val)
      : BaseImpl(val)
    {
    }

    explicit ExistsBetweenVersions(ValueType&& val)
      : BaseImpl(std::move(val))
    {
    }

    ExistsBetweenVersions(const ExistsBetweenVersions&) = default;
    ExistsBetweenVersions(ExistsBetweenVersions&&) = default;
    ExistsBetweenVersions& operator=(const ExistsBetweenVersions&) = default;
    ExistsBetweenVersions& operator=(ExistsBetweenVersions&&) = default;

    static constexpr bool isVersionDependent()
    {
        return true;
    }

    bool setVersion(VersionType version)
    {
        bool updated = BaseImpl::setVersion(version);
        auto mode = comms::field::OptionalMode::Missing;
        if (aboveFrom(version) && belowUntil(version)) {
            mode = comms::field::OptionalMode::Exists;
        }

        if (mode == BaseImpl::getMode()) {
            return updated;
        }

        BaseImpl::setMode(mode);
        return true;
    }

private:
    struct AlwaysTrueTag {};
    struct CompareTag {};

    static bool aboveFrom(VersionType version)
    {
        using Tag =
            typename std::conditional<
                TFrom == 0,
                AlwaysTrueTag,
                CompareTag
            >::type;
        return aboveFrom(version, Tag());
    }

    static constexpr bool aboveFrom(VersionType, AlwaysTrueTag)
    {
        return true;
    }

    static bool aboveFrom(VersionType version, CompareTag)
    {
        static const VersionType MinVersion =
            static_cast<VersionType>(
                std::min(
                    static_cast<decltype(TFrom)>(std::numeric_limits<VersionType>::max()),
                    TFrom));

        return MinVersion <= version;
    }

    static bool belowUntil(VersionType version)
    {
        using Tag =
            typename std::conditional<
                static_cast<decltype(TUntil)>(std::numeric_limits<VersionType>::max()) <= TUntil,
                AlwaysTrueTag,
                CompareTag
            >::type;
        return belowUntil(version, Tag());
    }

    static constexpr bool belowUntil(VersionType, AlwaysTrueTag)
    {
        return true;
    }

    static bool belowUntil(VersionType version, CompareTag)
    {
        static const VersionType MaxVersion =
            static_cast<VersionType>(
                std::min(
                    static_cast<decltype(TUntil)>(std::numeric_limits<VersionType>::max()),
                    TUntil));

        return version <= MaxVersion;
    }
};

}  // namespace adapter

}  // namespace field

}  // namespace comms

