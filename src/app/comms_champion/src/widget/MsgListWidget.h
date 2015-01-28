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

#include <QtWidgets/QWidget>
#include <QtCore/QString>

#include "comms_champion/MessageInfo.h"

#include "GuiAppMgr.h"

#include "ui_MsgListWidget.h"

namespace comms_champion
{

class Message;

class MsgListWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;
public:
    typedef GuiAppMgr::MsgInfosList MsgInfosList;

    MsgListWidget(
        const QString& listName,
        QWidget* toolbar,
        QWidget* parent = nullptr);

protected slots:
    void addMessage(MessageInfoPtr msgInfo);
    void updateCurrentMessage();
    void deleteCurrentMessage();
    void selectOnAdd(bool enabled);
    void clearSelection();
    void clear();
    void stateChanged(int state);

protected:
    virtual void msgClickedImpl(MessageInfoPtr msgInfo);
    virtual void msgDoubleClickedImpl(MessageInfoPtr msgInfo);
    virtual void msgDeletedImpl(MessageInfoPtr msgInfo);
    virtual void listClearedImpl();
    virtual QString msgPrefixImpl(const MessageInfo& msgInfo) const;
    virtual const QString& msgTooltipImpl() const;
    virtual void stateChangedImpl(int state);

    MessageInfoPtr currentMsg() const;
    MsgInfosList allMsgs() const;

private slots:
    void itemClicked(QListWidgetItem* item);
    void itemDoubleClicked(QListWidgetItem* item);

private:
    MessageInfoPtr getMsgFromItem(QListWidgetItem* item) const;
    QString getMsgNameText(MessageInfoPtr msgInfo);

    Ui::MsgListWidget m_ui;
    bool m_selectOnAdd = false;
};

}  // namespace comms_champion


