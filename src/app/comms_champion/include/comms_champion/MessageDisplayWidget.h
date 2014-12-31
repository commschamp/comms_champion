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

#include <QtWidgets/QWidget>

#include "MessageInfo.h"

namespace comms_champion
{

class MessageDisplayWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;
public:
    MessageDisplayWidget(QWidget* parent = nullptr)
      : Base(parent)
    {
    }

public slots:

    void displayMessage(MessageInfoPtr msgInfo)
    {
        displayMessageImpl(msgInfo);
    }

    void setEditEnabled(bool enabled)
    {
        setEditEnabledImpl(enabled);
    }


protected:
    virtual void displayMessageImpl(MessageInfoPtr msgInfo) = 0;
    virtual void setEditEnabledImpl(bool enabled) = 0;
};

}  // namespace comms_champion


