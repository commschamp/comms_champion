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

#include "MsgListWidget.h"

namespace comms_champion
{

class SendMsgListWidget: public MsgListWidget
{
    using Base = MsgListWidget;
public:
    SendMsgListWidget(QWidget* parent = nullptr);

protected:
    virtual void msgClickedImpl(MessageInfoPtr msgInfo) override;
    virtual void msgDoubleClickedImpl(MessageInfoPtr msgInfo) override;
    virtual void msgDeletedImpl(MessageInfoPtr msgInfo) override;
    virtual void listClearedImpl() override;
    virtual QString msgPrefixImpl(const MessageInfo& msgInfo) const override;
    virtual const QString& msgTooltipImpl() const override;
};

} /* namespace comms_champion */
