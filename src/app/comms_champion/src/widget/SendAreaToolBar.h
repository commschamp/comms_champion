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
    void sendListCountReport(unsigned count);
    void sendMsgSelectedReport(int idx);
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
    void refreshUpButton(QAction* button);
    void refreshDownButton(QAction* button);

    bool msgSelected() const;
    bool listEmpty() const;

    QAction* m_startStopButton = nullptr;
    QAction* m_startStopAllButton = nullptr;
    QAction* m_saveButton = nullptr;
    QAction* m_addButton = nullptr;
    QAction* m_editButton = nullptr;
    QAction* m_deleteButton = nullptr;
    QAction* m_clearButton = nullptr;
    QAction* m_topButton = nullptr;
    QAction* m_upButton = nullptr;
    QAction* m_downButton = nullptr;
    QAction* m_bottomButton = nullptr;
    State m_state = State::Idle;
    int m_selectedIdx = -1;
    unsigned m_listTotal = 0;
};

}  // namespace comms_champion


