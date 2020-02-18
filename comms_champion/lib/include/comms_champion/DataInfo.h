//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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

#include <chrono>
#include <vector>
#include <string>
#include <memory>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
CC_ENABLE_WARNINGS()

#include "Api.h"

namespace comms_champion
{

/// @brief Information about incomming or outdoing data
/// @headerfile "comms_champion/DataInfo.h"
struct DataInfo
{
    /// @brief Type of clock used for timestamping
    using TimestampClock = std::chrono::high_resolution_clock;

    /// @brief Type of timestamps
    using Timestamp = std::chrono::time_point<TimestampClock>;

    /// @brief Type of raw data sequence
    using DataSeq = std::vector<std::uint8_t>;

    /// @brief Type of extra properties storage
    using PropertiesMap = QVariantMap;

    Timestamp m_timestamp; ///< Timestam when data has been received / sent
    DataSeq m_data; ///< Actual raw data
    PropertiesMap m_extraProperties; ///< Extra properties that can be used by
                                     /// other componets
};

/// @brief Pointer to @ref DataInfo
using DataInfoPtr = std::shared_ptr<DataInfo>;

/// @brief Dynamically allocate @ref DataInfo and return in in @ref DataInfoPtr;
CC_API DataInfoPtr makeDataInfo();

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::DataInfoPtr);


