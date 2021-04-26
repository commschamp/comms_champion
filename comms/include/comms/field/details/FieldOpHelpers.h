//
// Copyright 2015 - 2021 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <type_traits>
#include <limits>

#include "comms/CompileControl.h"
#include "comms/ErrorStatus.h"

COMMS_MSVC_WARNING_PUSH
COMMS_MSVC_WARNING_DISABLE(4100) // Disable warning about unreferenced parameters

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
struct FieldMinLengthSumCalcHelper
{
    template <typename TField>
    constexpr std::size_t operator()(std::size_t sum) const
    {
        return sum + TField::minLength();
    }
};

template <typename...>
struct FieldMaxLengthSumCalcHelper
{
    template <typename TField>
    constexpr std::size_t operator()(std::size_t sum) const
    {
        return sum + TField::maxLength();
    }
};

template <bool THasBitLengthLimit>
struct FieldBitLengthRetrieveHelper
{
    template <typename TField>
    using Type = 
        std::integral_constant<
            std::size_t,
            TField::ParsedOptions::FixedBitLength
        >;
};

template <>
struct FieldBitLengthRetrieveHelper<false>
{
    template <typename TField>
    using Type = 
        std::integral_constant<
            std::size_t,
            TField::maxLength() * std::numeric_limits<std::uint8_t>::digits
        >;
};


template <typename...>
struct FieldTotalBitLengthSumCalcHelper
{
    template <typename TField>
    constexpr std::size_t operator()(std::size_t sum) const
    {
        return sum + FieldBitLengthRetrieveHelper<TField::ParsedOptions::HasFixedBitLengthLimit>::template Type<TField>::value;
    }
};

template <typename...>
struct FieldLengthSumCalcHelper
{
    template <typename TField>
    constexpr std::size_t operator()(std::size_t sum, const TField& field) const
    {
        return sum + field.length();
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

template<typename...>
struct FieldNonDefaultRefreshCheckHelper
{
    template <typename TField>
    constexpr bool operator()() const
    {
        return TField::hasNonDefaultRefresh();
    }

    template <typename TField>
    constexpr bool operator()(bool soFar) const
    {
        return TField::hasNonDefaultRefresh() || soFar;
    }
};

template<typename...>
struct FieldValidCheckHelper
{
    template <typename TField>
    constexpr bool operator()(bool soFar, const TField& field) const
    {
        return soFar && field.valid();
    }
};

template <typename...>
struct FieldRefreshHelper
{
    template <typename TField>
    bool operator()(bool soFar, TField& field) const
    {
        return field.refresh() || soFar;
    }
};

template <typename TIter>
class FieldReadHelper
{
public:
    FieldReadHelper(ErrorStatus& es, TIter& iter, std::size_t& len)
      : es_(es),
        iter_(iter),
        len_(len)
    {
    }

    template <typename TField>
    void operator()(TField& field)
    {
        if (es_ != comms::ErrorStatus::Success) {
            return;
        }

        auto fromIter = iter_;
        es_ = field.read(iter_, len_);
        if (es_ == comms::ErrorStatus::Success) {
            len_ -= static_cast<std::size_t>(std::distance(fromIter, iter_));
        }
    }


private:
    ErrorStatus& es_;
    TIter& iter_;
    std::size_t& len_;
};

template <typename TIter>
class FieldReadNoStatusHelper
{
public:
    FieldReadNoStatusHelper(TIter& iter)
      : iter_(iter)
    {
    }

    template <typename TField>
    void operator()(TField& field)
    {
        field.readNoStatus(iter_);
    }

private:
    TIter& iter_;
};

template <typename TIter>
class FieldWriteHelper
{
public:
    FieldWriteHelper(ErrorStatus& es, TIter& iter, std::size_t len)
      : es_(es),
        iter_(iter),
        len_(len)
    {
    }

    template <typename TField>
    void operator()(const TField& field)
    {
        if (es_ != comms::ErrorStatus::Success) {
            return;
        }

        es_ = field.write(iter_, len_);
        if (es_ == comms::ErrorStatus::Success) {
            len_ -= field.length();
        }
    }

private:
    ErrorStatus& es_;
    TIter& iter_;
    std::size_t len_;
};

template <typename TIter>
class FieldWriteNoStatusHelper
{
public:
    FieldWriteNoStatusHelper(TIter& iter)
      : iter_(iter)
    {
    }

    template <typename TField>
    void operator()(const TField& field)
    {
        field.writeNoStatus(iter_);
    }

private:
    TIter& iter_;
};

template <typename...>
struct FieldReadNoStatusDetectHelper
{
    template <typename TField>
    constexpr bool operator()() const
    {
        return TField::hasReadNoStatus();
    }    

    template <typename TField>
    constexpr bool operator()(bool soFar) const
    {
        return TField::hasReadNoStatus() && soFar;
    }
};

template<typename...>
struct FieldWriteNoStatusDetectHelper
{
    template <typename TField>
    constexpr bool operator()() const
    {
        return TField::hasWriteNoStatus();
    }

    template <typename TField>
    constexpr bool operator()(bool soFar) const
    {
        return TField::hasWriteNoStatus() && soFar;
    }
};

template <typename...>
struct FieldCanWriteCheckHelper
{
    template <typename TField>
    constexpr bool operator()(bool soFar, const TField& field) const
    {
        return soFar && field.canWrite();
    }
};

template <typename TVersionType>
struct FieldVersionUpdateHelper
{
    FieldVersionUpdateHelper(TVersionType version) : version_(version) {}

    template <typename TField>
    bool operator()(bool updated, TField& field) const
    {
        using FieldVersionType = typename std::decay<decltype(field)>::type::VersionType;
        return field.setVersion(static_cast<FieldVersionType>(version_)) || updated;
    }

private:
    const TVersionType version_ = static_cast<TVersionType>(0);
};
    
} // namespace details

} // namespace field

} // namespace comms

COMMS_MSVC_WARNING_POP