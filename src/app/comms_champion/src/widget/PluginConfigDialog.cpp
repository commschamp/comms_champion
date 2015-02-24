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

#include "PluginConfigDialog.h"

#include <cassert>
#include <memory>

#include <QtWidgets/QToolBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "icon.h"
#include "ConfigMgr.h"

namespace comms_champion
{

PluginConfigDialog::PluginConfigDialog(QWidget* parent)
  : Base(parent),
    m_availSearchLineEdit(new QLineEdit())
{
    m_ui.setupUi(this);

    createAvailableToolbar();
    createSelectedToolbar();

    assert(parent);
    auto newHeight = std::max(height(), (parent->height() * 9) / 10);
    auto newWidth = std::max(width(), (parent->width() * 7) / 10);
    resize(QSize(newWidth, newHeight));

    connect(
        m_ui.m_availListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(availPluginClicked(QListWidgetItem*)));

    connect(
        m_ui.m_availListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(availPluginDoubleClicked(QListWidgetItem*)));

    connect(
        m_ui.m_selectedListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(selectedPluginClicked(QListWidgetItem*)));

    m_applyButton = m_ui.m_buttonBox->button(QDialogButtonBox::Ok);
    m_applyButton->setText(tr("Apply"));
    refreshAll();
}

void PluginConfigDialog::accept()
{
    typedef PluginMgr::ListOfPluginInfos ListOfPluginInfos;
    ListOfPluginInfos infos;

    assert(0 < m_ui.m_selectedListWidget->count());
    for (auto idx = 0; idx < m_ui.m_selectedListWidget->count(); ++idx) {
        auto* item = m_ui.m_selectedListWidget->item(idx);
        assert(item != nullptr);
        auto pluginInfo = getPluginInfo(item);
        assert(pluginInfo);
        infos.push_back(std::move(pluginInfo));
    }

    auto& pluginMgr = PluginMgr::instanceRef();
    if (pluginMgr.needsReload(infos)) {
        auto result =
            QMessageBox::question(
                this,
                tr("Confirmation required!"),
                tr("The list of plugins was updated.\n"
                   "All the plugins must be reloaded and re-applied.\n"
                   "Proceed?"));
        if (result != QMessageBox::Yes) {
            return;
        }
    }

    bool applyResult = pluginMgr.apply(infos);
    if (!applyResult) {
        QMessageBox::critical(
            this,
            tr("Plugins error occurred!"),
            tr("Failed to apply requested list of plugins."));
        Base::reject();
        return;
    }

    Base::accept();
}

void PluginConfigDialog::availPluginClicked(QListWidgetItem* item)
{
    assert(item != nullptr);
    m_ui.m_selectedListWidget->setCurrentRow(-1);
    refreshSelectedToolbar();

    m_ui.m_availListWidget->setCurrentItem(item);
    assert(m_ui.m_availListWidget->currentRow() == m_ui.m_availListWidget->row(item));

    clearConfiguration();

    auto pluginInfoPtr = getPluginInfo(item);
    assert(pluginInfoPtr);

    m_ui.m_descLabel->setText(pluginInfoPtr->getDescription());
    refreshAvailableToolbar();
}

void PluginConfigDialog::availPluginDoubleClicked(QListWidgetItem* item)
{
    assert(item != nullptr);
    availPluginClicked(item);
    addClicked();
}

void PluginConfigDialog::selectedPluginClicked(QListWidgetItem* item)
{
    assert(item != nullptr);
    m_ui.m_availListWidget->setCurrentRow(-1);
    refreshAvailableToolbar();

    m_ui.m_selectedListWidget->setCurrentItem(item);
    assert(m_ui.m_selectedListWidget->currentRow() == m_ui.m_selectedListWidget->row(item));

    // TODO: proper configuration widget
    clearConfiguration();

    auto pluginInfoPtr = getPluginInfo(item);
    assert(pluginInfoPtr);

    m_ui.m_descLabel->setText(pluginInfoPtr->getDescription());
    refreshSelectedToolbar();
}

void PluginConfigDialog::addClicked()
{
    auto pluginInfoPtr = getPluginInfo(m_ui.m_availListWidget->currentItem());
    assert(pluginInfoPtr);

    auto loadResult = PluginMgr::instanceRef().loadPlugin(*pluginInfoPtr);
    if (!loadResult) {
        QMessageBox::critical(
            this,
            tr("Plugin Load Error."),
            tr("Failed to load selected plugin."));
        return;
    }

    m_ui.m_selectedListWidget->addItem(pluginInfoPtr->getName());
    auto* selectedItem = m_ui.m_selectedListWidget->item(
        m_ui.m_selectedListWidget->count() - 1);

    selectedItem->setData(
        Qt::UserRole,
        QVariant::fromValue(pluginInfoPtr));

    refreshAvailablePlugins();
    refreshSelectedToolbar();
    refreshButtonBox();
}

void PluginConfigDialog::searchTextChanged(const QString& text)
{
    static_cast<void>(text);
    refreshAvailable();
}

void PluginConfigDialog::searchClearClicked()
{
    m_availSearchLineEdit->clear();
    refreshAvailable();
}

void PluginConfigDialog::loadClicked()
{
    auto& configMgr = ConfigMgr::instanceRef();
    auto filename =
        QFileDialog::getOpenFileName(
            this,
            tr("Load Configuration File"),
            configMgr.getLastFile(),
            configMgr.getFilesFilter());

    if (filename.isEmpty()) {
        return;
    }

    auto config = configMgr.loadConfig(filename);
    if (config.getFullConfig().isEmpty()) {
        QMessageBox::critical(
            this,
            tr("Configuration Load Error."),
            tr("Invalid configuration file."));
    }

    // TODO:

    assert(!"NYI: load configuration from file");
}

void PluginConfigDialog::saveClicked()
{
    auto& configMgr = ConfigMgr::instanceRef();
    auto filename =
        QFileDialog::getSaveFileName(
            this,
            tr("Save Configuration File"),
            configMgr.getLastFile(),
            configMgr.getFilesFilter());

    if (filename.isEmpty()) {
        return;
    }

    // TODO:
    assert(!"NYI: save configuration to file");
}

void PluginConfigDialog::removeClicked()
{
    auto* item = m_ui.m_selectedListWidget->currentItem();
    assert(item != nullptr);
    delete item;
    refreshAvailablePlugins();
    refreshSelectedToolbar();
    refreshButtonBox();

    item = m_ui.m_selectedListWidget->currentItem();
    if (item == nullptr) {
        clearConfiguration();
        clearDescription();
        return;
    }

    selectedPluginClicked(item);
}

void PluginConfigDialog::clearClicked()
{
    bool displayingSelected =
        (m_ui.m_selectedListWidget->currentItem() != nullptr);
    m_ui.m_selectedListWidget->clear();
    refreshAvailablePlugins();
    refreshSelectedToolbar();
    refreshButtonBox();

    if (displayingSelected) {
        clearConfiguration();
        clearDescription();
    }
}

void PluginConfigDialog::topClicked()
{
    auto curRow = m_ui.m_selectedListWidget->currentRow();
    if (curRow <= 0) {
        assert(!"No item is selected or moving up top item");
        return;
    }

    moveSelectedPlugin(curRow, 0);
}

void PluginConfigDialog::upClicked()
{
    auto curRow = m_ui.m_selectedListWidget->currentRow();
    if (curRow <= 0) {
        assert(!"No item is selected or moving up top item");
        return;
    }

    moveSelectedPlugin(curRow, curRow - 1);
}

void PluginConfigDialog::downClicked()
{
    auto curRow = m_ui.m_selectedListWidget->currentRow();
    if ((m_ui.m_selectedListWidget->count() - 1) <= curRow) {
        assert(!"No item is selected or moving down bottom item");
        return;
    }

    moveSelectedPlugin(curRow, curRow + 1);
}

void PluginConfigDialog::bottomClicked()
{
    auto curRow = m_ui.m_selectedListWidget->currentRow();
    if ((m_ui.m_selectedListWidget->count() - 1) <= curRow) {
        assert(!"No item is selected or moving down bottom item");
        return;
    }

    moveSelectedPlugin(curRow, m_ui.m_selectedListWidget->count() - 1);
}

void PluginConfigDialog::createAvailableToolbar()
{
    std::unique_ptr<QToolBar> toolbar(new QToolBar());
    m_addButton = toolbar->addAction(icon::add(), "Add plugin to selected list");
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel(tr("Search:")));
    toolbar->addWidget(m_availSearchLineEdit);
    auto* clearSearchAction =
        toolbar->addAction(icon::clear(), "Clear search");

    m_ui.m_availLayout->insertWidget(0, toolbar.release());

    connect(
        m_addButton, SIGNAL(triggered()),
        this, SLOT(addClicked()));
    connect(
        m_availSearchLineEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(searchTextChanged(const QString&)));
    connect(
        clearSearchAction, SIGNAL(triggered()),
        this, SLOT(searchClearClicked()));
}

void PluginConfigDialog::createSelectedToolbar()
{
    std::unique_ptr<QToolBar> toolbar(new QToolBar());
    m_loadButton =
        toolbar->addAction(icon::upload(), "Load list of plugins");
    m_saveButton =
        toolbar->addAction(icon::saveAs(), "Save list of plugins");
    toolbar->addSeparator();
    m_removeButton =
        toolbar->addAction(icon::remove(), "Remove this plugin from selected list");
    m_clearButton =
        toolbar->addAction(icon::editClear(), "Clear selected plugins list");
    toolbar->addSeparator();
    m_topButton =
        toolbar->addAction(icon::top(), "Move selected plugin to the top of the list");
    m_upButton =
        toolbar->addAction(icon::up(), "Move selected plugin up");
    m_downButton =
        toolbar->addAction(icon::down(), "Move selected plugin down");
    m_bottomButton =
        toolbar->addAction(icon::bottom(), "Move selected plugin to the bottom of the list");

    m_ui.m_selectedLayout->insertWidget(0, toolbar.release());

    connect(
        m_loadButton, SIGNAL(triggered()),
        this, SLOT(loadClicked()));

    connect(
        m_saveButton, SIGNAL(triggered()),
        this, SLOT(saveClicked()));

    connect(
        m_removeButton, SIGNAL(triggered()),
        this, SLOT(removeClicked()));

    connect(
        m_clearButton, SIGNAL(triggered()),
        this, SLOT(clearClicked()));

    connect(
        m_topButton, SIGNAL(triggered()),
        this, SLOT(topClicked()));

    connect(
        m_upButton, SIGNAL(triggered()),
        this, SLOT(upClicked()));

    connect(
        m_downButton, SIGNAL(triggered()),
        this, SLOT(downClicked()));

    connect(
        m_bottomButton, SIGNAL(triggered()),
        this, SLOT(bottomClicked()));
}

void PluginConfigDialog::refreshAll()
{
    refreshSelectedPlugins();
    refreshSelectedToolbar();
    refreshAvailablePlugins();
    refreshAvailableToolbar();
    refreshButtonBox();
}

void PluginConfigDialog::refreshAvailable()
{
    refreshAvailablePlugins();
    refreshAvailableToolbar();
}

void PluginConfigDialog::refreshAvailablePlugins()
{
    PluginMgr::PluginInfoPtr curInfo;
    auto* curItem = m_ui.m_availListWidget->currentItem();
    if (curItem != nullptr) {
        curInfo = getPluginInfo(curItem);
    }

    m_ui.m_availListWidget->clear();
    auto& availablePlugins = PluginMgr::instanceRef().getAvailablePlugins();
    auto filterStr = m_availSearchLineEdit->text();

    for (auto& pluginInfoPtr : availablePlugins) {
        auto& name = pluginInfoPtr->getName();

        if ((!filterStr.isEmpty()) &&
            (!name.contains(filterStr, Qt::CaseInsensitive))) {
            continue;
        }

        bool alreadySelected = false;
        for (auto selIdx = 0; selIdx < m_ui.m_selectedListWidget->count(); ++selIdx) {
            auto* selItem = m_ui.m_selectedListWidget->item(selIdx);
            assert(selItem != nullptr);
            auto selPluginInfo = getPluginInfo(selItem);
            if (selPluginInfo == pluginInfoPtr) {
                alreadySelected = true;
                break;
            }
        }

        if (alreadySelected) {
            continue;
        }

        m_ui.m_availListWidget->addItem(name);
        auto* item = m_ui.m_availListWidget->item(
            m_ui.m_availListWidget->count() - 1);
        static const QString Tooltip("Use double click to select");
        item->setToolTip(Tooltip);

        item->setData(
            Qt::UserRole,
            QVariant::fromValue(pluginInfoPtr));

        if (curInfo == pluginInfoPtr) {
            m_ui.m_availListWidget->setCurrentRow(
                                        m_ui.m_availListWidget->count() - 1);
        }
    }
}

void PluginConfigDialog::refreshAvailableToolbar()
{
    bool availValid = (0 <= m_ui.m_availListWidget->currentRow());
    m_addButton->setEnabled(availValid);
}

void PluginConfigDialog::refreshSelectedToolbar()
{
    refreshSaveButton();
    refreshRemoveButton();
    refreshClearButton();
    refreshTopButton();
    refreshUpBotton();
    refreshDownBotton();
    refreshBottomButton();
}

void PluginConfigDialog::refreshSelectedPlugins()
{
    m_ui.m_selectedListWidget->clear();
    auto& appliedPlugins = PluginMgr::instanceRef().getAppliedPlugins();

    for (auto& pluginInfoPtr : appliedPlugins) {
        auto& name = pluginInfoPtr->getName();
        m_ui.m_selectedListWidget->addItem(name);
        auto* item = m_ui.m_selectedListWidget->item(
            m_ui.m_selectedListWidget->count() - 1);

        item->setData(
            Qt::UserRole,
            QVariant::fromValue(pluginInfoPtr));
    }

}

void PluginConfigDialog::refreshButtonBox()
{
    bool applyEnabled = (0 < m_ui.m_selectedListWidget->count());
    m_applyButton->setEnabled(applyEnabled);
}

void PluginConfigDialog::refreshSaveButton()
{
    auto* button = m_saveButton;
    bool enabled = (0 < m_ui.m_selectedListWidget->count());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshRemoveButton()
{
    auto* button = m_removeButton;
    bool enabled = (0 <= m_ui.m_selectedListWidget->currentRow());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshClearButton()
{
    auto* button = m_clearButton;
    bool enabled = (0 < m_ui.m_selectedListWidget->count());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshTopButton()
{
    auto* button = m_topButton;
    bool enabled = (0 < m_ui.m_selectedListWidget->currentRow());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshUpBotton()
{
    auto* button = m_upButton;
    bool enabled = (0 < m_ui.m_selectedListWidget->currentRow());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshDownBotton()
{
    auto* button = m_downButton;
    auto row = m_ui.m_selectedListWidget->currentRow();
    bool enabled =
        (0 <= row) &&
        (row < (m_ui.m_selectedListWidget->count() - 1));
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshBottomButton()
{
    auto* button = m_bottomButton;
    auto row = m_ui.m_selectedListWidget->currentRow();
    bool enabled =
        (0 <= row) &&
        (row < (m_ui.m_selectedListWidget->count() - 1));
    button->setEnabled(enabled);
}

PluginMgr::PluginInfoPtr PluginConfigDialog::getPluginInfo(
    QListWidgetItem* item)
{
    assert(item != nullptr);
    auto pluginInfoPtrVar = item->data(Qt::UserRole);
    assert(pluginInfoPtrVar.isValid());
    assert(pluginInfoPtrVar.canConvert<PluginMgr::PluginInfoPtr>());
    return pluginInfoPtrVar.value<PluginMgr::PluginInfoPtr>();
}

void PluginConfigDialog::clearConfiguration()
{
    m_ui.m_configScrollArea->setWidget(new QWidget());
}

void PluginConfigDialog::clearDescription()
{
    m_ui.m_descLabel->setText(QString());
}

void PluginConfigDialog::moveSelectedPlugin(int fromRow, int toRow)
{
    assert(fromRow < m_ui.m_selectedListWidget->count());
    auto* item = m_ui.m_selectedListWidget->takeItem(fromRow);
    assert(toRow <= m_ui.m_selectedListWidget->count());
    m_ui.m_selectedListWidget->insertItem(toRow, item);
    m_ui.m_selectedListWidget->setCurrentRow(toRow);
    refreshSelectedToolbar();
}

} /* namespace comms_champion */
