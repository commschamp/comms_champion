//
// Copyright 2017 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

/// @file comms/units.h
/// This file contains all the functions required for proper units conversion.

#pragma once

#include <ratio>
#include <type_traits>

#include "comms/traits.h"

namespace comms
{

namespace units
{

namespace details
{

template <typename TField, bool THasScalingRatio>
struct ScalingRatioRetriever
{
    using Type = std::ratio<1, 1>;
};

template <typename TField>
struct ScalingRatioRetriever<TField, true>
{
    using Type = typename TField::ParsedOptions::ScalingRatio;
};

template <typename TField>
using ScalingRatioOf =
    typename ScalingRatioRetriever<TField, TField::ParsedOptions::HasScalingRatio>::Type;

template <typename TField, typename TConvRatio>
using FullUnitsRatioOf =
    typename std::ratio_divide<
        typename std::ratio_multiply<
            ScalingRatioOf<TField>,
            typename TField::ParsedOptions::UnitsRatio
        >::type,
        TConvRatio
    >::type;

struct UnitsValueConverter
{
    template <typename TRet, typename TConvRatio, typename TField>
    static TRet getValue(const TField& field)
    {
        using Ratio = FullUnitsRatioOf<TField, TConvRatio>;
        using Tag = typename std::conditional<
            std::is_same<Ratio, std::ratio<1, 1> >::value,
            NoConversionTag,
            HasConversionTag
        >::type;

        return getValueInternal<TRet, Ratio>(field, Tag());
    }

    template <typename TConvRatio, typename TField, typename TVal>
    static void setValue(TField& field, TVal&& value)
    {
        using Ratio = FullUnitsRatioOf<TField, TConvRatio>;
        using Tag = typename std::conditional<
            std::is_same<Ratio, std::ratio<1, 1> >::value,
            NoConversionTag,
            HasConversionTag
        >::type;

        return setValueInternal<Ratio>(field, std::forward<TVal>(value), Tag());
    }

private:
    struct HasConversionTag {};
    struct NoConversionTag {};
    struct ConvertToFpTag {};
    struct ConvertToIntTag {};


    template <typename TRet, typename TRatio, typename TField>
    static TRet getValueInternal(const TField& field, NoConversionTag)
    {
        return static_cast<TRet>(field.value());
    }

    template <typename TRet, typename TRatio, typename TField>
    static TRet getValueInternal(const TField& field, HasConversionTag)
    {
        using Tag = typename std::conditional<
            std::is_floating_point<TRet>::value,
            ConvertToFpTag,
            ConvertToIntTag
        >::type;

        return getValueInternal<TRet, TRatio>(field, Tag());
    }

    template <typename TRet, typename TRatio, typename TField>
    static TRet getValueInternal(const TField& field, ConvertToFpTag)
    {
        static_assert(std::is_floating_point<TRet>::value,
            "TRet is expected to be floating point type");
        return static_cast<TRet>(field.value()) * (static_cast<TRet>(TRatio::num) / static_cast<TRet>(TRatio::den));
    }

    template <typename TRet, typename TRatio, typename TField>
    static TRet getValueInternal(const TField& field, ConvertToIntTag)
    {
        static_assert(std::is_integral<TRet>::value,
            "TRet is expected to be integral type");

        typedef typename std::conditional<
            std::is_signed<TRet>::value,
            std::intmax_t,
            std::uintmax_t
        >::type CastType;

        return
            static_cast<TRet>(
                (static_cast<CastType>(field.value()) * TRatio::num) / TRatio::den);
    }

    template <typename TRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& value, NoConversionTag)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        using ValueType = typename FieldType::ValueType;
        field.value() = static_cast<ValueType>(value);
    }

    template <typename TRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& value, HasConversionTag)
    {
        using Tag = typename std::conditional<
            std::is_floating_point<typename std::decay<decltype(value)>::type>::value,
            ConvertToFpTag,
            ConvertToIntTag
        >::type;

        setValueInternal<TRatio>(field, std::forward<TVal>(value), Tag());
    }

    template <typename TRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& value, ConvertToIntTag)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        using ValueType = typename FieldType::ValueType;
        using CastType = typename std::conditional<
            std::is_signed<typename std::decay<decltype(value)>::type>::value,
            std::intmax_t,
            std::uintmax_t
        >::type;

        field.value() =
            static_cast<ValueType>(
                (static_cast<CastType>(value) * TRatio::den) / static_cast<CastType>(TRatio::num));
    }

    template <typename TRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& value, ConvertToFpTag)
    {
        typedef typename std::decay<decltype(value)>::type DecayedType;
        using FieldType = typename std::decay<decltype(field)>::type;
        using ValueType = typename FieldType::ValueType;

        auto epsilon = DecayedType(0);
        if (TRatio::num < TRatio::den) {
            epsilon = static_cast<DecayedType>(TRatio::num) / static_cast<DecayedType>(TRatio::den + 1);
        }

        if (epsilon < DecayedType(0)) {
            epsilon = -epsilon;
        }

        if (value < DecayedType(0)) {
            epsilon = -epsilon;
        }

        field.value() =
            static_cast<ValueType>(
                ((value + epsilon) * static_cast<DecayedType>(TRatio::den)) / static_cast<DecayedType>(TRatio::num));
    }

};

template <typename TField, typename TType, bool THasUnits>
struct UnitsChecker
{
    static const bool Value = false;
};

template <typename TField, typename TType>
struct UnitsChecker<TField, TType, true>
{
    static const bool Value =
        std::is_same<typename TField::ParsedOptions::UnitsType, TType>::value;
};

template <typename TField, typename TType>
constexpr bool hasExpectedUnits()
{
    return UnitsChecker<TField, TType, TField::ParsedOptions::HasUnits>::Value;
}

template <typename TRet, typename TConvRatio, typename TField>
TRet getTime(const TField& field)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Time>(),
         "The field is expected to contain \"time\" units.");
    return UnitsValueConverter::getValue<TRet, TConvRatio>(field);
}

template <typename TConvRatio, typename TField, typename TVal>
void setTime(TField& field, TVal&& val)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Time>(),
         "The field is expected to contain \"time\" units.");
    UnitsValueConverter::setValue<TConvRatio>(field, std::forward<TVal>(val));
}

} // namespace details

template <typename TRet, typename TField>
constexpr TRet getNanoseconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::NanosecondsRatio>(field);
}

template <typename TField, typename TVal>
void setNanoseconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::NanosecondsRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TField>
constexpr TRet getMicroseconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::MicrosecondsRatio>(field);
}

template <typename TField, typename TVal>
void setMicroseconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::MicrosecondsRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TField>
constexpr TRet getMilliseconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::MillisecondsRatio>(field);
}

template <typename TField, typename TVal>
constexpr void setMilliseconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::MillisecondsRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TField>
constexpr TRet getSeconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::SecondsRatio>(field);
}

template <typename TField, typename TVal>
void setSeconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::SecondsRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TField>
constexpr TRet getMinutes(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::MinutesRatio>(field);
}

template <typename TField, typename TVal>
void setMinutes(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::MinutesRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TField>
constexpr TRet getHours(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::HoursRatio>(field);
}

template <typename TField, typename TVal>
void setHours(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::HoursRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TField>
constexpr TRet getDays(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::DaysRatio>(field);
}

template <typename TField, typename TVal>
void setDays(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::DaysRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TField>
constexpr TRet getWeeks(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::WeeksRatio>(field);
}

template <typename TField, typename TVal>
void setWeeks(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::WeeksRatio>(field, std::forward<TVal>(val));
}


} // namespace units

} // namespace comms
