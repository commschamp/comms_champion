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

#include "MainWindowWidget.h"

#include <cassert>

#include <QtWidgets/QSplitter>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

#include "LeftPaneWidget.h"
#include "RightPaneWidget.h"
#include "MessageUpdateDialog.h"
#include "PluginConfigDialog.h"
#include "GuiAppMgr.h"
#include "ConfigMgr.h"
#include "MsgFileMgr.h"
#include "icon.h"

namespace comms_champion
{

namespace
{

void createStandardButtons(QToolBar& bar)
{
    auto* config = bar.addAction(icon::pluginEdit(), "Manage and configure plugings");
    QObject::connect(
        config, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(pluginsEditClicked()));

    bar.addSeparator();
}

}  // namespace

MainWindowWidget::MainWindowWidget(QWidget* parent)
  : Base(parent)
{
    m_ui.setupUi(this);

    m_toolbar = new QToolBar();
    createStandardButtons(*m_toolbar);
    addToolBar(m_toolbar);

    auto* splitter = new QSplitter();
    auto* leftPane = new LeftPaneWidget();
    auto* rightPane = new RightPaneWidget();
    rightPane->resize(leftPane->width() / 2, rightPane->height());
    splitter->addWidget(leftPane);
    splitter->addWidget(rightPane);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));

    auto* guiAppMgr = GuiAppMgr::instance();
    connect(
        guiAppMgr, SIGNAL(sigNewSendMsgDialog(ProtocolPtr)),
        this, SLOT(newSendMsgDialog(ProtocolPtr)));
    connect(
        guiAppMgr, SIGNAL(sigUpdateSendMsgDialog(MessageInfoPtr, ProtocolPtr)),
        this, SLOT(updateSendMsgDialog(MessageInfoPtr, ProtocolPtr)));
    connect(
        guiAppMgr, SIGNAL(sigPluginsEditDialog()),
        this, SLOT(pluginsEditDialog()));
    connect(
        guiAppMgr, SIGNAL(sigErrorReported(const QString&)),
        this, SLOT(displayErrorMsg(const QString&)));
    connect(
        guiAppMgr, SIGNAL(sigAddMainToolbarAction(ActionPtr)),
        this, SLOT(addMainToolbarAction(ActionPtr)));
    connect(
        guiAppMgr, SIGNAL(sigRemoveMainToolbarAction(ActionPtr)),
        this, SLOT(removeMainToolbarAction(ActionPtr)));
    connect(
        guiAppMgr, SIGNAL(sigActivityStateChanged(int)),
        this, SLOT(activeStateChanged(int)));
    connect(
        guiAppMgr, SIGNAL(sigSaveSendMsgsDialog()),
        this, SLOT(saveSendMsgsDialog()));
    connect(
        m_ui.m_actionQuit, SIGNAL(triggered()),
        this, SLOT(close()));
}

MainWindowWidget::~MainWindowWidget()
{
    clearCustomToolbarActions();
}

void MainWindowWidget::newSendMsgDialog(ProtocolPtr protocol)
{
    MessageInfoPtr msgInfo;
    MessageUpdateDialog dialog(msgInfo, std::move(protocol), this);
    dialog.exec();
    if (msgInfo) {
        GuiAppMgr::instance()->sendAddNewMessage(std::move(msgInfo));
    }
}

void MainWindowWidget::updateSendMsgDialog(
    MessageInfoPtr msgInfo,
    ProtocolPtr protocol)
{
    assert(msgInfo);
    MessageUpdateDialog dialog(msgInfo, std::move(protocol), this);
    int result = dialog.exec();
    assert(msgInfo);
    if (result != 0) {
        GuiAppMgr::instance()->sendUpdateMessage(std::move(msgInfo));
    }
}

void MainWindowWidget::pluginsEditDialog()
{
    PluginConfigDialog dialog(this);
    dialog.exec();
}

void MainWindowWidget::displayErrorMsg(const QString& msg)
{
    QMessageBox::critical(
        this,
        QObject::tr("Error occurred!"),
        msg);
}

void MainWindowWidget::addMainToolbarAction(ActionPtr action)
{
    auto iter = std::find(m_customActions.begin(), m_customActions.end(), action);
    if (iter != m_customActions.end())
    {
        assert(!"Adding action second time");
        return;
    }

    assert(m_toolbar != nullptr);
    m_toolbar->addAction(action.get());
    m_customActions.push_back(action);
}

void MainWindowWidget::removeMainToolbarAction(ActionPtr action)
{
    auto iter = std::find(m_customActions.begin(), m_customActions.end(), action);
    if (iter == m_customActions.end())
    {
        assert(!"Removing action that wasn't added");
        return;
    }

    assert(m_toolbar != nullptr);
    m_toolbar->removeAction(action.get());
    m_customActions.erase(iter);
}

void MainWindowWidget::activeStateChanged(int state)
{
    auto castedState = static_cast<ActiveState>(state);
    if (castedState == ActiveState::Clear) {
        clearCustomToolbarActions();
    }
}

void MainWindowWidget::saveSendMsgsDialog()
{
    auto filename = saveMsgsDialog();
    if (filename.isEmpty()) {
        return;
    }

    GuiAppMgr::instance()->sendSaveMsgsToFile(filename);
}

void MainWindowWidget::clearCustomToolbarActions()
{
    assert(m_toolbar != nullptr);
    for (auto& action : m_customActions) {
        m_toolbar->removeAction(action.get());
    }
    m_customActions.clear();
}

QString MainWindowWidget::saveMsgsDialog()
{
    auto& msgsFileMgr = MsgFileMgr::instanceRef();
    return
        QFileDialog::getSaveFileName(
            this,
            tr("Save Messages to File"),
            msgsFileMgr.getLastFile(),
            msgsFileMgr.getFilesFilter());

}


}  // namespace comms_champion
