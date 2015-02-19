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

#include "icon.h"

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

    refreshAll();
}

void PluginConfigDialog::availPluginClicked(QListWidgetItem* item)
{
    m_ui.m_selectedListWidget->setCurrentRow(-1);
    m_ui.m_availListWidget->setCurrentItem(item);
    assert(m_ui.m_availListWidget->currentRow() == m_ui.m_availListWidget->row(item));

    m_ui.m_configScrollArea->setWidget(new QWidget());

    auto pluginInfoPtr = getPluginInfo(item);
    assert(pluginInfoPtr);

    m_ui.m_descLabel->setText(pluginInfoPtr->getDescription());
}

void PluginConfigDialog::availPluginDoubleClicked(QListWidgetItem* item)
{
    static_cast<void>(item);
    // TODO:
    assert(!"NYI: available double clicked");
}

void PluginConfigDialog::selectedPluginClicked(QListWidgetItem* item)
{
    static_cast<void>(item);
    // TODO:
    assert(!"NYI: selected clicked");
}

void PluginConfigDialog::addClicked()
{
    // TODO:
    assert(!"NYI: add button clicked");
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
    // TODO:
    assert(!"NYI: load clicked");
}

void PluginConfigDialog::saveClicked()
{
    // TODO:
    assert(!"NYI: save clicked");
}

void PluginConfigDialog::removeClicked()
{
    // TODO:
    assert(!"NYI: remove clicked");
}

void PluginConfigDialog::clearClicked()
{
    // TODO:
    assert(!"NYI: clear clicked");
}

void PluginConfigDialog::topClicked()
{
    // TODO:
    assert(!"NYI: top clicked");
}

void PluginConfigDialog::upClicked()
{
    // TODO:
    assert(!"NYI: up clicked");
}

void PluginConfigDialog::downClicked()
{
    // TODO:
    assert(!"NYI: down clicked");
}

void PluginConfigDialog::bottomClicked()
{
    // TODO:
    assert(!"NYI: bottom clicked");
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
    refreshAvailablePlugins();
    refreshAvailableToolbar();
    refreshSelectedToolbar();
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
        auto dataVar = curItem->data(Qt::UserRole);
        assert(dataVar.isValid());
        assert(dataVar.canConvert<PluginMgr::PluginInfoPtr>());
        curInfo = dataVar.value<PluginMgr::PluginInfoPtr>();
    }

    m_ui.m_availListWidget->clear();
    auto& availablePlugins = PluginMgr::instanceRef().getAvailablePlugins();
    auto filterStr = m_availSearchLineEdit->text();

    for (auto& pluginInfoPtr : availablePlugins) {
        auto& name = pluginInfoPtr->getName();
        bool addEntry =
            filterStr.isEmpty() ||
            name.contains(filterStr, Qt::CaseInsensitive);

        if (addEntry) {
            m_ui.m_availListWidget->addItem(name);
            auto* item = m_ui.m_availListWidget->item(
                m_ui.m_availListWidget->count() - 1);
            static const QString Tooltip("Use double click to select");
            item->setToolTip(Tooltip);

            item->setData(
                Qt::UserRole,
                QVariant::fromValue(pluginInfoPtr));
            }

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
    // TODO:
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

} /* namespace comms_champion */
