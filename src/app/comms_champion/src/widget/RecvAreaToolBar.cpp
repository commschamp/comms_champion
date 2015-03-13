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

#include "icon.h"

namespace comms_champion
{

namespace
{

const QString StartTooltip("Start Reception");
const QString StopTooltip("Stop Reception");

QAction* createStartButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::start(), StartTooltip);
    return action;
}

QAction* createSaveButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::save(), "Save Messages");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvSaveClicked()));

    return action;
}

QAction* createDeleteButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::remove(), "Delete Selected Message");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvDeleteClicked()));
    return action;
}

QAction* createClearButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::editClear(), "Delete All Displayed Messages");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvClearClicked()));
    return action;
}

QAction* createShowGarbage(QToolBar& bar)
{
    auto guiAppMgr = GuiAppMgr::instance();
    auto* action = bar.addAction(
        icon::errorLog(), "Show Message Errors and Garbage Data");
    action->setCheckable(true);
    action->setChecked(guiAppMgr->recvListShowsGarbage());
    QObject::connect(
        action, SIGNAL(triggered(bool)),
        guiAppMgr, SLOT(recvShowGarbageToggled(bool)));
    return action;
}

QAction* createShowReceived(QToolBar& bar)
{
    auto guiAppMgr = GuiAppMgr::instance();
    auto* action = bar.addAction(icon::showRecv(), "Show Received Messages");
    action->setCheckable(true);
    action->setChecked(guiAppMgr->recvListShowsReceived());
    QObject::connect(
        action, SIGNAL(triggered(bool)),
        guiAppMgr, SLOT(recvShowRecvToggled(bool)));
    return action;
}

QAction* createShowSent(QToolBar& bar)
{
    auto guiAppMgr = GuiAppMgr::instance();
    auto* action = bar.addAction(icon::showSent(), "Show Sent Messages");
    action->setCheckable(true);
    action->setChecked(guiAppMgr->recvListShowsSent());
    QObject::connect(
        action, SIGNAL(triggered(bool)),
        guiAppMgr, SLOT(recvShowSentToggled(bool)));
    return action;
}



}  // namespace

RecvAreaToolBar::RecvAreaToolBar(QWidget* parent)
  : Base(parent),
    m_startStopButton(createStartButton(*this)),
    m_saveButton(createSaveButton(*this)),
    m_deleteButton(createDeleteButton(*this)),
    m_clearButton(createClearButton(*this)),
    m_showGarbageButton(createShowGarbage(*this)),
    m_showRecvButton(createShowReceived(*this)),
    m_showSentButton(createShowSent(*this)),
    m_state(GuiAppMgr::instance()->recvState()),
    m_activeState(GuiAppMgr::instance()->getActivityState())
{
    insertSeparator(m_showGarbageButton);
    auto empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    insertWidget(m_showGarbageButton, empty);

    connect(
        m_startStopButton, SIGNAL(triggered()),
        this, SLOT(startStopClicked()));

    auto* guiAppMgr = GuiAppMgr::instance();
    connect(
        guiAppMgr, SIGNAL(sigRecvListCountReport(unsigned)),
        this, SLOT(recvListCountReport(unsigned)));

    connect(
        guiAppMgr, SIGNAL(sigRecvMsgSelected(int)),
        this, SLOT(recvMsgSelectedReport(int)));

    connect(
        guiAppMgr, SIGNAL(sigSetRecvState(int)),
        this, SLOT(recvStateChanged(int)));

    connect(
        guiAppMgr, SIGNAL(sigActivityStateChanged(int)),
        this, SLOT(activeStateChanged(int)));

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

void RecvAreaToolBar::recvListCountReport(unsigned count)
{
    m_listTotal = count;
    refresh();
}

void RecvAreaToolBar::recvMsgSelectedReport(int idx)
{
    m_selectedIdx = idx;
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

void RecvAreaToolBar::activeStateChanged(int state)
{
    auto castedState = static_cast<ActivityState>(state);
    if (m_activeState == castedState) {
        return;
    }

    m_activeState = castedState;
    refresh();
}

void RecvAreaToolBar::refresh()
{
    refreshStartStopButton();
    refreshSaveButton();
    refreshDeleteButton();
    refreshClearButton();
}

void RecvAreaToolBar::refreshStartStopButton()
{
    auto* button = m_startStopButton;
    assert(button != nullptr);
    bool enabled = (m_activeState == ActivityState::Active);
    if (m_state == State::Running) {
        button->setIcon(icon::stop());
        button->setText(StopTooltip);
    }
    else {
        button->setIcon(icon::start());
        button->setText(StartTooltip);
    }
    button->setEnabled(enabled);
}

void RecvAreaToolBar::refreshSaveButton()
{
    auto* button = m_saveButton;
    assert(button != nullptr);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (!listEmpty());
    button->setEnabled(enabled);
}

void RecvAreaToolBar::refreshDeleteButton()
{
    auto* button = m_deleteButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        msgSelected();
    button->setEnabled(enabled);
}

void RecvAreaToolBar::refreshClearButton()
{
    auto* button = m_clearButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (!listEmpty());
    button->setEnabled(enabled);
}

bool RecvAreaToolBar::msgSelected() const
{
    bool result = (0 <= m_selectedIdx);
    assert(m_selectedIdx < static_cast<decltype(m_selectedIdx)>(m_listTotal));
    return result;
}

bool RecvAreaToolBar::listEmpty() const
{
    return 0 == m_listTotal;
}


}  // namespace comms_champion

