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

private slots:
    void startStopClicked();
    void recvListEmptyReport(bool empty);
    void recvMsgSelectedReport(bool selected);
    void recvStateChanged(int state);

private:
    void refresh();
    void refreshStartStopButton();
    void refreshSaveButton();
    void refreshDeleteButton();

    QAction* m_startStopButton = nullptr;
    QAction* m_saveButton = nullptr;
    QAction* m_deleteButton = nullptr;
    State m_state = State::Idle;
    bool m_listEmpty = true;
    bool m_msgSelected = false;
};

}  // namespace comms_champion


