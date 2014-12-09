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


#include "comms_champion/MessageInfo.h"

#include <algorithm>
#include <cassert>

namespace comms_champion
{

namespace
{

const std::string AppMsgProperty("AppMsg");
const std::string TransportMsgProperty("TransportMsg");

const std::string* ReservedProperties[] = {
    &AppMsgProperty,
    &TransportMsgProperty
};

bool isReservedProperty(const std::string& property)
{
    return std::any_of(
        std::begin(ReservedProperties),
        std::end(ReservedProperties),
        [&property](const std::string* str) -> bool
        {
            assert(str != nullptr);
            return (property == *str);
        });
}

}  // namespace

MessageInfo::MessagePtr MessageInfo::getAppMessage() const
{
    return getMessage(AppMsgProperty);
}

void MessageInfo::setAppMessage(MessagePtr msg)
{
    setMessage(AppMsgProperty, std::move(msg));
}

MessageInfo::MessagePtr MessageInfo::getTransportMessage() const
{
    return getMessage(TransportMsgProperty);
}

void MessageInfo::setTrasportMessage(MessagePtr msg)
{
    setMessage(TransportMsgProperty, std::move(msg));
}

QVariant MessageInfo::getExtraProperty(const std::string& property)
{
    auto iter = m_map.find(property);
    if (iter == m_map.end()) {
        return QVariant();
    }

    return iter->second;
}

bool MessageInfo::setExtraProperty(const std::string& property, QVariant&& value)
{
    if (isReservedProperty(property)) {
        return false;
    }

    auto iter = m_map.find(property);
    if (iter != m_map.end()) {
        if (!value.isValid()) {
            m_map.erase(iter);
            return true;
        }


        iter->second = std::move(value);
        return true;
    }

    m_map.insert(std::make_pair(property, value));
    return true;
}


MessageInfo::MessagePtr MessageInfo::getMessage(const std::string& property) const
{
    auto iter = m_map.find(property);
    if (iter == m_map.end()) {
        return MessagePtr();
    }

    auto var = iter->second;
    assert(var.isValid());
    assert(var.canConvert<MessagePtr>());
    return var.value<MessagePtr>();
}

void MessageInfo::setMessage(const std::string& property, MessagePtr msg)
{
    auto iter = m_map.find(property);
    if (iter != m_map.end()) {
        if (!msg) {
            m_map.erase(iter);
            return;
        }

        iter->second = QVariant::fromValue(std::move(msg));
        return;
    }

    if (!msg) {
        return;
    }

    m_map.insert(std::make_pair(property, QVariant::fromValue(std::move(msg))));
}

}  // namespace comms_champion

