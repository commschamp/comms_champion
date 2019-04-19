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

#include <functional>
#include <array>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QToolBar>
CC_ENABLE_WARNINGS()

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
    typedef GuiAppMgr::SendState SendState;
    typedef GuiAppMgr::ActivityState ActivityState;

    RecvAreaToolBar(QWidget* parentObj = nullptr);

private slots:
    void startStopClicked();
    void recvListCountReport(unsigned count);
    void recvMsgSelectedReport(int idx);
    void recvStateChanged(int state);
    void sendStateChanged(int state);
    void activeStateChanged(int state);

private:
    void refresh();
    void refreshStartStopButton();
    void refreshLoadButton();
    void refreshSaveButton();
    void refreshDeleteButton();
    void refreshClearButton();

    bool msgSelected() const;
    bool listEmpty() const;

    QAction* m_startStopButton = nullptr;
    QAction* m_loadButton = nullptr;
    QAction* m_saveButton = nullptr;
    QAction* m_deleteButton = nullptr;
    QAction* m_clearButton = nullptr;
    QAction* m_showGarbageButton = nullptr;
    QAction* m_showRecvButton = nullptr;
    QAction* m_showSentButton = nullptr;
    State m_state = State::Idle;
    SendState m_sendState = SendState::Idle;
    ActivityState m_activeState = ActivityState::Inactive;
    int m_selectedIdx = -1;
    unsigned m_listTotal = 0;
};

}  // namespace comms_champion


