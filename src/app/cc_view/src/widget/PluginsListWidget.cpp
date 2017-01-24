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

#include "PluginsListWidget.h"

namespace comms_champion
{

PluginsListWidget::PluginsListWidget(
    const QString& name,
    QWidget* parentObj)
  : Base(parentObj)
{
    m_ui.setupUi(this);
    m_ui.m_groupBox->setTitle(name);

    connect(
        m_ui.m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SIGNAL(itemClicked(QListWidgetItem*)));
    connect(
        m_ui.m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SIGNAL(itemDoubleClicked(QListWidgetItem*)));
}

QListWidgetItem* PluginsListWidget::currentItem()
{
    return m_ui.m_listWidget->currentItem();
}

void PluginsListWidget::clear()
{
    m_ui.m_listWidget->clear();
}

int PluginsListWidget::count() const
{
    return m_ui.m_listWidget->count();
}

QListWidgetItem* PluginsListWidget::item(int idx)
{
    return m_ui.m_listWidget->item(idx);
}

QListWidgetItem* PluginsListWidget::takeItem(int idx)
{
    return m_ui.m_listWidget->takeItem(idx);
}

void PluginsListWidget::addItem(const QString& name)
{
    m_ui.m_listWidget->addItem(name);
}

void PluginsListWidget::setCurrentRow(int rowIdx)
{
    m_ui.m_listWidget->setCurrentRow(rowIdx);
}

void PluginsListWidget::setCurrentItem(QListWidgetItem* itemPtr)
{
    m_ui.m_listWidget->setCurrentItem(itemPtr);
}

int PluginsListWidget::currentRow() const
{
    return m_ui.m_listWidget->currentRow();
}

int PluginsListWidget::getRow(QListWidgetItem* itemPtr) const
{
    return m_ui.m_listWidget->row(itemPtr);
}

void PluginsListWidget::insertItem(int row, QListWidgetItem* itemPtr)
{
    m_ui.m_listWidget->insertItem(row, itemPtr);
}

}  // namespace comms_champion


