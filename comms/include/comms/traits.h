//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

/// @file comms/traits.h
/// This file contains all the classes necessary to properly
/// define message traits.


#pragma once

#include <ratio>
#include "util/access.h"

namespace comms
{

namespace traits
{

namespace endian
{

/// @brief Empty class used in traits to indicate Big Endian.
using Big = util::traits::endian::Big;

/// @brief Empty class used in traits to indicate Little Endian.
using Little = util::traits::endian::Little;

}  // namespace endian

namespace units
{

/// @brief Tag class used to indicate time value
struct Time {};

/// @brief Tag class used to indicate distance value
struct Distance {};

/// @brief Tag class used to indicate speed value
struct Speed {};

/// @brief Tag class used to indicate frequency value
struct Frequency {};

/// @brief Tag class used to indicate angle value
struct Angle {};

/// @brief Tag class used to indicate electrical current value
struct Current {};

/// @brief Tag class used to indicate electrical voltage value
struct Voltage {};

/// @brief Ratio for nanoseconds
using NanosecondsRatio = std::nano;

/// @brief Ratio for microseconds
using MicrosecondsRatio = std::micro;

/// @brief Ratio for milliseconds
using MillisecondsRatio = std::milli;

/// @brief Ratio for seconds
using SecondsRatio = std::ratio<1, 1>;

/// @brief Ratio for minutes
using MinutesRatio = std::ratio<60>;

/// @brief Ratio for hours
using HoursRatio = std::ratio<60 * 60>;

/// @brief Ratio for days
using DaysRatio = std::ratio<24L * 60 * 60>;

/// @brief Ratio for weeks
using WeeksRatio = std::ratio<7L * 24 * 60 * 60>;

/// @brief Ratio for nanometers
using NanometersRatio = std::nano;

/// @brief Ratio for micrometers
using MicrometersRatio = std::micro;

/// @brief Ratio for millimeters
using MillimetersRatio = std::milli;

/// @brief Ratio for centimeters
using CentimetersRatio = std::centi;

/// @brief Ratio for meters
using MetersRatio = std::ratio<1, 1>;

/// @brief Ratio for kilometers
using KilometersRatio = std::kilo;

/// @brief Ratio for nanometers / second
using NanometersPerSecondRatio =
    typename std::ratio_divide<NanometersRatio, SecondsRatio>::type;

/// @brief Ratio for micrometers / second
using MicrometersPerSecondRatio =
    typename std::ratio_divide<MicrometersRatio, SecondsRatio>::type;

/// @brief Ratio for millimeters / second
using MillimetersPerSecondRatio =
    typename std::ratio_divide<MillimetersRatio, SecondsRatio>::type;

/// @brief Ratio for centimeters / second
using CentimetersPerSecondRatio =
    typename std::ratio_divide<CentimetersRatio, SecondsRatio>::type;

/// @brief Ratio for meters / second
using MetersPerSecondRatio =
    typename std::ratio_divide<MetersRatio, SecondsRatio>::type;

/// @brief Ratio for kilometers / second
using KilometersPerSecondRatio =
    typename std::ratio_divide<KilometersRatio, SecondsRatio>::type;

/// @brief Ratio for kilometers / hour
using KilometersPerHourRatio =
    typename std::ratio_divide<KilometersRatio, HoursRatio>::type;

/// @brief Ratio for hertz
using HzRatio = std::ratio<1, 1>;

/// @brief Ratio for kilohertz
using KiloHzRatio = std::kilo;

/// @brief Ratio for megahertz
using MegaHzRatio = std::mega;

/// @brief Ratio for gigahertz
using GigaHzRatio = std::giga;

/// @brief Ratio for degrees
using DegreesRatio = std::ratio<1, 1>;

/// @brief Ratio for radians
using RadiansRatio = std::ratio<180, 1>;

/// @brief Ratio for nanoamperes
using NanoampsRatio = std::nano;

/// @brief Ratio for microamperes
using MicroampsRatio = std::micro;

/// @brief Ratio for milliamperes
using MilliampsRatio = std::milli;

/// @brief Ratio for amperes
using AmpsRatio = std::ratio<1, 1>;

/// @brief Ratio for kiloamperes
using KiloampsRatio = std::kilo;

/// @brief Ratio for nanovolts
using NanovoltsRatio = std::nano;

/// @brief Ratio for microvolts
using MicrovoltsRatio = std::micro;

/// @brief Ratio for millivolts
using MillivoltsRatio = std::milli;

/// @brief Ratio for volts
using VoltsRatio = std::ratio<1, 1>;

/// @brief Ratio for kilovolts
using KilovoltsRatio = std::kilo;

} // namespace units

namespace dispatch
{

/// @brief Tag class used to indicate polymorphic dispatch
struct Polymorphic {};

/// @brief Tag class used to indicate static binary search dispatch
struct StaticBinSearch {};

/// @brief Tag class used to indicate linear switch dispatch
struct LinearSwitch {};

} // namespace dispatch

}  // namespace traits

}  // namespace comms

