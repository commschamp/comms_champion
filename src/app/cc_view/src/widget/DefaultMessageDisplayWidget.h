//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include "MessageDisplayWidget.h"

namespace comms_champion
{

class MsgDetailsWidget;
class ProtocolsStackWidget;
class DefaultMessageDisplayWidget : public MessageDisplayWidget
{
    Q_OBJECT
    using Base = MessageDisplayWidget;
public:
    DefaultMessageDisplayWidget(QWidget* parentObj = nullptr);

protected:
    virtual void displayMessageImpl(MessagePtr msg, bool force) override;
    virtual void setEditEnabledImpl(bool enabled) override;
    virtual void clearImpl() override;
    virtual void refreshImpl() override;

private slots:
    void msgSelectedInProtocol(MessagePtr msg, bool editEnabled);
private:
    MessagePtr m_currMsg;
    MsgDetailsWidget* m_msgDetailsWidget = nullptr;
    ProtocolsStackWidget* m_protocolsDetailsWidget = nullptr;
    bool m_globalEditEnabled = true;
};

}  // namespace comms_champion




