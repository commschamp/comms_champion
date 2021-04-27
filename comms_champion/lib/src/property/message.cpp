//
// Copyright 2016 - 2021 (C). Alex Robenko. All rights reserved.
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

#include "comms_champion/property/message.h"

namespace comms_champion
{

namespace property
{

namespace message
{

const QString Type::Name("cc.msg_type");
const QByteArray Type::PropName = Type::Name.toUtf8();

const QString Timestamp::Name("cc.msg_timestamp");
const QByteArray Timestamp::PropName = Timestamp::Name.toUtf8();

const QString ProtocolName::Name("cc.msg_prot_name");
const QByteArray ProtocolName::PropName = ProtocolName::Name.toUtf8();

const QString TransportMsg::Name("cc.msg_transport");
const QByteArray TransportMsg::PropName = TransportMsg::Name.toUtf8();

const QString RawDataMsg::Name("cc.msg_raw_data");
const QByteArray RawDataMsg::PropName = RawDataMsg::Name.toUtf8();

const QString ExtraInfoMsg::Name("cc.msg_extra_info");
const QByteArray ExtraInfoMsg::PropName = ExtraInfoMsg::Name.toUtf8();

const QString ExtraInfo::Name("cc.msg_extra_info_map");
const QByteArray ExtraInfo::PropName = ExtraInfo::Name.toUtf8();

const QString ForceExtraInfoExistence::Name("cc.force_extra_info_exist");
const QByteArray ForceExtraInfoExistence::PropName = ForceExtraInfoExistence::Name.toUtf8();

const QString Delay::Name("cc.msg_delay");
const QByteArray Delay::PropName = Delay::Name.toUtf8();

const QString DelayUnits::Name("cc.msg_delay_units");
const QByteArray DelayUnits::PropName = DelayUnits::Name.toUtf8();

const QString RepeatDuration::Name("cc.msg_repeat");
const QByteArray RepeatDuration::PropName = RepeatDuration::Name.toUtf8();

const QString RepeatDurationUnits::Name("cc.msg_repeat_units");
const QByteArray RepeatDurationUnits::PropName = RepeatDurationUnits::Name.toUtf8();

const QString RepeatCount::Name("cc.msg_repeat_count");
const QByteArray RepeatCount::PropName = RepeatCount::Name.toUtf8();

const QString ScrollPos::Name("cc.msg_scroll_pos");
const QByteArray ScrollPos::PropName = ScrollPos::Name.toUtf8();

const QString Comment::Name("cc.msg_comment");
const QByteArray Comment::PropName = Comment::Name.toUtf8();

}  // namespace message

}  // namespace property

}  // namespace comms_champion

