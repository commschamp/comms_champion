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

#include <QtWidgets/QDialog>

#include "comms_champion/Protocol.h"
#include "comms_champion/MessageDisplayWidget.h"

#include "ui_MessageUpdateDialog.h"


namespace comms_champion
{

class MessageUpdateDialog : public QDialog
{
    Q_OBJECT
    using Base = QDialog;
public:
    MessageUpdateDialog(
        MessageInfoPtr& msgInfo,
        ProtocolPtr protocol,
        QWidget* parent = nullptr);

private slots:
    void msgUpdated();
    void itemClicked(QListWidgetItem* item);
    void displayMessagePostponed(comms_champion::MessageInfoPtr msgInfo);
    void refreshDisplayedList(const QString& searchText);
    void refreshDelayInfo(int checkboxValue);
    void delayUpdated(int value);
    void refreshRepeatInfo(int checkboxValue);
    void repeatDurationUpdated(int value);
    void repeatCountUpdated(int value);
    void indefinitelyUpdated(int checkboxValue);
    void accept();
    void reset();

private:
    MessageInfoPtr getMsgFromItem(QListWidgetItem* item);
    void refreshButtons();

    MessageInfoPtr& m_msgInfo;
    ProtocolPtr m_protocol;
    Protocol::MessagesList m_allMsgs;
    MessageDisplayWidget* m_msgDisplayWidget = nullptr;
    Ui::MessageUpdateDialog m_ui;
    int m_prevDelay = DisabledDelayValue + 1;
    int m_prevRepeatDuration = DisabledRepeatDuration + 1;
    int m_prevRepeatCount = EnabledMinRepeatCount;
    Qt::CheckState m_sendIndefinitelyState = DisabledSendIndefinitelyState;

    static const int DisabledDelayValue = 0;
    static const int DisabledRepeatDuration = 0;
    static const int DisabledRepeatCount = 1;
    static const int EnabledMinRepeatCount = 2;
    static const Qt::CheckState DisabledSendIndefinitelyState = Qt::Unchecked;
};

}  // namespace comms_champion

