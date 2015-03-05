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
#include <QtWidgets/QToolBar>
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
#include "icon.h"

namespace comms_champion
{

namespace
{

void createPluginsButton(QToolBar& bar)
{
    auto* config = bar.addAction(icon::pluginEdit(), "Manage and configure plugings");
    QObject::connect(
        config, SIGNAL(triggered()),
        GuiAppMgr::instance(), SLOT(pluginsEditClicked()));
}

}  // namespace

MainWindowWidget::MainWindowWidget(QWidget* parent)
  : Base(parent)
{
    m_ui.setupUi(this);

    auto* toolbar = new QToolBar();
    createPluginsButton(*toolbar);
    addToolBar(toolbar);

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
        m_ui.m_actionQuit, SIGNAL(triggered()),
        this, SLOT(close()));
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

}  // namespace comms_champion
