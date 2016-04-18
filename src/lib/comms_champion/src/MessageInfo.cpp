//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "comms_champion/MessageInfo.h"

#include <algorithm>
#include <cassert>

#include "comms_champion/StdMeta.h"

namespace comms_champion
{

namespace
{

const QString PropPrefix("cc.msg_");
const QString ProtocolNamePropName = PropPrefix + "prot_name";
const QString DelayPropName = PropPrefix + "delay";
const QString DelayUnitsPropName = PropPrefix + "delay_units";
const QString RepeatPropName = PropPrefix + "repeat";
const QString RepeatUnitsPropName = PropPrefix + "repeat_units";
const QString RepeatCountPropName = PropPrefix + "repeat_count";
const QString TypePropName = PropPrefix + "type";
const QString TimestampPropName = PropPrefix + "timestamp";
const QString NumPropName = PropPrefix + "num";

template <typename T>
T getPropertyAs(const QVariantMap& map, const QString& name, const T& defaultVal = T())
{
    auto iter = map.find(name);
    if (iter == map.end()) {
        return defaultVal;
    }

    auto var = iter.value();
    assert(var.isValid());
    assert(var.canConvert<T>());
    return var.value<T>();
}

void setStringProperty(QVariantMap& map, const QString& name, const QString& value)
{
    if (value.isEmpty()) {
        map.remove(name);
        return;
    }

    map.insert(name, QVariant::fromValue(value));
}

}  // namespace

MessageInfo::MessageInfo() = default;
MessageInfo::~MessageInfo() = default;

MessageInfo::MessagePtr MessageInfo::getAppMessage() const
{
    return m_appMsg;
}

void MessageInfo::setAppMessage(MessagePtr msg)
{
    m_appMsg = std::move(msg);
}

MessageInfo::MessagePtr MessageInfo::getTransportMessage() const
{
    return m_transportMsg;
}

void MessageInfo::setTransportMessage(MessagePtr msg)
{
    m_transportMsg = std::move(msg);
}

MessageInfo::MessagePtr MessageInfo::getRawDataMessage() const
{
    return m_rawDataMsg;
}

void MessageInfo::setRawDataMessage(MessagePtr msg)
{
    m_rawDataMsg = std::move(msg);
}

QString MessageInfo::getProtocolName() const
{
    return getPropertyAs<QString>(m_props, ProtocolNamePropName);
}

void MessageInfo::setProtocolName(const QString& value)
{
    setStringProperty(m_props, ProtocolNamePropName, value);
}

unsigned long long MessageInfo::getDelay() const
{
    return getPropertyAs<unsigned long long>(m_props, DelayPropName);
}

void MessageInfo::setDelay(unsigned long long value)
{
    m_props.insert(DelayPropName, value);
}

QString MessageInfo::getDelayUnits() const
{
    return getPropertyAs<QString>(m_props, DelayUnitsPropName);
}

void MessageInfo::setDelayUnits(const QString& value)
{
    setStringProperty(m_props, DelayUnitsPropName, value);
}

unsigned long long MessageInfo::getRepeatDuration() const
{
    return getPropertyAs<unsigned long long>(m_props, RepeatPropName);
}

void MessageInfo::setRepeatDuration(unsigned long long value)
{
    m_props.insert(RepeatPropName, value);
}

QString MessageInfo::getRepeatDurationUnits() const
{
    return getPropertyAs<QString>(m_props, RepeatUnitsPropName);
}

void MessageInfo::setRepeatDurationUnits(const QString& value)
{
    setStringProperty(m_props, RepeatUnitsPropName, value);
}

unsigned long long MessageInfo::getRepeatCount() const
{
    return getPropertyAs<unsigned long long>(m_props, RepeatCountPropName);
}

void MessageInfo::setRepeatCount(unsigned long long value)
{
    m_props.insert(RepeatCountPropName, value);
}

MessageInfo::MsgType MessageInfo::getMsgType() const
{
    return getPropertyAs<MsgType>(m_props, TypePropName, MsgType::Invalid);
}

void MessageInfo::setMsgType(MsgType value)
{
    assert((value == MsgType::Received) || (value == MsgType::Sent));
    m_props.insert(TypePropName, QVariant::fromValue(value));
}

unsigned long long MessageInfo::getTimestamp() const
{
    return getPropertyAs<unsigned long long>(m_props, TimestampPropName);
}

void MessageInfo::setTimestamp(unsigned long long value)
{
    m_props.insert(TimestampPropName, value);
}

unsigned long long MessageInfo::getMsgNum() const
{
    return getPropertyAs<unsigned long long>(m_props, NumPropName);
}

void MessageInfo::setMsgNum(unsigned long long value)
{
    m_props.insert(NumPropName, value);
}

QVariant MessageInfo::getExtraProperty(const QString& name) const
{
    if (name.startsWith(PropPrefix)) {
        assert(!"Cannot retrieve built-in property directly, use proper functions");
        return QVariant();
    }

    auto iter = m_props.find(name);
    if (iter == m_props.end()) {
        return QVariant();
    }

    return iter.value();
}

bool MessageInfo::setExtraProperty(const QString& name, QVariant&& value)
{
    if (name.startsWith(PropPrefix)) {
        assert(!"Cannot set built-in property directly, use proper functions");
        return false;
    }

    m_props.insert(name, std::move(value));
    return true;
}

const QVariantMap& MessageInfo::getAllProperties() const
{
    return m_props;
}

void MessageInfo::setAllProperties(const QVariantMap& props)
{
    m_props = props;
}

void MessageInfo::setAllProperties(QVariantMap&& props)
{
    m_props = std::move(props);
}

}  // namespace comms_champion

