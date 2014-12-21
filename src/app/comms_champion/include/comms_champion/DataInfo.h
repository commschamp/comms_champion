//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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
#include <map>
#include <string>
#include <memory>

#include <QtCore/QVariant>


namespace comms_champion
{

struct DataInfo
{
    typedef std::chrono::high_resolution_clock TimestampClock;
    typedef std::chrono::time_point<TimestampClock> Timestamp;
    typedef std::vector<std::uint8_t> DataSeq;
    typedef std::map<std::string, QVariant> PropertiesMap;

    Timestamp m_timestamp;
    DataSeq m_data;
    PropertiesMap m_extraProperties;
};

using DataInfoPtr = std::shared_ptr<DataInfo>;

}  // namespace comms_champion

Q_DECLARE_METATYPE(comms_champion::DataInfoPtr);


