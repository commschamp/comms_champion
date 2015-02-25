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

#include "SendAreaToolBar.h"

#include <cassert>

#include <QtCore/QObject>
#include <QtGui/QIcon>

#include "icon.h"

namespace comms_champion
{

namespace
{

const QString StartTooltip("Send Selected");
const QString StartAllTooltip("Send All");
const QString StopTooltip("Stop Sending");

QAction* createStartButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::start(), StartTooltip);
    return action;
}

QAction* createStartAllButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::startAll(), StartAllTooltip);
    return action;
}

QAction* createSaveButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::save(), "Save Messages");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendSaveClicked()));

    return action;
}

QAction* createAddButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::add(), "Add New Message");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendAddClicked()));
    return action;
}

QAction* createEditButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::edit(), "Edit Selected Message");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendEditClicked()));
    return action;
}

QAction* createDeleteButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::remove(), "Delete Selected Message");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendDeleteClicked()));
    return action;
}

QAction* createClearButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::editClear(), "Delete All Messages");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendClearClicked()));
    return action;
}

QAction* createTopButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::top(), "Move Message to the Top");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendTopClicked()));
    return action;
}

QAction* createUpButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::up(), "Move Message Up");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendUpClicked()));
    return action;
}

QAction* createDownButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::down(), "Move Message Down");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendDownClicked()));
    return action;
}

QAction* createBottomButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::bottom(), "Move Message to the Bottom");
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(sendBottomClicked()));
    return action;
}

}  // namespace

SendAreaToolBar::SendAreaToolBar(QWidget* parent)
  : Base(parent),
    m_startStopButton(createStartButton(*this)),
    m_startStopAllButton(createStartAllButton(*this)),
    m_saveButton(createSaveButton(*this)),
    m_addButton(createAddButton(*this)),
    m_editButton(createEditButton(*this)),
    m_deleteButton(createDeleteButton(*this)),
    m_clearButton(createClearButton(*this)),
    m_topButton(createTopButton(*this)),
    m_upButton(createUpButton(*this)),
    m_downButton(createDownButton(*this)),
    m_bottomButton(createBottomButton(*this)),
    m_state(GuiAppMgr::instance()->sendState()),
    m_activeState(GuiAppMgr::instance()->getActivityState())
{
    connect(
        m_startStopButton, SIGNAL(triggered()),
        this, SLOT(startStopClicked()));

    connect(
        m_startStopAllButton, SIGNAL(triggered()),
        this, SLOT(startStopAllClicked()));

    auto* guiAppMgr = GuiAppMgr::instance();
    connect(
        guiAppMgr, SIGNAL(sigSendListCountReport(unsigned)),
        this, SLOT(sendListCountReport(unsigned)));

    connect(
        guiAppMgr, SIGNAL(sigSendMsgSelected(int)),
        this, SLOT(sendMsgSelectedReport(int)));

    connect(
        guiAppMgr, SIGNAL(sigSetSendState(int)),
        this, SLOT(stateChanged(int)));

    connect(
        guiAppMgr, SIGNAL(sigActivityStateChanged(int)),
        this, SLOT(activeStateChanged(int)));


    refresh();
}

void SendAreaToolBar::sendListCountReport(unsigned count)
{
    m_listTotal = count;
    refresh();
}

void SendAreaToolBar::sendMsgSelectedReport(int idx)
{
    m_selectedIdx = idx;
    refresh();
}

void SendAreaToolBar::stateChanged(int state)
{
    auto castedState = static_cast<State>(state);
    if (m_state == castedState) {
        return;
    }

    m_state = castedState;
    refresh();
}

void SendAreaToolBar::activeStateChanged(int state)
{
    auto castedState = static_cast<ActivityState>(state);
    if (m_activeState == castedState) {
        return;
    }

    m_activeState = castedState;
    refresh();
}

void SendAreaToolBar::startStopClicked()
{
    if (m_state == State::Idle) {
        GuiAppMgr::instance()->sendStartClicked();
        return;
    }

    assert(m_state == State::SendingSingle);
    GuiAppMgr::instance()->sendStopClicked();
}

void SendAreaToolBar::startStopAllClicked()
{
    if (m_state == State::Idle) {
        GuiAppMgr::instance()->sendStartAllClicked();
        return;
    }

    assert(m_state == State::SendingAll);
    GuiAppMgr::instance()->sendStopClicked();
}

void SendAreaToolBar::refresh()
{
    refreshStartStopButton();
    refreshStartStopAllButton();
    refreshSaveButton();
    refreshAddButton();
    refreshEditButton();
    refreshDeleteButton();
    refreshClearButton();
    refreshUpButton(m_topButton);
    refreshUpButton(m_upButton);
    refreshDownButton(m_downButton);
    refreshDownButton(m_bottomButton);
}

void SendAreaToolBar::refreshStartStopButton()
{
    auto* button = m_startStopButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (!listEmpty()) &&
        (msgSelected()) &&
        ((m_state == State::SendingSingle) || (m_state == State::Idle));
    button->setEnabled(enabled);

    if (m_state == State::SendingSingle) {
        button->setIcon(icon::stop());
        button->setText(StopTooltip);
    }
    else {
        button->setIcon(icon::start());
        button->setText(StartTooltip);
    }
}

void SendAreaToolBar::refreshStartStopAllButton()
{
    auto* button = m_startStopAllButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (!listEmpty()) &&
        ((m_state == State::SendingAll) || (m_state == State::Idle));

    button->setEnabled(enabled);

    if (m_state == State::SendingAll) {
        button->setIcon(icon::stop());
        button->setText(StopTooltip);
    }
    else {
        button->setIcon(icon::startAll());
        button->setText(StartAllTooltip);
    }
}

void SendAreaToolBar::refreshSaveButton()
{
    auto* button = m_saveButton;
    assert(button != nullptr);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (!listEmpty());
    button->setEnabled(enabled);
}

void SendAreaToolBar::refreshAddButton()
{
    auto* button = m_addButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle);
    button->setEnabled(enabled);
}

void SendAreaToolBar::refreshEditButton()
{
    auto* button = m_editButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (msgSelected());
    button->setEnabled(enabled);
}

void SendAreaToolBar::refreshDeleteButton()
{
    auto* button = m_deleteButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (msgSelected());
    button->setEnabled(enabled);
}

void SendAreaToolBar::refreshClearButton()
{
    auto* button = m_clearButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (!listEmpty());
    button->setEnabled(enabled);
}

void SendAreaToolBar::refreshUpButton(QAction* button)
{
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (0 < m_selectedIdx);
    button->setEnabled(enabled);
}

void SendAreaToolBar::refreshDownButton(QAction* button)
{
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (msgSelected()) &&
        (m_selectedIdx < static_cast<decltype(m_selectedIdx)>(m_listTotal - 1));
    button->setEnabled(enabled);
}

bool SendAreaToolBar::msgSelected() const
{
    bool result = (0 <= m_selectedIdx);
    assert(m_selectedIdx < static_cast<decltype(m_selectedIdx)>(m_listTotal));
    return result;
}

bool SendAreaToolBar::listEmpty() const
{
    return 0 == m_listTotal;
}

}  // namespace comms_champion

