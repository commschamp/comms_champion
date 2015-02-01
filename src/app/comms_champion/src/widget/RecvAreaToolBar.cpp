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

const QIcon& deleteIcon()
{
    static const QIcon Icon(":/image/delete.png");
    return Icon;
}

const QIcon& clearIcon()
{
    static const QIcon Icon(":/image/edit-clear.png");
    return Icon;
}

const QIcon& showRecvIcon()
{
    static const QIcon Icon(":/image/msg_recv.png");
    return Icon;
}

const QIcon& showSentIcon()
{
    static const QIcon Icon(":/image/msg_send.png");
    return Icon;
}

const QString StartTooltip("Start Reception");
const QString StopTooltip("Stop Reception");

QAction* createStartButton(QToolBar& bar)
{
    auto* action = bar.addAction(startIcon(), StartTooltip);
    return action;
}

QAction* createSaveButton(QToolBar& bar)
{
    auto* action = bar.addAction(saveIcon(), "Save Messages");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvSaveClicked()));

    return action;
}

QAction* createDeleteButton(QToolBar& bar)
{
    auto* action = bar.addAction(deleteIcon(), "Delete Selected Message");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvDeleteClicked()));
    return action;
}

QAction* createClearButton(QToolBar& bar)
{
    auto* action = bar.addAction(clearIcon(), "Delete All Displayed Messages");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvClearClicked()));
    return action;
}

QAction* createShowReceived(QToolBar& bar)
{
    auto guiAppMgr = GuiAppMgr::instance();
    auto* action = bar.addAction(showRecvIcon(), "Show Received Messages");
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
    auto* action = bar.addAction(showSentIcon(), "Show Sent Messages");
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
    m_showRecvButton(createShowReceived(*this)),
    m_showSentButton(createShowSent(*this)),
    m_state(GuiAppMgr::instance()->recvState())
{
    insertSeparator(m_showRecvButton);
    auto empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    insertWidget(m_showRecvButton, empty);

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
    refreshDeleteButton();
    refreshClearButton();
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

void RecvAreaToolBar::refreshDeleteButton()
{
    auto* button = m_deleteButton;
    assert(button);
    bool enabled = m_msgSelected;
    button->setEnabled(enabled);
}

void RecvAreaToolBar::refreshClearButton()
{
    auto* button = m_clearButton;
    assert(button);
    bool enabled = (!m_listEmpty);
    button->setEnabled(enabled);
}

}  // namespace comms_champion

