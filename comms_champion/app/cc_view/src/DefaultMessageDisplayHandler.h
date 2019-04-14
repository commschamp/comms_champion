//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
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

#include <cassert>
#include <type_traits>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"

#include "comms_champion/Message.h"
#include "comms_champion/MessageHandler.h"
#include "widget/DefaultMessageWidget.h"

namespace comms_champion
{

class DefaultMessageDisplayHandler : public MessageHandler
{
public:
    using MsgWidgetPtr = std::unique_ptr<MessageWidget>;

    ~DefaultMessageDisplayHandler() noexcept;

    MsgWidgetPtr getMsgWidget();

protected:

    virtual void beginMsgHandlingImpl(Message& msg) override;
    virtual void addExtraTransportFieldImpl(FieldWrapperPtr wrapper) override;
    virtual void addFieldImpl(FieldWrapperPtr wrapper) override;

private:

    using DefaultMsgWidgetPtr = std::unique_ptr<DefaultMessageWidget>;
    DefaultMsgWidgetPtr m_widget;
};

}  // namespace comms_champion


