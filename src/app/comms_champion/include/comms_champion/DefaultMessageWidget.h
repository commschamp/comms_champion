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

#include <memory>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFrame>

#include "MessageWidget.h"
#include "FieldWidget.h"
#include "Message.h"

namespace comms_champion
{

class DefaultMessageWidget : public MessageWidget
{
    Q_OBJECT
    using Base = MessageWidget;
public:
    DefaultMessageWidget(Message& msg, QWidget* parent = nullptr);
    ~DefaultMessageWidget() = default;

    void addFieldWidget(FieldWidget* field);
    void insertFieldWidget(int fieldIdx, FieldWidget* field);

protected:
    virtual void refreshImpl() override;
    virtual void setEditEnabledImpl(bool enabled) override;

signals:
    void sigRefreshFields();
    void sigSetEditEnabled(bool enabled);

private:
    std::unique_ptr<QFrame> createFieldSeparator();
    void connectFieldSignals(FieldWidget* field);

    using LayoutType = QVBoxLayout;
    Message& m_msg;
    LayoutType* m_layout;
    uint m_curFieldIdx = 0;
};

}  // namespace comms_champion


