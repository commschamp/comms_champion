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

#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QAction>
#include <QtWidgets/QPushButton>

#include "PluginMgr.h"

#include "ui_PluginConfigDialog.h"

namespace comms_champion
{

class PluginConfigDialog: public QDialog
{
    Q_OBJECT
    using Base = QDialog;

public:
    PluginConfigDialog(QWidget* parent = nullptr);
    virtual void accept();

private slots:
    void availPluginClicked(QListWidgetItem* item);
    void availPluginDoubleClicked(QListWidgetItem* item);
    void selectedPluginClicked(QListWidgetItem* item);
    void addClicked();
    void searchTextChanged(const QString& text);
    void searchClearClicked();
    void loadClicked();
    void saveClicked();
    void removeClicked();
    void clearClicked();
    void topClicked();
    void upClicked();
    void downClicked();
    void bottomClicked();

private:

    void createAvailableToolbar();
    void createSelectedToolbar();

    void refreshAll();
    void refreshAvailable();
    void refreshAvailablePlugins();
    void refreshAvailableToolbar();
    void refreshSelectedToolbar();
    void refreshSelectedPlugins();
    void refreshButtonBox();
    void refreshSaveButton();
    void refreshRemoveButton();
    void refreshClearButton();
    void refreshTopButton();
    void refreshUpBotton();
    void refreshDownBotton();
    void refreshBottomButton();
    void clearConfiguration();
    void clearDescription();
    void moveSelectedPlugin(int fromRow, int toRow);

    PluginMgr::PluginInfoPtr getPluginInfo(QListWidgetItem* item);

    Ui::PluginConfigDialog m_ui;
    QLineEdit* m_availSearchLineEdit = nullptr;
    QAction* m_addButton = nullptr;
    QAction* m_loadButton = nullptr;
    QAction* m_saveButton = nullptr;
    QAction* m_removeButton = nullptr;
    QAction* m_clearButton = nullptr;
    QAction* m_topButton = nullptr;
    QAction* m_upButton = nullptr;
    QAction* m_downButton = nullptr;
    QAction* m_bottomButton = nullptr;
    QPushButton* m_applyButton = nullptr;
};

} /* namespace comms_champion */
