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

#include "RecvAreaToolBar.h"

#include <cassert>

#include <QtCore/QObject>
#include <QtWidgets/QAction>
#include <QtGui/QIcon>

namespace comms_champion
{

namespace
{

const QIcon& startIcon()
{
    static const QIcon Icon(":/image/start.png");
    return Icon;
}

const QIcon& stopIcon()
{
    static const QIcon Icon(":/image/stop.png");
    return Icon;
}

const QIcon& saveIcon()
{
    static const QIcon Icon(":/image/save.png");
    return Icon;
}

const QString StartTooltip("Start Reception");
const QString StopTooltip("Stop Reception");
const QString SaveTooltip("Save Messages");

void connectStartButton(QAction* action)
{
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvStartClicked()));
}

void disconnectStartButton(QAction* action)
{
    QObject::disconnect(action, SIGNAL(triggered()),
                        GuiAppMgr::instance(), SLOT(recvStartClicked()));
}

void connectStopButton(QAction* action)
{
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvStopClicked()));
}

void disconnectStopButton(QAction* action)
{
    QObject::disconnect(action, SIGNAL(triggered()),
                        GuiAppMgr::instance(), SLOT(recvStopClicked()));
}

QAction* createStartButton(QToolBar& bar)
{
    auto* action = bar.addAction(startIcon(), StartTooltip);
    connectStartButton(action);
    return action;
}

QAction* createSaveButton(QToolBar& bar)
{
    auto* action = bar.addAction(saveIcon(), SaveTooltip);
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvSaveClicked()));

    return action;
}

}  // namespace

RecvAreaToolBar::RecvAreaToolBar(QWidget* parent)
  : Base(parent)
{
    m_startStopAction = createStartButton(*this);
    m_saveAction = createSaveButton(*this);

    connect(GuiAppMgr::instance(), SIGNAL(sigSetRecvState(int)),
            this, SLOT(recvStateChanged(int)));

    m_stateChangeHandlers[static_cast<int>(State::Idle)] =
        std::bind(&RecvAreaToolBar::toIdleState, this);

    m_stateChangeHandlers[static_cast<int>(State::Running)] =
        std::bind(&RecvAreaToolBar::toRunningState, this);

    recvStateChanged(static_cast<int>(GuiAppMgr::instance()->recvState()));
}

void RecvAreaToolBar::recvStateChanged(int state)
{
    if ((static_cast<int>(m_stateChangeHandlers.size()) <= state) ||
        (!m_stateChangeHandlers[state])) {
        assert(!"Invalid recv state reported");
        return;
    }

    m_stateChangeHandlers[state]();
}

void RecvAreaToolBar::toIdleState()
{
    if (m_state == State::Idle) {
        return;
    }

    disconnectStopButton(m_startStopAction);
    m_startStopAction->setIcon(startIcon());
    m_startStopAction->setText(StartTooltip);
    connectStartButton(m_startStopAction);

    m_saveAction->setEnabled(true);

    m_state = State::Idle;
}

void RecvAreaToolBar::toRunningState()
{
    if (m_state == State::Running) {
        return;
    }

    disconnectStartButton(m_startStopAction);
    m_startStopAction->setIcon(stopIcon());
    m_startStopAction->setText(StopTooltip);
    connectStopButton(m_startStopAction);

    m_saveAction->setEnabled(false);

    m_state = State::Running;
}

}  // namespace comms_champion

