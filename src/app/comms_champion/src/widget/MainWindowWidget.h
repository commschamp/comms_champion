//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <list>
#include <tuple>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolBar>

#include "ui_MainWindowWidget.h"
CC_ENABLE_WARNINGS()

#include "comms_champion/Protocol.h"

#include "GuiAppMgr.h"

namespace comms_champion
{

class MainWindowWidget : public QMainWindow
{
    Q_OBJECT
    using Base = QMainWindow;
public:
    typedef GuiAppMgr::ActionPtr ActionPtr;
    typedef GuiAppMgr::ActivityState ActiveState;

    MainWindowWidget(QWidget* parentObj = nullptr);
    ~MainWindowWidget();

private slots:
    void newSendMsgDialog(ProtocolPtr protocol);
    void updateSendMsgDialog(MessagePtr msg, ProtocolPtr protocol);
    void pluginsEditDialog();
    void displayErrorMsg(const QString& msg);
    void addMainToolbarAction(ActionPtr action);
    void clearAllMainToolbarActions();
    void activeStateChanged(int state);
    void loadRecvMsgsDialog(bool askForClear);
    void saveRecvMsgsDialog();
    void loadSendMsgsDialog(bool askForClear);
    void saveSendMsgsDialog();
    void aboutInfo();

private:
    void clearCustomToolbarActions();
    std::tuple<QString, bool> loadMsgsDialog(bool askForClear);
    QString saveMsgsDialog();

    Ui::MainWindowWidget m_ui;
    QToolBar* m_toolbar = nullptr;
    std::list<ActionPtr> m_customActions;
};

}  // namespace comms_champion
