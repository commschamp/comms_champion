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

#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>

#include "GuiAppMgr.h"

namespace comms_champion
{

class SendAreaToolBar : public QToolBar
{
    Q_OBJECT
    using Base = QToolBar;
public:
    typedef GuiAppMgr::SendState State;

    SendAreaToolBar(QWidget* parent = nullptr);

private slots:
    void sendListEmptyReport(bool empty);
    void sendMsgSelectedReport(bool selected);
    void stateChanged(int state);
    void startStopClicked();
    void startStopAllClicked();

private:
    void refresh();
    void refreshStartStopButton();
    void refreshStartStopAllButton();
    void refreshSaveButton();
    void refreshAddButton();
    void refreshEditButton();
    void refreshDeleteButton();
    void refreshClearButton();

    QAction* m_startStopButton = nullptr;
    QAction* m_startStopAllButton = nullptr;
    QAction* m_saveButton = nullptr;
    QAction* m_addButton = nullptr;
    QAction* m_editButton = nullptr;
    QAction* m_deleteButton = nullptr;
    QAction* m_clearButton = nullptr;
    State m_state = State::Idle;
    bool m_listEmpty = true;
    bool m_msgSelected = false;
};

}  // namespace comms_champion


