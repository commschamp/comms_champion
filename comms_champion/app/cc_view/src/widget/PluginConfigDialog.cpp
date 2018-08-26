//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
CC_ENABLE_WARNINGS()

#include "comms/util/ScopeGuard.h"
#include "PluginMgrG.h"
#include "icon.h"
#include "dir.h"

namespace comms_champion
{

namespace
{

void addHorLine(QBoxLayout& layout) {
    std::unique_ptr<QFrame> line(new QFrame);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout.addWidget(line.release());
}

void addVerLine(QBoxLayout& layout) {
    std::unique_ptr<QFrame> line(new QFrame);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    layout.addWidget(line.release());
}

QString getLastLoadSaveFile()
{
    auto filename = PluginMgrG::instanceRef().getLastFile();
    if (!filename.isEmpty()) {
        return filename;
    }

    auto configDirPath = getConfigDir();
    if (configDirPath.isEmpty()) {
        return filename;
    }

    filename = QDir(configDirPath).path();
    return filename;
}

}  // namespace

PluginConfigDialog::PluginConfigDialog(
    ListOfPluginInfos& outputInfos,
    QWidget* parentObj)
  : Base(parentObj),
    m_outputInfos(outputInfos),
    m_availSearchLineEdit(new QLineEdit())
{
    m_ui.setupUi(this);

    createAvailableToolbar();
    createSelectedToolbar();
    createAvailableLists();
    createSelectedLists();

    assert(parentObj);
    auto newHeight = std::max(height(), (parentObj->height() * 9) / 10);
    auto newWidth = std::max(width(), (parentObj->width() * 8) / 10);
    resize(QSize(newWidth, newHeight));

    m_applyButton = m_ui.m_buttonBox->button(QDialogButtonBox::Ok);
    m_applyButton->setText(tr("Apply"));
    refreshAll();
}

void PluginConfigDialog::accept()
{
    auto& pluginMgr = PluginMgrG::instanceRef();
    auto infos = getSelectedPlugins();
    if (pluginMgr.needsReload(infos)) {
        auto answer =
            QMessageBox::question(
                this,
                tr("Confirmation required!"),
                tr("The list of plugins was updated.\n"
                   "All the plugins must be reloaded and re-applied.\n"
                   "Proceed?"));
        if (answer != QMessageBox::Yes) {
            Base::reject();
            return;
        }
    }

    m_outputInfos = infos;
    Base::accept();
    return;
}

void PluginConfigDialog::availSocketPluginClicked(QListWidgetItem* item)
{
    availPluginClicked(item, m_availableSocketsWidget);
}

void PluginConfigDialog::availFilterPluginClicked(QListWidgetItem* item)
{
    availPluginClicked(item, m_availableFiltersWidget);
}

void PluginConfigDialog::availProtocolPluginClicked(QListWidgetItem* item)
{
    availPluginClicked(item, m_availableProtocolsWidget);
}

void PluginConfigDialog::availSocketPluginDoubleClicked(QListWidgetItem* item)
{
    availSocketPluginClicked(item);
    addClicked();
}

void PluginConfigDialog::availFilterPluginDoubleClicked(QListWidgetItem* item)
{
    availFilterPluginClicked(item);
    addClicked();
}

void PluginConfigDialog::availProtocolPluginDoubleClicked(QListWidgetItem* item)
{
    availProtocolPluginClicked(item);
    addClicked();
}

void PluginConfigDialog::selectedSocketPluginClicked(QListWidgetItem* item)
{
    selectedPluginClicked(item, m_selectedSocketsWidget);
}

void PluginConfigDialog::selectedFilterPluginClicked(QListWidgetItem* item)
{
    selectedPluginClicked(item, m_selectedFiltersWidget);
}

void PluginConfigDialog::selectedProtocolPluginClicked(QListWidgetItem* item)
{
    selectedPluginClicked(item, m_selectedProtocolsWidget);
}

void PluginConfigDialog::addClicked()
{
    assert(m_currentAvailableList != nullptr);
    auto pluginInfoPtr = getPluginInfo(m_currentAvailableList->currentItem());
    assert(pluginInfoPtr);

    auto loadResult = PluginMgrG::instanceRef().loadPlugin(*pluginInfoPtr);
    if (!loadResult) {
        QMessageBox::critical(
            this,
            tr("Plugin Load Error."),
            tr("Failed to load selected plugin."));
        return;
    }

    auto* selectedListWidget = getSelectedListForAvailable(m_currentAvailableList);
    assert(selectedListWidget != nullptr);

    selectedListWidget->addItem(pluginInfoPtr->getName());
    auto* selectedItem = selectedListWidget->item(selectedListWidget->count() - 1);

    selectedItem->setData(
        Qt::UserRole,
        QVariant::fromValue(pluginInfoPtr));

    selectedPluginClicked(selectedItem, selectedListWidget);
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
    auto& pluginMgr = PluginMgrG::instanceRef();
    auto filename =
        QFileDialog::getOpenFileName(
            this,
            tr("Load Configuration File"),
            getLastLoadSaveFile(),
            pluginMgr.getFilesFilter());

    if (filename.isEmpty()) {
        return;
    }

    auto loadedPlugins = pluginMgr.loadPluginsFromConfigFile(filename);
    if (loadedPlugins.empty()) {
        QMessageBox::critical(
            this,
            tr("Configuration Load Error."),
            tr("Invalid configuration file."));
        return;
    }

    refreshSelectedPlugins(loadedPlugins);
    refreshSelectedToolbar();
    refreshAvailablePlugins();
    refreshAvailableToolbar();
    refreshButtonBox();

    assert(m_currentSelectedList == nullptr);
    assert(m_selectedSocketsWidget->currentItem() == nullptr);
    assert(m_selectedFiltersWidget->currentItem() == nullptr);
    assert(m_selectedProtocolsWidget->currentItem() == nullptr);

    if ((m_currentAvailableList == nullptr) ||
        (m_currentAvailableList->currentItem() == nullptr)) {
        clearConfiguration();
        clearDescription();
    }
}

void PluginConfigDialog::saveClicked()
{
    auto& pluginMgr = PluginMgrG::instanceRef();
    auto filename =
        QFileDialog::getSaveFileName(
            this,
            tr("Save Configuration File"),
            getLastLoadSaveFile(),
            pluginMgr.getFilesFilter());

    if (filename.isEmpty()) {
        return;
    }

    auto infos = getSelectedPlugins();

    bool saveResult = pluginMgr.savePluginsToConfigFile(infos, filename);
    if (!saveResult) {
        QMessageBox::critical(
            this,
            tr("File system error!"),
            tr("Failed to save plugins configuration."));
    }
}

void PluginConfigDialog::removeClicked()
{
    assert(m_currentSelectedList != nullptr);
    auto* item = m_currentSelectedList->currentItem();
    assert(item != nullptr);
    delete item;
    refreshAvailablePlugins();
    refreshSelectedToolbar();
    refreshButtonBox();

    item = m_currentSelectedList->currentItem();
    if (item == nullptr) {
        clearConfiguration();
        clearDescription();
        m_currentSelectedList = nullptr;
        return;
    }

    selectedPluginClicked(item, m_currentSelectedList);
}

void PluginConfigDialog::clearClicked()
{
    bool displayingSelected =
        (m_currentSelectedList != nullptr) &&
        (m_currentSelectedList->currentItem() != nullptr);
    m_selectedSocketsWidget->clear();
    m_selectedFiltersWidget->clear();
    m_selectedProtocolsWidget->clear();
    m_currentSelectedList = nullptr;
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
    assert(m_currentSelectedList != nullptr);
    auto curRow = m_currentSelectedList->currentRow();
    if (curRow <= 0) {
        assert(!"No item is selected or moving up top item");
        return;
    }

    moveSelectedPlugin(curRow, 0);
}

void PluginConfigDialog::upClicked()
{
    assert(m_currentSelectedList != nullptr);
    auto curRow = m_currentSelectedList->currentRow();
    if (curRow <= 0) {
        assert(!"No item is selected or moving up top item");
        return;
    }

    moveSelectedPlugin(curRow, curRow - 1);
}

void PluginConfigDialog::downClicked()
{
    assert(m_currentSelectedList != nullptr);
    auto curRow = m_currentSelectedList->currentRow();
    if ((m_currentSelectedList->count() - 1) <= curRow) {
        assert(!"No item is selected or moving down bottom item");
        return;
    }

    moveSelectedPlugin(curRow, curRow + 1);
}

void PluginConfigDialog::bottomClicked()
{
    assert(m_currentSelectedList != nullptr);
    auto curRow = m_currentSelectedList->currentRow();
    if ((m_currentSelectedList->count() - 1) <= curRow) {
        assert(!"No item is selected or moving down bottom item");
        return;
    }

    moveSelectedPlugin(curRow, m_currentSelectedList->count() - 1);
}

void PluginConfigDialog::availPluginClicked(
    QListWidgetItem* item,
    PluginsListWidget* availableList)
{
    assert(item != nullptr);
    assert(availableList != nullptr);

    if (m_currentSelectedList != nullptr) {
        m_currentSelectedList->setCurrentRow(-1);
        refreshSelectedToolbar();
        m_currentSelectedList = nullptr;
    }

    if ((m_currentAvailableList != nullptr) && (m_currentAvailableList != availableList)) {
        m_currentAvailableList->setCurrentRow(-1);
    }

    m_currentAvailableList = availableList;
    m_currentAvailableList->setCurrentItem(item);
    assert(m_currentAvailableList->currentRow() == m_currentAvailableList->getRow(item));

    clearConfiguration();

    auto pluginInfoPtr = getPluginInfo(item);
    assert(pluginInfoPtr);

    m_ui.m_descLabel->setText(pluginInfoPtr->getDescription());
    refreshAvailableToolbar();
}

void PluginConfigDialog::selectedPluginClicked(
    QListWidgetItem* item,
    PluginsListWidget* selectedList)
{
    assert(item != nullptr);
    if (m_currentAvailableList != nullptr) {
        m_currentAvailableList->setCurrentRow(-1);
        m_currentAvailableList = nullptr;
        refreshAvailableToolbar();
    }

    if ((m_currentSelectedList != nullptr) &&
        (m_currentSelectedList != selectedList)) {
        m_currentSelectedList->setCurrentRow(-1);
    }
    m_currentSelectedList = selectedList;

    auto pluginInfoPtr = getPluginInfo(item);
    assert(pluginInfoPtr);

    assert(selectedList != nullptr);
    selectedList->setCurrentItem(item);
    assert(selectedList->currentRow() == selectedList->getRow(item));

    do {
        auto clearGuard =
            comms::util::makeScopeGuard(
                [this]()
                {
                    clearConfiguration();
                });

        auto* plugin = PluginMgrG::instanceRef().loadPlugin(*pluginInfoPtr);
        if (plugin == nullptr) {
            break;
        }

        auto* configWidget = plugin->createConfiguarionWidget();
        if (configWidget == nullptr) {
            break;
        }

        clearGuard.release();
        m_ui.m_configScrollArea->setWidget(configWidget);
    } while (false);

    m_ui.m_descLabel->setText(pluginInfoPtr->getDescription());
    refreshSelectedToolbar();
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

void PluginConfigDialog::createAvailableLists()
{
    std::unique_ptr<QVBoxLayout> layoutPtr(new QVBoxLayout);
    auto* listsLayout = layoutPtr.get();
    assert(listsLayout != nullptr);

    m_ui.m_availableWidget->setLayout(layoutPtr.release());

    addHorLine(*listsLayout);
    std::unique_ptr<PluginsListWidget> socketPlugins(new PluginsListWidget("Socket"));
    m_availableSocketsWidget = socketPlugins.get();
    listsLayout->addWidget(socketPlugins.release());
    connect(
        m_availableSocketsWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(availSocketPluginClicked(QListWidgetItem*)));
    connect(
        m_availableSocketsWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(availSocketPluginDoubleClicked(QListWidgetItem*)));


    addHorLine(*listsLayout);
    std::unique_ptr<PluginsListWidget> filterPlugins(new PluginsListWidget("Filter"));
    m_availableFiltersWidget = filterPlugins.get();
    listsLayout->addWidget(filterPlugins.release());
    connect(
        m_availableFiltersWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(availFilterPluginClicked(QListWidgetItem*)));
    connect(
        m_availableFiltersWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(availFilterPluginDoubleClicked(QListWidgetItem*)));

    addHorLine(*listsLayout);
    std::unique_ptr<PluginsListWidget> protocolPlugins(new PluginsListWidget("Protocol"));
    m_availableProtocolsWidget = protocolPlugins.get();
    listsLayout->addWidget(protocolPlugins.release());
    connect(
        m_availableProtocolsWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(availProtocolPluginClicked(QListWidgetItem*)));
    connect(
        m_availableProtocolsWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(availProtocolPluginDoubleClicked(QListWidgetItem*)));

    listsLayout->setContentsMargins(0, 0, 0, 0);
}

void PluginConfigDialog::createSelectedLists()
{
    std::unique_ptr<QHBoxLayout> layoutPtr(new QHBoxLayout);

    auto* listsLayout = layoutPtr.get();
    assert(listsLayout != nullptr);

    m_ui.m_selectedWidget->setLayout(layoutPtr.release());

    std::unique_ptr<PluginsListWidget> socketPlugins(new PluginsListWidget("Socket"));
    m_selectedSocketsWidget = socketPlugins.get();
    listsLayout->addWidget(socketPlugins.release());
    connect(
        m_selectedSocketsWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(selectedSocketPluginClicked(QListWidgetItem*)));


    addVerLine(*listsLayout);
    std::unique_ptr<PluginsListWidget> filterPlugins(new PluginsListWidget("Filter"));
    m_selectedFiltersWidget = filterPlugins.get();
    listsLayout->addWidget(filterPlugins.release());
    connect(
        m_selectedFiltersWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(selectedFilterPluginClicked(QListWidgetItem*)));


    addVerLine(*listsLayout);
    std::unique_ptr<PluginsListWidget> protocolPlugins(new PluginsListWidget("Protocol"));
    m_selectedProtocolsWidget = protocolPlugins.get();
    listsLayout->addWidget(protocolPlugins.release());
    connect(
        m_selectedProtocolsWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(selectedProtocolPluginClicked(QListWidgetItem*)));

    listsLayout->setContentsMargins(0, 0, 0, 0);
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
    typedef PluginMgr::PluginInfo::Type PluginType;

    auto filterStr = m_availSearchLineEdit->text();

    auto refreshListsFunc =
        [this, &filterStr](PluginsListWidget* availableList, PluginsListWidget* selectedList, PluginType type)
        {
            assert(availableList != nullptr);
            assert(selectedList != nullptr);

            PluginMgr::PluginInfoPtr curInfo;
            auto* curItem = availableList->currentItem();
            if (curItem != nullptr) {
                curInfo = getPluginInfo(curItem);
            }

            availableList->clear();
            auto& availablePlugins = PluginMgrG::instanceRef().getAvailablePlugins();

            for (auto& pluginInfoPtr : availablePlugins) {
                auto& name = pluginInfoPtr->getName();

                if (pluginInfoPtr->getType() != type) {
                    continue;
                }

                if ((!filterStr.isEmpty()) &&
                    (!name.contains(filterStr, Qt::CaseInsensitive))) {
                    continue;
                }

                bool alreadySelected = false;
                for (auto selIdx = 0; selIdx < selectedList->count(); ++selIdx) {
                    auto* selItem = selectedList->item(selIdx);
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

                availableList->addItem(name);
                auto* item = availableList->item(availableList->count() - 1);
                static const QString Tooltip("Use double click to select");
                item->setToolTip(Tooltip);

                item->setData(
                    Qt::UserRole,
                    QVariant::fromValue(pluginInfoPtr));

                if (curInfo == pluginInfoPtr) {
                    availableList->setCurrentRow(availableList->count() - 1);
                }
            }

            bool disabled = false;
            if (((type == PluginType::Socket) || (type == PluginType::Protocol)) &&
                (0 < selectedList->count())) {
                disabled = true;
            }
            availableList->setDisabled(disabled);
        };

    refreshListsFunc(m_availableSocketsWidget, m_selectedSocketsWidget, PluginType::Socket);
    refreshListsFunc(m_availableFiltersWidget, m_selectedFiltersWidget, PluginType::Filter);
    refreshListsFunc(m_availableProtocolsWidget, m_selectedProtocolsWidget, PluginType::Protocol);
}

void PluginConfigDialog::refreshAvailableToolbar()
{
    bool availValid =
        (m_currentAvailableList != nullptr) &&
        (0 <= m_currentAvailableList->currentRow());
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
    refreshSelectedPlugins(PluginMgrG::instanceRef().getAppliedPlugins());
}

void PluginConfigDialog::refreshSelectedPlugins(
    const ListOfPluginInfos& infos)
{
    typedef PluginMgr::PluginInfo::Type PluginType;
    auto refreshListFunc =
        [&infos](PluginsListWidget* list, PluginType type)
        {
            list->clear();

            for (auto& pluginInfoPtr : infos) {
                assert(pluginInfoPtr);
                if (pluginInfoPtr->getType() != type) {
                    continue;
                }

                auto& name = pluginInfoPtr->getName();
                list->addItem(name);
                auto* item = list->item(list->count() - 1);

                item->setData(
                    Qt::UserRole,
                    QVariant::fromValue(pluginInfoPtr));
            }

        };

    m_currentSelectedList = nullptr;
    refreshListFunc(m_selectedSocketsWidget, PluginType::Socket);
    refreshListFunc(m_selectedFiltersWidget, PluginType::Filter);
    refreshListFunc(m_selectedProtocolsWidget, PluginType::Protocol);
}

void PluginConfigDialog::refreshButtonBox()
{
    bool applyEnabled =
        (0 < m_selectedSocketsWidget->count()) &&
        (0 < m_selectedProtocolsWidget->count());
    m_applyButton->setEnabled(applyEnabled);
}

void PluginConfigDialog::refreshSaveButton()
{
    auto* button = m_saveButton;
    bool enabled =
        (0 < m_selectedSocketsWidget->count()) ||
        (0 < m_selectedFiltersWidget->count()) ||
        (0 < m_selectedProtocolsWidget->count());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshRemoveButton()
{
    auto* button = m_removeButton;
    bool enabled =
        (m_currentSelectedList != nullptr) &&
        (0 <= m_currentSelectedList->currentRow());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshClearButton()
{
    auto* button = m_clearButton;
    bool enabled =
        (0 < m_selectedSocketsWidget->count()) ||
        (0 < m_selectedFiltersWidget->count()) ||
        (0 < m_selectedProtocolsWidget->count());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshTopButton()
{
    auto* button = m_topButton;
    bool enabled =
        (m_currentSelectedList != nullptr) &&
        (0 < m_currentSelectedList->currentRow());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshUpBotton()
{
    auto* button = m_upButton;
    bool enabled =
        (m_currentSelectedList != nullptr) &&
        (0 < m_currentSelectedList->currentRow());
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshDownBotton()
{
    auto* button = m_downButton;
    int row = -1;
    if (m_currentSelectedList != nullptr) {
        row = m_currentSelectedList->currentRow();
    }

    bool enabled =
        (0 <= row) &&
        (row < (m_currentSelectedList->count() - 1));
    button->setEnabled(enabled);
}

void PluginConfigDialog::refreshBottomButton()
{
    auto* button = m_bottomButton;
    int row = -1;
    if (m_currentSelectedList != nullptr) {
        row = m_currentSelectedList->currentRow();
    }

    bool enabled =
        (0 <= row) &&
        (row < (m_currentSelectedList->count() - 1));
    button->setEnabled(enabled);
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
    assert(m_currentSelectedList != nullptr);
    assert(fromRow < m_currentSelectedList->count());
    auto* item = m_currentSelectedList->takeItem(fromRow);
    assert(toRow <= m_currentSelectedList->count());
    m_currentSelectedList->insertItem(toRow, item);
    m_currentSelectedList->setCurrentRow(toRow);
    refreshSelectedToolbar();
}

PluginConfigDialog::PluginInfoPtr PluginConfigDialog::getPluginInfo(
    QListWidgetItem* item) const
{
    assert(item != nullptr);
    auto pluginInfoPtrVar = item->data(Qt::UserRole);
    assert(pluginInfoPtrVar.isValid());
    assert(pluginInfoPtrVar.canConvert<PluginMgr::PluginInfoPtr>());
    return pluginInfoPtrVar.value<PluginMgr::PluginInfoPtr>();
}

PluginConfigDialog::ListOfPluginInfos PluginConfigDialog::getSelectedPlugins() const
{
    ListOfPluginInfos infos;

    auto appendPluginInfoFunc =
        [this, &infos](PluginsListWidget* list)
        {
            for (auto idx = 0; idx < list->count(); ++idx) {
                auto* item = list->item(idx);
                assert(item != nullptr);
                auto pluginInfo = getPluginInfo(item);
                assert(pluginInfo);
                infos.push_back(std::move(pluginInfo));
            }
        };

    appendPluginInfoFunc(m_selectedSocketsWidget);
    appendPluginInfoFunc(m_selectedFiltersWidget);
    appendPluginInfoFunc(m_selectedProtocolsWidget);
    return infos;
}

PluginsListWidget* PluginConfigDialog::getSelectedListForAvailable(
    PluginsListWidget* list)
{
    assert(list != nullptr);
    if (list == m_availableSocketsWidget) {
        return m_selectedSocketsWidget;
    }

    if (list == m_availableProtocolsWidget) {
        return m_selectedProtocolsWidget;
    }

    if (list == m_availableFiltersWidget) {
        return m_selectedFiltersWidget;
    }

    return nullptr;
}

} // namespace comms_champion
