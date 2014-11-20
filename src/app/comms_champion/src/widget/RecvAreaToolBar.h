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

#include <functional>
#include <array>

#include <QtWidgets/QToolBar>

#include "GuiAppMgr.h"

class QAction;

namespace comms_champion
{

class RecvAreaToolBar : public QToolBar
{
    Q_OBJECT
    using Base = QToolBar;
public:
    typedef GuiAppMgr::RecvState State;

    RecvAreaToolBar(QWidget* parent = nullptr);

public slots:
    void recvStateChanged(int state);

private:

    using StateChangeHandler = std::function<void ()>;
    using StateChangeHandlersMap =
        std::array<StateChangeHandler, static_cast<unsigned>(State::NumOfStates)>;

    void toIdleState();
    void toRunningState();

    QAction* m_startStopAction = nullptr;
    QAction* m_saveAction = nullptr;
    State m_state = State::Idle;
    StateChangeHandlersMap m_stateChangeHandlers;
};

}  // namespace comms_champion


