//
// Copyright 2017 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <algorithm>
#include "comms/Assert.h"
#include "comms/field/OptionalMode.h"
#include "comms/util/type_traits.h"

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
            comms::util::ConditionalT<
                TFrom == 0,
                AlwaysTrueTag,
                CompareTag
            >;
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
            comms::util::ConditionalT<
                static_cast<decltype(TUntil)>(std::numeric_limits<VersionType>::max()) <= TUntil,
                AlwaysTrueTag,
                CompareTag
            >;
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

