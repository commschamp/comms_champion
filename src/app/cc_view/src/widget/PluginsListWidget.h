//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidgetItem>

#include "ui_PluginsListWidget.h"
CC_ENABLE_WARNINGS()

namespace comms_champion
{

class PluginsListWidget : public QWidget
{
    Q_OBJECT
    using Base = QWidget;

public:

    PluginsListWidget(
        const QString& name,
        QWidget* parentObj = nullptr);

    QListWidgetItem* currentItem();
    void clear();
    int count() const;
    QListWidgetItem* item(int idx);
    QListWidgetItem* takeItem(int idx);
    void addItem(const QString& name);
    void setCurrentRow(int rowIdx);
    void setCurrentItem(QListWidgetItem* itemPtr);
    int currentRow() const;
    int getRow(QListWidgetItem* itemPtr) const;
    void insertItem(int row, QListWidgetItem* itemPtr);

signals:
    void itemClicked(QListWidgetItem* itemPtr);
    void itemDoubleClicked(QListWidgetItem* itemPtr);

private:
    Ui::PluginsListWidget m_ui;
};

}  // namespace comms_champion



