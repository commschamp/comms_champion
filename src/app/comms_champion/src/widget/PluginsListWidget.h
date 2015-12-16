//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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
        QWidget* parent = nullptr);

    QListWidgetItem* currentItem();
    void clear();
    int count() const;
    QListWidgetItem* item(int idx);
    QListWidgetItem* takeItem(int idx);
    void addItem(const QString& name);
    void setCurrentRow(int row);
    void setCurrentItem(QListWidgetItem* item);
    int currentRow() const;
    int row(QListWidgetItem* item) const;
    void insertItem(int row, QListWidgetItem* item);

signals:
    void itemClicked(QListWidgetItem* item);
    void itemDoubleClicked(QListWidgetItem* item);

private:
    Ui::PluginsListWidget m_ui;
};

}  // namespace comms_champion



