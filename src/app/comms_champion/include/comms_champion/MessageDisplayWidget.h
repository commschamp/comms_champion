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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QWidget>
CC_ENABLE_WARNINGS()

#include "MessageInfo.h"

namespace comms_champion
{

class MessageDisplayWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;
public:
    MessageDisplayWidget(QWidget* parentObj = nullptr)
      : Base(parentObj)
    {
    }

public slots:

    void displayMessage(MessageInfoPtr msgInfo, bool force = false)
    {
        displayMessageImpl(msgInfo, force);
    }

    void setEditEnabled(bool enabled)
    {
        setEditEnabledImpl(enabled);
    }

    void clear()
    {
        clearImpl();
    }

    void refresh()
    {
        refreshImpl();
    }

signals:
    void sigMsgUpdated();

protected:
    virtual void displayMessageImpl(MessageInfoPtr msgInfo, bool force) = 0;
    virtual void setEditEnabledImpl(bool enabled) = 0;
    virtual void clearImpl() = 0;
    virtual void refreshImpl() = 0;
};

}  // namespace comms_champion


