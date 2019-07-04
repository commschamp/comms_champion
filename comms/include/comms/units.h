//
// Copyright 2017 - 2019 (C). Alex Robenko. All rights reserved.
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

/// @file
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

        using FieldType = typename std::decay<decltype(field)>::type;
        using ValueType = typename FieldType::ValueType;

        static_assert(
            std::is_integral<ValueType>::value ||
            std::is_floating_point<ValueType>::value ||
            std::is_enum<ValueType>::value,
            "Unexpected field in units conversion");

        using CastType =
            typename std::conditional<
                std::is_floating_point<ValueType>::value,
                typename std::conditional<
                    std::is_same<ValueType, float>::value,
                    double,
                    ValueType
                >::type,
                typename std::conditional<
                    std::is_signed<TRet>::value,
                    std::intmax_t,
                    std::uintmax_t
                >::type
        >::type;

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

        static_assert(
            std::is_integral<ValueType>::value ||
            std::is_floating_point<ValueType>::value ||
            std::is_enum<ValueType>::value,
            "Unexpected field in units conversion");

        using CastType =
            typename std::conditional<
                std::is_floating_point<ValueType>::value,
                typename std::conditional<
                    std::is_same<ValueType, float>::value,
                    double,
                    ValueType
                >::type,
                typename std::conditional<
                    std::is_signed<typename std::decay<decltype(value)>::type>::value,
                    std::intmax_t,
                    std::uintmax_t
                >::type
        >::type;

        field.value() =
            static_cast<ValueType>(
                (static_cast<CastType>(value) * TRatio::den) / static_cast<CastType>(TRatio::num));
    }

    template <typename TRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& value, ConvertToFpTag)
    {
        using DecayedType = typename std::decay<decltype(value)>::type;
        using FieldType = typename std::decay<decltype(field)>::type;
        using ValueType = typename FieldType::ValueType;

        auto epsilon = DecayedType(0);
        if ((TRatio::num < TRatio::den) && std::is_integral<ValueType>::value) {
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

template <typename TRet, typename TConvRatio, typename TField>
TRet getDistance(const TField& field)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Distance>(),
         "The field is expected to contain \"distance\" units.");
    return UnitsValueConverter::getValue<TRet, TConvRatio>(field);
}

template <typename TConvRatio, typename TField, typename TVal>
void setDistance(TField& field, TVal&& val)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Distance>(),
         "The field is expected to contain \"distance\" units.");
    UnitsValueConverter::setValue<TConvRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TConvRatio, typename TField>
TRet getSpeed(const TField& field)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Speed>(),
         "The field is expected to contain \"speed\" units.");
    return UnitsValueConverter::getValue<TRet, TConvRatio>(field);
}

template <typename TConvRatio, typename TField, typename TVal>
void setSpeed(TField& field, TVal&& val)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Speed>(),
         "The field is expected to contain \"speed\" units.");
    UnitsValueConverter::setValue<TConvRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TConvRatio, typename TField>
TRet getFrequency(const TField& field)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Frequency>(),
         "The field is expected to contain \"frequency\" units.");
    return UnitsValueConverter::getValue<TRet, TConvRatio>(field);
}

template <typename TConvRatio, typename TField, typename TVal>
void setFrequency(TField& field, TVal&& val)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Frequency>(),
         "The field is expected to contain \"frequency\" units.");
    UnitsValueConverter::setValue<TConvRatio>(field, std::forward<TVal>(val));
}

template <typename T>
struct PI
{
    static constexpr T Value = static_cast<T>(3.14159265358979323846264338327950288419716939937510582097494459230781640628620899L);

};

struct AngleValueConverter
{
    template <typename TRet, typename TConvRatio, typename TField>
    static TRet getValue(const TField& field)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        static_assert(details::hasExpectedUnits<FieldType, comms::traits::units::Angle>(),
             "The field is expected to contain \"angle\" units.");

        using Tag =
            typename std::conditional<
                std::is_same<TConvRatio, typename FieldType::ParsedOptions::UnitsRatio>::value,
                SameUnitsTag,
                typename::std::conditional<
                    std::is_same<TConvRatio, comms::traits::units::RadiansRatio>::value,
                    DegreesToRadiansTag,
                    RadiansToDegreesTag
                >::type
            >::type;

        return getValueInternal<TRet, TConvRatio>(field, Tag());
    }

    template <typename TConvRatio, typename TField, typename TVal>
    static void setValue(TField& field, TVal&& val)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        static_assert(details::hasExpectedUnits<FieldType, comms::traits::units::Angle>(),
             "The field is expected to contain \"angle\" units.");

        using Tag =
            typename std::conditional<
                std::is_same<TConvRatio, typename FieldType::ParsedOptions::UnitsRatio>::value,
                SameUnitsTag,
                typename::std::conditional<
                    std::is_same<TConvRatio, typename comms::traits::units::RadiansRatio>::value,
                    RadiansToDegreesTag,
                    DegreesToRadiansTag
                >::type
            >::type;

        setValueInternal<TConvRatio>(field, std::forward<TVal>(val), Tag());
    }

private:
    struct SameUnitsTag {};
    struct DegreesToRadiansTag {};
    struct RadiansToDegreesTag {};

    template <typename TRet, typename TConvRatio, typename TField>
    static TRet getValueInternal(const TField& field, SameUnitsTag)
    {
        return field.template getScaled<TRet>();
    }

    template <typename TRet, typename TConvRatio, typename TField>
    static TRet getValueInternal(const TField& field, DegreesToRadiansTag)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        static_assert(std::is_same<typename FieldType::ParsedOptions::UnitsRatio, comms::traits::units::DegreesRatio>::value,
             "The field is expected to contain degrees.");

        return PI<TRet>::Value * UnitsValueConverter::getValue<TRet, TConvRatio>(field);
    }

    template <typename TRet, typename TConvRatio, typename TField>
    static TRet getValueInternal(const TField& field, RadiansToDegreesTag)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        static_assert(std::is_same<typename FieldType::ParsedOptions::UnitsRatio, comms::traits::units::RadiansRatio>::value,
             "The field is expected to contain radians.");

        return UnitsValueConverter::getValue<TRet, TConvRatio>(field) / PI<TRet>::Value;
    }

    template <typename TConvRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& val, SameUnitsTag)
    {
        field.setScaled(std::forward<TVal>(val));
    }

    template <typename TConvRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& val, DegreesToRadiansTag)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        static_assert(std::is_same<typename FieldType::ParsedOptions::UnitsRatio, comms::traits::units::RadiansRatio>::value,
             "The field is expected to contain radians.");

        using ValueType = typename std::decay<decltype(val)>::type;
        using PiType =
            typename std::conditional<
                std::is_floating_point<ValueType>::value,
                ValueType,
                double
            >::type;

        UnitsValueConverter::setValue<TConvRatio>(field, val * PI<PiType>::Value);
    }

    template <typename TConvRatio, typename TField, typename TVal>
    static void setValueInternal(TField& field, TVal&& val, RadiansToDegreesTag)
    {
        using FieldType = typename std::decay<decltype(field)>::type;
        static_assert(std::is_same<typename FieldType::ParsedOptions::UnitsRatio, comms::traits::units::DegreesRatio>::value,
             "The field is expected to contain degrees.");

        using ValueType = typename std::decay<decltype(val)>::type;
        using PiType =
            typename std::conditional<
                std::is_floating_point<ValueType>::value,
                ValueType,
                double
            >::type;

        UnitsValueConverter::setValue<TConvRatio>(field, static_cast<PiType>(val) / PI<PiType>::Value);
    }
};

template <typename TRet, typename TConvRatio, typename TField>
TRet getAngle(const TField& field)
{
    return AngleValueConverter::getValue<TRet, TConvRatio>(field);
}

template <typename TConvRatio, typename TField, typename TVal>
void setAngle(TField& field, TVal&& val)
{
    AngleValueConverter::setValue<TConvRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TConvRatio, typename TField>
TRet getCurrent(const TField& field)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Current>(),
         "The field is expected to contain \"current\" units.");
    return UnitsValueConverter::getValue<TRet, TConvRatio>(field);
}

template <typename TConvRatio, typename TField, typename TVal>
void setCurrent(TField& field, TVal&& val)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Current>(),
         "The field is expected to contain \"current\" units.");
    UnitsValueConverter::setValue<TConvRatio>(field, std::forward<TVal>(val));
}

template <typename TRet, typename TConvRatio, typename TField>
TRet getVoltage(const TField& field)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Voltage>(),
         "The field is expected to contain \"voltage\" units.");
    return UnitsValueConverter::getValue<TRet, TConvRatio>(field);
}

template <typename TConvRatio, typename TField, typename TVal>
void setVoltage(TField& field, TVal&& val)
{
    static_assert(details::hasExpectedUnits<typename std::decay<decltype(field)>::type, comms::traits::units::Voltage>(),
         "The field is expected to contain \"voltage\" units.");
    UnitsValueConverter::setValue<TConvRatio>(field, std::forward<TVal>(val));
}

} // namespace details

/// @brief Retrieve field's value as nanoseconds.
/// @details The function will do all the necessary math operations to convert
///     stored value to nanoseconds and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getNanoseconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::NanosecondsRatio>(field);
}

/// @brief Update field's value accordingly, while providing nanoseconds value.
/// @details The function will do all the necessary math operations to convert
///     provided nanoseconds into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setNanoseconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::NanosecondsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds nanoseconds.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isNanoseconds()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::NanosecondsRatio>::value;
}

/// @brief Compile time check whether the field type holds nanoseconds.
/// @details Similar to other @ref isNanoseconds(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isNanoseconds(const TField& field)
{
    return isNanoseconds<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as microseconds.
/// @details The function will do all the necessary math operations to convert
///     stored value to microseconds and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getMicroseconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::MicrosecondsRatio>(field);
}

/// @brief Update field's value accordingly, while providing microseconds value.
/// @details The function will do all the necessary math operations to convert
///     provided microseconds into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setMicroseconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::MicrosecondsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds microseconds.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMicroseconds()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MicrosecondsRatio>::value;
}

/// @brief Compile time check whether the field type holds microseconds.
/// @details Similar to other @ref isMicroseconds(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMicroseconds(const TField& field)
{
    return isMicroseconds<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as milliseconds.
/// @details The function will do all the necessary math operations to convert
///     stored value to milliseconds and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getMilliseconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::MillisecondsRatio>(field);
}

/// @brief Update field's value accordingly, while providing milliseconds value.
/// @details The function will do all the necessary math operations to convert
///     provided milliseconds into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setMilliseconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::MillisecondsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds milliseconds.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMilliseconds()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MillisecondsRatio>::value;
}

/// @brief Compile time check whether the field type holds milliseconds.
/// @details Similar to other @ref isMilliseconds(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMilliseconds(const TField& field)
{
    return isMilliseconds<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as seconds.
/// @details The function will do all the necessary math operations to convert
///     stored value to seconds and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getSeconds(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::SecondsRatio>(field);
}

/// @brief Update field's value accordingly, while providing seconds value.
/// @details The function will do all the necessary math operations to convert
///     provided seconds into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setSeconds(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::SecondsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds seconds.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isSeconds()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::SecondsRatio>::value;
}

/// @brief Compile time check whether the field type holds seconds.
/// @details Similar to other @ref isSeconds(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isSeconds(const TField& field)
{
    return isSeconds<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as minutes.
/// @details The function will do all the necessary math operations to convert
///     stored value to minutes and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getMinutes(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::MinutesRatio>(field);
}

/// @brief Update field's value accordingly, while providing minutes value.
/// @details The function will do all the necessary math operations to convert
///     provided minutes into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setMinutes(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::MinutesRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds minutes.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMinutes()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MinutesRatio>::value;
}

/// @brief Compile time check whether the field type holds minutes.
/// @details Similar to other @ref isMinutes(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMinutes(const TField& field)
{
    return isMinutes<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as hours.
/// @details The function will do all the necessary math operations to convert
///     stored value to hours and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getHours(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::HoursRatio>(field);
}

/// @brief Update field's value accordingly, while providing hours value.
/// @details The function will do all the necessary math operations to convert
///     provided hours into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setHours(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::HoursRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds hours.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isHours()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::HoursRatio>::value;
}

/// @brief Compile time check whether the field type holds hours.
/// @details Similar to other @ref isHours(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isHours(const TField& field)
{
    return isHours<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as days.
/// @details The function will do all the necessary math operations to convert
///     stored value to days and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getDays(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::DaysRatio>(field);
}

/// @brief Update field's value accordingly, while providing days value.
/// @details The function will do all the necessary math operations to convert
///     provided days into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setDays(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::DaysRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds days.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isDays()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::DaysRatio>::value;
}

/// @brief Compile time check whether the field type holds days.
/// @details Similar to other @ref isDays(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isDays(const TField& field)
{
    return isDays<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as weeks.
/// @details The function will do all the necessary math operations to convert
///     stored value to weeks and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TRet, typename TField>
TRet getWeeks(const TField& field)
{
    return details::getTime<TRet, comms::traits::units::WeeksRatio>(field);
}

/// @brief Update field's value accordingly, while providing weeks value.
/// @details The function will do all the necessary math operations to convert
///     provided weeks into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any time value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliseconds,
///     @ref comms::option::def::UnitsSeconds, etc...
template <typename TField, typename TVal>
void setWeeks(TField& field, TVal&& val)
{
    details::setTime<comms::traits::units::WeeksRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds weeks.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isWeeks()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Time>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::WeeksRatio>::value;
}

/// @brief Compile time check whether the field type holds weeks.
/// @details Similar to other @ref isWeeks(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isWeeks(const TField& field)
{
    return isWeeks<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as nanometers.
/// @details The function will do all the necessary math operations to convert
///     stored value to nanometers and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TRet, typename TField>
TRet getNanometers(const TField& field)
{
    return details::getDistance<TRet, comms::traits::units::NanometersRatio>(field);
}

/// @brief Update field's value accordingly, while providing nanometers value.
/// @details The function will do all the necessary math operations to convert
///     provided nanometers into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TField, typename TVal>
void setNanometers(TField& field, TVal&& val)
{
    details::setDistance<comms::traits::units::NanometersRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds nanometers.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isNanometers()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Distance>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::NanometersRatio>::value;
}

/// @brief Compile time check whether the field type holds nanometers.
/// @details Similar to other @ref isNanometers(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isNanometers(const TField& field)
{
    return isNanometers<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as micrometers.
/// @details The function will do all the necessary math operations to convert
///     stored value to micrometers and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TRet, typename TField>
TRet getMicrometers(const TField& field)
{
    return details::getDistance<TRet, comms::traits::units::MicrometersRatio>(field);
}

/// @brief Update field's value accordingly, while providing micrometers value.
/// @details The function will do all the necessary math operations to convert
///     provided micrometers into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TField, typename TVal>
void setMicrometers(TField& field, TVal&& val)
{
    details::setDistance<comms::traits::units::MicrometersRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds micrometers.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMicrometers()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Distance>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MicrometersRatio>::value;
}

/// @brief Compile time check whether the field type holds micrometers.
/// @details Similar to other @ref isMicrometers(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMicrometers(const TField& field)
{
    return isMicrometers<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as millimeters.
/// @details The function will do all the necessary math operations to convert
///     stored value to millimeters and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TRet, typename TField>
TRet getMillimeters(const TField& field)
{
    return details::getDistance<TRet, comms::traits::units::MillimetersRatio>(field);
}

/// @brief Update field's value accordingly, while providing millimeters value.
/// @details The function will do all the necessary math operations to convert
///     provided millimeters into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TField, typename TVal>
void setMillimeters(TField& field, TVal&& val)
{
    details::setDistance<comms::traits::units::MillimetersRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds millimeters.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMillimeters()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Distance>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MillimetersRatio>::value;
}

/// @brief Compile time check whether the field type holds millimeters.
/// @details Similar to other @ref isMillimeters(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMillimeters(const TField& field)
{
    return isMillimeters<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as centimeters.
/// @details The function will do all the necessary math operations to convert
///     stored value to centimeters and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TRet, typename TField>
TRet getCentimeters(const TField& field)
{
    return details::getDistance<TRet, comms::traits::units::CentimetersRatio>(field);
}

/// @brief Update field's value accordingly, while providing centimeters value.
/// @details The function will do all the necessary math operations to convert
///     provided centimeters into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TField, typename TVal>
void setCentimeters(TField& field, TVal&& val)
{
    details::setDistance<comms::traits::units::CentimetersRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds centimeters.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isCentimeters()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Distance>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::CentimetersRatio>::value;
}

/// @brief Compile time check whether the field type holds centimeters.
/// @details Similar to other @ref isCentimeters(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isCentimeters(const TField& field)
{
    return isCentimeters<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as meters.
/// @details The function will do all the necessary math operations to convert
///     stored value to meters and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TRet, typename TField>
TRet getMeters(const TField& field)
{
    return details::getDistance<TRet, comms::traits::units::MetersRatio>(field);
}

/// @brief Update field's value accordingly, while providing meters value.
/// @details The function will do all the necessary math operations to convert
///     provided meters into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TField, typename TVal>
void setMeters(TField& field, TVal&& val)
{
    details::setDistance<comms::traits::units::MetersRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds meters.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMeters()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Distance>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MetersRatio>::value;
}

/// @brief Compile time check whether the field type holds meters.
/// @details Similar to other @ref isMeters(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMeters(const TField& field)
{
    return isMeters<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as kilometers.
/// @details The function will do all the necessary math operations to convert
///     stored value to kilometers and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TRet, typename TField>
TRet getKilometers(const TField& field)
{
    return details::getDistance<TRet, comms::traits::units::KilometersRatio>(field);
}

/// @brief Update field's value accordingly, while providing kilometers value.
/// @details The function will do all the necessary math operations to convert
///     provided kilometers into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any distance value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimeters,
///     @ref comms::option::def::UnitsMeters, etc...
template <typename TField, typename TVal>
void setKilometers(TField& field, TVal&& val)
{
    details::setDistance<comms::traits::units::KilometersRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds kilometers.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isKilometers()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Distance>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::KilometersRatio>::value;
}

/// @brief Compile time check whether the field type holds kilometers.
/// @details Similar to other @ref isKilometers(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isKilometers(const TField& field)
{
    return isKilometers<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as nanometers per second.
/// @details The function will do all the necessary math operations to convert
///     stored value to nm/s and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TRet, typename TField>
TRet getNanometersPerSecond(const TField& field)
{
    return details::getSpeed<TRet, comms::traits::units::NanometersPerSecondRatio>(field);
}

/// @brief Update field's value accordingly, while providing nanometers per second value.
/// @details The function will do all the necessary math operations to convert
///     provided nm/s into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TField, typename TVal>
void setNanometersPerSecond(TField& field, TVal&& val)
{
    details::setSpeed<comms::traits::units::NanometersPerSecondRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds nanometers per second.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isNanometersPerSecond()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Speed>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::NanometersPerSecondRatio>::value;
}

/// @brief Compile time check whether the field type holds nanometers per second.
/// @details Similar to other @ref isNanometersPerSecond(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isNanometersPerSecond(const TField& field)
{
    return isNanometersPerSecond<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as micrometers per second.
/// @details The function will do all the necessary math operations to convert
///     stored value to um/s and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TRet, typename TField>
TRet getMicrometersPerSecond(const TField& field)
{
    return details::getSpeed<TRet, comms::traits::units::MicrometersPerSecondRatio>(field);
}

/// @brief Update field's value accordingly, while providing micrometers per second value.
/// @details The function will do all the necessary math operations to convert
///     provided um/s into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TField, typename TVal>
void setMicrometersPerSecond(TField& field, TVal&& val)
{
    details::setSpeed<comms::traits::units::MicrometersPerSecondRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds micrometers per second.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMicrometersPerSecond()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Speed>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MicrometersPerSecondRatio>::value;
}

/// @brief Compile time check whether the field type holds micrometers per second.
/// @details Similar to other @ref isMicrometersPerSecond(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMicrometersPerSecond(const TField& field)
{
    return isMicrometersPerSecond<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as millimeters per second.
/// @details The function will do all the necessary math operations to convert
///     stored value to mm/s and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TRet, typename TField>
TRet getMillimetersPerSecond(const TField& field)
{
    return details::getSpeed<TRet, comms::traits::units::MillimetersPerSecondRatio>(field);
}

/// @brief Update field's value accordingly, while providing millimeters per second value.
/// @details The function will do all the necessary math operations to convert
///     provided mm/s into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TField, typename TVal>
void setMillimetersPerSecond(TField& field, TVal&& val)
{
    details::setSpeed<comms::traits::units::MillimetersPerSecondRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds millimeters per second.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMillimetersPerSecond()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Speed>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MillimetersPerSecondRatio>::value;
}

/// @brief Compile time check whether the field type holds millimeters per second.
/// @details Similar to other @ref isMillimetersPerSecond(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMillimetersPerSecond(const TField& field)
{
    return isMillimetersPerSecond<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as centimeters per second.
/// @details The function will do all the necessary math operations to convert
///     stored value to cm/s and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TRet, typename TField>
TRet getCentimetersPerSecond(const TField& field)
{
    return details::getSpeed<TRet, comms::traits::units::CentimetersPerSecondRatio>(field);
}

/// @brief Update field's value accordingly, while providing centimeters per second value.
/// @details The function will do all the necessary math operations to convert
///     provided cm/s into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TField, typename TVal>
void setCentimetersPerSecond(TField& field, TVal&& val)
{
    details::setSpeed<comms::traits::units::CentimetersPerSecondRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds centimeters per second.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isCentimetersPerSecond()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Speed>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::CentimetersPerSecondRatio>::value;
}

/// @brief Compile time check whether the field type holds centimeters per second.
/// @details Similar to other @ref isCentimetersPerSecond(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isCentimetersPerSecond(const TField& field)
{
    return isCentimetersPerSecond<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as meters per second.
/// @details The function will do all the necessary math operations to convert
///     stored value to m/s and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TRet, typename TField>
TRet getMetersPerSecond(const TField& field)
{
    return details::getSpeed<TRet, comms::traits::units::MetersPerSecondRatio>(field);
}

/// @brief Update field's value accordingly, while providing meters per second value.
/// @details The function will do all the necessary math operations to convert
///     provided m/s into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TField, typename TVal>
void setMetersPerSecond(TField& field, TVal&& val)
{
    details::setSpeed<comms::traits::units::MetersPerSecondRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds meters per second.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMetersPerSecond()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Speed>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MetersPerSecondRatio>::value;
}

/// @brief Compile time check whether the field type holds meters per second.
/// @details Similar to other @ref isMetersPerSecond(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMetersPerSecond(const TField& field)
{
    return isMetersPerSecond<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as kilometers per second.
/// @details The function will do all the necessary math operations to convert
///     stored value to km/s and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TRet, typename TField>
TRet getKilometersPerSecond(const TField& field)
{
    return details::getSpeed<TRet, comms::traits::units::KilometersPerSecondRatio>(field);
}

/// @brief Update field's value accordingly, while providing kilometers per second value.
/// @details The function will do all the necessary math operations to convert
///     provided km/s into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TField, typename TVal>
void setKilometersPerSecond(TField& field, TVal&& val)
{
    details::setSpeed<comms::traits::units::KilometersPerSecondRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds kilometers per second.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isKilometersPerSecond()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Speed>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::KilometersPerSecondRatio>::value;
}

/// @brief Compile time check whether the field type holds kilometers per second.
/// @details Similar to other @ref isKilometersPerSecond(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isKilometersPerSecond(const TField& field)
{
    return isKilometersPerSecond<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as kilometers per hour.
/// @details The function will do all the necessary math operations to convert
///     stored value to km/h and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TRet, typename TField>
TRet getKilometersPerHour(const TField& field)
{
    return details::getSpeed<TRet, comms::traits::units::KilometersPerHourRatio>(field);
}

/// @brief Update field's value accordingly, while providing kilometers per hour value.
/// @details The function will do all the necessary math operations to convert
///     provided km/h into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any speed value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillimetersPerSecond,
///     @ref comms::option::def::UnitsMetersPerSecond, etc...
template <typename TField, typename TVal>
void setKilometersPerHour(TField& field, TVal&& val)
{
    details::setSpeed<comms::traits::units::KilometersPerHourRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds kilometers per hour.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isKilometersPerHour()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Speed>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::KilometersPerHourRatio>::value;
}

/// @brief Compile time check whether the field type holds kilometers per hour.
/// @details Similar to other @ref isKilometersPerHour(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isKilometersPerHour(const TField& field)
{
    return isKilometersPerHour<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as hertz.
/// @details The function will do all the necessary math operations to convert
///     stored value to hertz and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TRet, typename TField>
TRet getHertz(const TField& field)
{
    return details::getFrequency<TRet, comms::traits::units::HzRatio>(field);
}

/// @brief Update field's value accordingly, while providing hertz value.
/// @details The function will do all the necessary math operations to convert
///     provided hertz into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TField, typename TVal>
void setHertz(TField& field, TVal&& val)
{
    details::setFrequency<comms::traits::units::HzRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds hertz.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isHertz()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Frequency>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::HzRatio>::value;
}

/// @brief Compile time check whether the field type holds hertz.
/// @details Similar to other @ref isHertz(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isHertz(const TField& field)
{
    return isHertz<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as kilohertz.
/// @details The function will do all the necessary math operations to convert
///     stored value to kilohertz and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TRet, typename TField>
TRet getKilohertz(const TField& field)
{
    return details::getFrequency<TRet, comms::traits::units::KiloHzRatio>(field);
}

/// @brief Update field's value accordingly, while providing kilohertz value.
/// @details The function will do all the necessary math operations to convert
///     provided kilohertz into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TField, typename TVal>
void setKilohertz(TField& field, TVal&& val)
{
    details::setFrequency<comms::traits::units::KiloHzRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds kilohertz.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isKilohertz()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Frequency>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::KiloHzRatio>::value;
}

/// @brief Compile time check whether the field type holds kilohertz.
/// @details Similar to other @ref isKilohertz(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isKilohertz(const TField& field)
{
    return isKilohertz<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as megahertz.
/// @details The function will do all the necessary math operations to convert
///     stored value to megahertz and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TRet, typename TField>
TRet getMegahertz(const TField& field)
{
    return details::getFrequency<TRet, comms::traits::units::MegaHzRatio>(field);
}

/// @brief Update field's value accordingly, while providing megahertz value.
/// @details The function will do all the necessary math operations to convert
///     provided megahertz into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TField, typename TVal>
void setMegahertz(TField& field, TVal&& val)
{
    details::setFrequency<comms::traits::units::MegaHzRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds megahertz.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMegahertz()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Frequency>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MegaHzRatio>::value;
}

/// @brief Compile time check whether the field type holds megahertz.
/// @details Similar to other @ref isMegahertz(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMegahertz(const TField& field)
{
    return isMegahertz<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as gigahertz.
/// @details The function will do all the necessary math operations to convert
///     stored value to gigahertz and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TRet, typename TField>
TRet getGigahertz(const TField& field)
{
    return details::getFrequency<TRet, comms::traits::units::GigaHzRatio>(field);
}

/// @brief Update field's value accordingly, while providing gigahertz value.
/// @details The function will do all the necessary math operations to convert
///     provided gigahertz into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any frequency value, using
///     any of the relevant options: @ref comms::option::def::UnitsHertz,
///     @ref comms::option::def::UnitsKilohertz, etc...
template <typename TField, typename TVal>
void setGigahertz(TField& field, TVal&& val)
{
    details::setFrequency<comms::traits::units::GigaHzRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds gigahertz.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isGigahertz()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Frequency>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::GigaHzRatio>::value;
}

/// @brief Compile time check whether the field type holds gigahertz.
/// @details Similar to other @ref isGigahertz(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isGigahertz(const TField& field)
{
    return isGigahertz<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as degrees.
/// @details The function will do all the necessary math operations to convert
///     stored value to degrees and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any angle measurement value, using
///     any of the relevant options: @ref comms::option::def::UnitsDegrees or
///     @ref comms::option::def::UnitsRadians
template <typename TRet, typename TField>
TRet getDegrees(const TField& field)
{
    return details::getAngle<TRet, comms::traits::units::DegreesRatio>(field);
}

/// @brief Update field's value accordingly, while providing degrees value.
/// @details The function will do all the necessary math operations to convert
///     provided degrees into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any angle measurement value, using
///     any of the relevant options: @ref comms::option::def::UnitsDegrees or
///     @ref comms::option::def::UnitsRadians
template <typename TField, typename TVal>
void setDegrees(TField& field, TVal&& val)
{
    details::setAngle<comms::traits::units::DegreesRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds degrees.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isDegrees()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Angle>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::DegreesRatio>::value;
}

/// @brief Compile time check whether the field type holds degrees.
/// @details Similar to other @ref isDegrees(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isDegrees(const TField& field)
{
    return isDegrees<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as radians.
/// @details The function will do all the necessary math operations to convert
///     stored value to radians and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any angle measurement value, using
///     any of the relevant options: @ref comms::option::def::UnitsDegrees or
///     @ref comms::option::def::UnitsRadians
template <typename TRet, typename TField>
TRet getRadians(const TField& field)
{
    return details::getAngle<TRet, comms::traits::units::RadiansRatio>(field);
}

/// @brief Update field's value accordingly, while providing radians value.
/// @details The function will do all the necessary math operations to convert
///     provided radians into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any angle measurement value, using
///     any of the relevant options: @ref comms::option::def::UnitsDegrees or
///     @ref comms::option::def::UnitsRadians
template <typename TField, typename TVal>
void setRadians(TField& field, TVal&& val)
{
    details::setAngle<comms::traits::units::RadiansRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds radians.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isRadians()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Angle>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::RadiansRatio>::value;
}

/// @brief Compile time check whether the field type holds radia s.
/// @details Similar to other @ref isRadians(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isRadians(const TField& field)
{
    return isRadians<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as nanoamps.
/// @details The function will do all the necessary math operations to convert
///     stored value to nanoamps and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TRet, typename TField>
TRet getNanoamps(const TField& field)
{
    return details::getCurrent<TRet, comms::traits::units::NanoampsRatio>(field);
}

/// @brief Update field's value accordingly, while providing nanoamps value.
/// @details The function will do all the necessary math operations to convert
///     provided nanoamps into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TField, typename TVal>
void setNanoamps(TField& field, TVal&& val)
{
    details::setCurrent<comms::traits::units::NanoampsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds nanoamps.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isNanoamps()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Current>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::NanoampsRatio>::value;
}

/// @brief Compile time check whether the field type holds nanoamps.
/// @details Similar to other @ref isNanoamps(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isNanoamps(const TField& field)
{
    return isNanoamps<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as microamps.
/// @details The function will do all the necessary math operations to convert
///     stored value to microamps and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TRet, typename TField>
TRet getMicroamps(const TField& field)
{
    return details::getCurrent<TRet, comms::traits::units::MicroampsRatio>(field);
}

/// @brief Update field's value accordingly, while providing microamps value.
/// @details The function will do all the necessary math operations to convert
///     provided microamps into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TField, typename TVal>
void setMicroamps(TField& field, TVal&& val)
{
    details::setCurrent<comms::traits::units::MicroampsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds microamps.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMicroamps()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Current>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MicroampsRatio>::value;
}

/// @brief Compile time check whether the field type holds microamps.
/// @details Similar to other @ref isMicroamps(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMicroamps(const TField& field)
{
    return isMicroamps<typename std::decay<decltype(field)>::type>();
}


/// @brief Retrieve field's value as milliamps.
/// @details The function will do all the necessary math operations to convert
///     stored value to milliamps and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TRet, typename TField>
TRet getMilliamps(const TField& field)
{
    return details::getCurrent<TRet, comms::traits::units::MilliampsRatio>(field);
}

/// @brief Update field's value accordingly, while providing milliamps value.
/// @details The function will do all the necessary math operations to convert
///     provided milliamps into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TField, typename TVal>
void setMilliamps(TField& field, TVal&& val)
{
    details::setCurrent<comms::traits::units::MilliampsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds milliamps.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMilliamps()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Current>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MilliampsRatio>::value;
}

/// @brief Compile time check whether the field type holds milliamps.
/// @details Similar to other @ref isMilliamps(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMilliamps(const TField& field)
{
    return isMilliamps<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as amps.
/// @details The function will do all the necessary math operations to convert
///     stored value to amps and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TRet, typename TField>
TRet getAmps(const TField& field)
{
    return details::getCurrent<TRet, comms::traits::units::AmpsRatio>(field);
}

/// @brief Update field's value accordingly, while providing amps value.
/// @details The function will do all the necessary math operations to convert
///     provided amps into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TField, typename TVal>
void setAmps(TField& field, TVal&& val)
{
    details::setCurrent<comms::traits::units::AmpsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds amps.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isAmps()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Current>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::AmpsRatio>::value;
}

/// @brief Compile time check whether the field type holds amps.
/// @details Similar to other @ref isAmps(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isAmps(const TField& field)
{
    return isAmps<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as kiloamps.
/// @details The function will do all the necessary math operations to convert
///     stored value to kiloamps and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TRet, typename TField>
TRet getKiloamps(const TField& field)
{
    return details::getCurrent<TRet, comms::traits::units::KiloampsRatio>(field);
}

/// @brief Update field's value accordingly, while providing kiloamps value.
/// @details The function will do all the necessary math operations to convert
///     provided kiloamps into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMilliamps,
///     @ref comms::option::def::UnitsAmps, etc...
template <typename TField, typename TVal>
void setKiloamps(TField& field, TVal&& val)
{
    details::setCurrent<comms::traits::units::KiloampsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds kiloamps.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isKiloamps()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Current>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::KiloampsRatio>::value;
}

/// @brief Compile time check whether the field type holds kiloamps.
/// @details Similar to other @ref isKiloamps(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isKiloamps(const TField& field)
{
    return isKiloamps<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as nanovolts.
/// @details The function will do all the necessary math operations to convert
///     stored value to nanovolts and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical current value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TRet, typename TField>
TRet getNanovolts(const TField& field)
{
    return details::getVoltage<TRet, comms::traits::units::NanovoltsRatio>(field);
}

/// @brief Update field's value accordingly, while providing nanovolts value.
/// @details The function will do all the necessary math operations to convert
///     provided nanovolts into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TField, typename TVal>
void setNanovolts(TField& field, TVal&& val)
{
    details::setVoltage<comms::traits::units::NanovoltsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds nanovolts.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isNanovolts()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Voltage>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::NanovoltsRatio>::value;
}

/// @brief Compile time check whether the field type holds nanovolts.
/// @details Similar to other @ref isNanovolts(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isNanovolts(const TField& field)
{
    return isNanovolts<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as microvolts.
/// @details The function will do all the necessary math operations to convert
///     stored value to microvolts and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TRet, typename TField>
TRet getMicrovolts(const TField& field)
{
    return details::getVoltage<TRet, comms::traits::units::MicrovoltsRatio>(field);
}

/// @brief Update field's value accordingly, while providing microvolts value.
/// @details The function will do all the necessary math operations to convert
///     provided microvolts into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TField, typename TVal>
void setMicrovolts(TField& field, TVal&& val)
{
    details::setVoltage<comms::traits::units::MicrovoltsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds microvolts.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMicrovolts()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Voltage>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MicrovoltsRatio>::value;
}

/// @brief Compile time check whether the field type holds microvolts.
/// @details Similar to other @ref isMicrovolts(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMicrovolts(const TField& field)
{
    return isMicrovolts<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as millivolts.
/// @details The function will do all the necessary math operations to convert
///     stored value to millivolts and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TRet, typename TField>
TRet getMillivolts(const TField& field)
{
    return details::getVoltage<TRet, comms::traits::units::MillivoltsRatio>(field);
}

/// @brief Update field's value accordingly, while providing millivolts value.
/// @details The function will do all the necessary math operations to convert
///     provided millivolts into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TField, typename TVal>
void setMillivolts(TField& field, TVal&& val)
{
    details::setVoltage<comms::traits::units::MillivoltsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds millivolts.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isMillivolts()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Voltage>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::MillivoltsRatio>::value;
}

/// @brief Compile time check whether the field type holds millivolts.
/// @details Similar to other @ref isMillivolts(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isMillivolts(const TField& field)
{
    return isMillivolts<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as volts.
/// @details The function will do all the necessary math operations to convert
///     stored value to volts and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TRet, typename TField>
TRet getVolts(const TField& field)
{
    return details::getVoltage<TRet, comms::traits::units::VoltsRatio>(field);
}

/// @brief Update field's value accordingly, while providing volts value.
/// @details The function will do all the necessary math operations to convert
///     provided volts into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TField, typename TVal>
void setVolts(TField& field, TVal&& val)
{
    details::setVoltage<comms::traits::units::VoltsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds volts.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isVolts()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Voltage>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::VoltsRatio>::value;
}

/// @brief Compile time check whether the field type holds volts.
/// @details Similar to other @ref isVolts(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isVolts(const TField& field)
{
    return isVolts<typename std::decay<decltype(field)>::type>();
}

/// @brief Retrieve field's value as kilovolts.
/// @details The function will do all the necessary math operations to convert
///     stored value to kilovolts and return the result in specified return
///     type.
/// @tparam TRet Return type
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TRet, typename TField>
TRet getKilovolts(const TField& field)
{
    return details::getVoltage<TRet, comms::traits::units::KilovoltsRatio>(field);
}

/// @brief Update field's value accordingly, while providing kilovolts value.
/// @details The function will do all the necessary math operations to convert
///     provided kilovolts into the units stored by the field and update the
///     internal value of the latter accordingly.
/// @tparam TField Type of the field, expected to be a field with integral
///     internal value, such as a variant of comms::field::IntValue.
/// @tparam TVal Type of value to assign.
/// @pre The @b TField type must be defined containing any electrical voltage value, using
///     any of the relevant options: @ref comms::option::def::UnitsMillivolts,
///     @ref comms::option::def::UnitsVolts, etc...
template <typename TField, typename TVal>
void setKilovolts(TField& field, TVal&& val)
{
    details::setVoltage<comms::traits::units::KilovoltsRatio>(field, std::forward<TVal>(val));
}

/// @brief Compile time check whether the field type holds kilovolts.
/// @tparam TField Type of field.
template <typename TField>
constexpr bool isKilovolts()
{
    return
        TField::ParsedOptions::HasUnits &&
        std::is_same<typename TField::ParsedOptions::UnitsType, comms::traits::units::Voltage>::value &&
        std::is_same<typename TField::ParsedOptions::UnitsRatio, comms::traits::units::KilovoltsRatio>::value;
}

/// @brief Compile time check whether the field type holds kilovolts.
/// @details Similar to other @ref isKilovolts(), but allows automatic
///     deduction of the field type.
/// @param[in] field Reference to field object, units of which need to be checked.
template <typename TField>
constexpr bool isKilovolts(const TField& field)
{
    return isKilovolts<typename std::decay<decltype(field)>::type>();
}

} // namespace units

} // namespace comms
