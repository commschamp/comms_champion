//
// Copyright 2015 - 2019 (C). Alex Robenko. All rights reserved.
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
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QAction>
#include <QtWidgets/QPushButton>

#include "ui_PluginConfigDialog.h"
CC_ENABLE_WARNINGS()

#include "comms_champion/PluginMgr.h"
#include "PluginsListWidget.h"

namespace comms_champion
{

class PluginConfigDialog: public QDialog
{
    Q_OBJECT
    using Base = QDialog;

public:
    typedef PluginMgr::ListOfPluginInfos ListOfPluginInfos;
    typedef PluginMgr::PluginInfoPtr PluginInfoPtr;
    PluginConfigDialog(ListOfPluginInfos& outputInfos, QWidget* parentObj = nullptr);
    virtual void accept();

private slots:
    void availSocketPluginClicked(QListWidgetItem* item);
    void availFilterPluginClicked(QListWidgetItem* item);
    void availProtocolPluginClicked(QListWidgetItem* item);

    void availSocketPluginDoubleClicked(QListWidgetItem* item);
    void availFilterPluginDoubleClicked(QListWidgetItem* item);
    void availProtocolPluginDoubleClicked(QListWidgetItem* item);

    void selectedSocketPluginClicked(QListWidgetItem* item);
    void selectedFilterPluginClicked(QListWidgetItem* item);
    void selectedProtocolPluginClicked(QListWidgetItem* item);


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

    void availPluginClicked(
        QListWidgetItem* item,
        PluginsListWidget* availableList);

    void selectedPluginClicked(
        QListWidgetItem* item,
        PluginsListWidget* selectedList);

    void createAvailableToolbar();
    void createSelectedToolbar();
    void createAvailableLists();
    void createSelectedLists();

    void refreshAll();
    void refreshAvailable();
    void refreshAvailablePlugins();
    void refreshAvailableToolbar();
    void refreshSelectedToolbar();
    void refreshSelectedPlugins();
    void refreshSelectedPlugins(const ListOfPluginInfos& infos);
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

    PluginInfoPtr getPluginInfo(QListWidgetItem* item) const;
    ListOfPluginInfos getSelectedPlugins() const;
    PluginsListWidget* getSelectedListForAvailable(PluginsListWidget* list);

    ListOfPluginInfos& m_outputInfos;
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

    PluginsListWidget* m_availableSocketsWidget = nullptr;
    PluginsListWidget* m_availableFiltersWidget = nullptr;
    PluginsListWidget* m_availableProtocolsWidget = nullptr;
    PluginsListWidget* m_currentAvailableList = nullptr;

    PluginsListWidget* m_selectedSocketsWidget = nullptr;
    PluginsListWidget* m_selectedFiltersWidget = nullptr;
    PluginsListWidget* m_selectedProtocolsWidget = nullptr;
    PluginsListWidget* m_currentSelectedList = nullptr;
};

} // namespace comms_champion
