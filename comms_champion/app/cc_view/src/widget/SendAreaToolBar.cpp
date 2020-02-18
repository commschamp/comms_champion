//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
#include <QtGui/QIcon>
CC_ENABLE_WARNINGS()

#include "icon.h"
#include "ShortcutMgr.h"

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

QAction* createLoadButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::upload(), "Load Messages");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendLoadClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_LoadSend);
    return action;
}

QAction* createSaveButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::save(), "Save Messages");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendSaveClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_SaveSend);
    return action;
}

QAction* createAddButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::add(), "Add New Message");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendAddClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_AddMessage);
    return action;
}

QAction* createAddRawButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::addRaw(), "Add From Raw Data");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendAddRawClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_AddRaw);
    return action;
}

QAction* createEditButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::edit(), "Edit Selected Message");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendEditClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_EditMessage);
    return action;
}

QAction* createCommentButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::comment(), "Add/Edit Message Comment");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendCommentClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_Comment);
    return action;
}

QAction* createDupButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::dup(), "Duplicate Message");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendDupClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_DupMessage);
    return action;
}

QAction* createDeleteButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::remove(), "Delete Selected Message");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendDeleteClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_Delete);
    return action;
}

QAction* createClearButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::editClear(), "Delete All Messages");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendClearClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_ClearSend);
    return action;
}

QAction* createTopButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::top(), "Move Message to the Top");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendTopClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_Top);
    return action;
}

QAction* createUpButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::up(), "Move Message Up");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendUpClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_Up);
    return action;
}

QAction* createDownButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::down(), "Move Message Down");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendDownClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_Down);
    return action;
}

QAction* createBottomButton(QToolBar& bar)
{
    auto* action = bar.addAction(icon::bottom(), "Move Message to the Bottom");
    QObject::connect(
        action, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(sendBottomClicked()));
    ShortcutMgr::instanceRef().updateShortcut(*action, ShortcutMgr::Key_Bottom);
    return action;
}

}  // namespace

SendAreaToolBar::SendAreaToolBar(QWidget* parentObj)
  : Base(parentObj),
    m_startStopButton(createStartButton(*this)),
    m_startStopAllButton(createStartAllButton(*this)),
    m_loadButton(createLoadButton(*this)),
    m_saveButton(createSaveButton(*this)),
    m_addButton(createAddButton(*this)),
    m_addRawButton(createAddRawButton(*this)),
    m_editButton(createEditButton(*this)),
    m_commentButton(createCommentButton(*this)),
    m_dupButton(createDupButton(*this)),
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
    refreshLoadButton();
    refreshSaveButton();
    refreshAddButtons();
    refreshEditButton();
    refreshCommentButton();
    refreshDupButton();
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
    if (m_state == State::SendingSingle) {
        button->setIcon(icon::stop());
        button->setText(StopTooltip);
        button->setEnabled(true);
    }
    else {
        button->setIcon(icon::start());
        button->setText(StartTooltip);
        bool enabled =
            (m_activeState == ActivityState::Active) &&
            (!listEmpty()) &&
            (msgSelected()) &&
            (m_state == State::Idle);
        button->setEnabled(enabled);
    }
    ShortcutMgr::instanceRef().updateShortcut(*button, ShortcutMgr::Key_Send);
}

void SendAreaToolBar::refreshStartStopAllButton()
{
    auto* button = m_startStopAllButton;
    assert(button);
    if (m_state == State::SendingAll) {
        button->setIcon(icon::stop());
        button->setText(StopTooltip);
        button->setEnabled(true);
    }
    else {
        button->setIcon(icon::startAll());
        button->setText(StartAllTooltip);
        bool enabled =
            (m_activeState == ActivityState::Active) &&
            (!listEmpty()) &&
            (m_state == State::Idle);

        button->setEnabled(enabled);

    }
    ShortcutMgr::instanceRef().updateShortcut(*button, ShortcutMgr::Key_SendAll);
}

void SendAreaToolBar::refreshLoadButton()
{
    auto* button = m_loadButton;
    assert(button != nullptr);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle);
    button->setEnabled(enabled);
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

void SendAreaToolBar::refreshAddButtons()
{
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle);

    auto setEnabledFunc =
        [enabled](QAction* button)
        {
            assert(button);
            button->setEnabled(enabled);
        };

    setEnabledFunc(m_addButton);
    setEnabledFunc(m_addRawButton);
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

void SendAreaToolBar::refreshCommentButton()
{
    auto* button = m_commentButton;
    assert(button);
    bool enabled =
        (m_activeState == ActivityState::Active) &&
        (m_state == State::Idle) &&
        (msgSelected());
    button->setEnabled(enabled);
}

void SendAreaToolBar::refreshDupButton()
{
    auto* button = m_dupButton;
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

