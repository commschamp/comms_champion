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

QAction* createStartButton(QToolBar& bar)
{
    auto* action = bar.addAction(startIcon(), StartTooltip);
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
  : Base(parent),
    m_startStopButton(createStartButton(*this)),
    m_saveButton(createSaveButton(*this)),
    m_state(GuiAppMgr::instance()->recvState())
{
    connect(
        m_startStopButton, SIGNAL(triggered()),
        this, SLOT(startStopClicked()));

    auto* guiAppMgr = GuiAppMgr::instance();
    connect(
        guiAppMgr, SIGNAL(sigRecvListEmpty(bool)),
        this, SLOT(recvListEmptyReport(bool)));

    connect(
        guiAppMgr, SIGNAL(sigRecvMsgSelected(bool)),
        this, SLOT(recvMsgSelectedReport(bool)));

    connect(
        guiAppMgr, SIGNAL(sigSetRecvState(int)),
        this, SLOT(recvStateChanged(int)));

    refresh();
}

void RecvAreaToolBar::startStopClicked()
{
    if (m_state == State::Idle) {
        GuiAppMgr::instance()->recvStartClicked();
        return;
    }

    assert(m_state == State::Running);
    GuiAppMgr::instance()->recvStopClicked();
}

void RecvAreaToolBar::recvListEmptyReport(bool empty)
{
    m_listEmpty = empty;
    refresh();
}

void RecvAreaToolBar::recvMsgSelectedReport(bool selected)
{
    m_msgSelected = selected;
    refresh();
}

void RecvAreaToolBar::recvStateChanged(int state)
{
    auto castedState = static_cast<State>(state);
    if (m_state == castedState) {
        return;
    }

    m_state = castedState;
    refresh();
    return;
}

void RecvAreaToolBar::refresh()
{
    refreshStartStopButton();
    refreshSaveButton();
}

void RecvAreaToolBar::refreshStartStopButton()
{
    auto* button = m_startStopButton;
    assert(button != nullptr);
    if (m_state == State::Running) {
        button->setIcon(stopIcon());
        button->setText(StopTooltip);
    }
    else {
        button->setIcon(startIcon());
        button->setText(StartTooltip);
    }
}

void RecvAreaToolBar::refreshSaveButton()
{
    auto* button = m_saveButton;
    assert(button != nullptr);
    bool enabled =
        (m_state == State::Idle) &&
        (!m_listEmpty);
    button->setEnabled(enabled);
}

}  // namespace comms_champion

