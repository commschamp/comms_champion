//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include "PluginConfig.h"

#include <cassert>

namespace comms_champion
{

const QVariantMap& PluginConfig::getFullConfig() const
{
    return m_map;
}

void PluginConfig::setFullConfig(const QVariantMap& map)
{
    m_map = map;
}

QVariantMap PluginConfig::getConfig(const QString& key) const
{
    auto subMapVar = m_map.value(key);
    if (!subMapVar.isValid()) {
        return QVariantMap();
    }

    assert(subMapVar.canConvert<QVariantMap>());
    return subMapVar.value<QVariantMap>();
}

void PluginConfig::setConfig(const QString& key, const QVariantMap& map)
{
    m_map.insert(key, QVariant::fromValue(map));
}

}  // namespace comms_champion
