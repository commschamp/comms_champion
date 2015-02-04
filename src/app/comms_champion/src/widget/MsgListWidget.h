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
#include <QtCore/qnamespace.h>

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
    typedef GuiAppMgr::MsgType MsgType;

    MsgListWidget(
        const QString& title,
        QWidget* toolbar,
        QWidget* parent = nullptr);

protected slots:
    void addMessage(MessageInfoPtr msgInfo);
    void updateCurrentMessage();
    void deleteCurrentMessage();
    void selectOnAdd(bool enabled);
    void clearSelection();
    void clear(bool reportDeleted);
    void clear();
    void stateChanged(int state);
    void moveSelectedTop();
    void moveSelectedUp();
    void moveSelectedDown();
    void moveSelectedBottom();
    void titleNeedsUpdate();

protected:
    virtual void msgClickedImpl(MessageInfoPtr msgInfo, int idx);
    virtual void msgDoubleClickedImpl(MessageInfoPtr msgInfo, int idx);
    virtual void msgListClearedImpl(MsgInfosList&& msgInfosList);
    virtual QString msgPrefixImpl(const MessageInfo& msgInfo) const;
    virtual const QString& msgTooltipImpl() const;
    virtual void stateChangedImpl(int state);
    virtual Qt::GlobalColor getItemColourImpl(MsgType type, bool valid) const;
    virtual void msgMovedImpl(int idx);
    virtual QString getTitleImpl() const;

    MessageInfoPtr currentMsg() const;
    MsgInfosList allMsgs() const;

private slots:
    void itemClicked(QListWidgetItem* item);
    void itemDoubleClicked(QListWidgetItem* item);

private:
    MessageInfoPtr getMsgFromItem(QListWidgetItem* item) const;
    QString getMsgNameText(MessageInfoPtr msgInfo);
    Qt::GlobalColor defaultItemColour(bool valid) const;
    void moveItem(int fromRow, int toRow);
    void updateTitle();

    Ui::MsgListWidget m_ui;
    bool m_selectOnAdd = false;
    QString m_title;
};

}  // namespace comms_champion


