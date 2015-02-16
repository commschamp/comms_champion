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
}

void PluginConfigDialog::createAvailableToolbar()
{
    std::unique_ptr<QToolBar> toolbar(new QToolBar());
    auto* pluginAddAction =
        toolbar->addAction(icon::add(), "Select this plugin");
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel(tr("Search:")));
    toolbar->addWidget(m_availSearchLineEdit);
    auto* clearSearchAction =
        toolbar->addAction(icon::clear(), "Clear search");

    m_ui.m_availLayout->insertWidget(0, toolbar.release());

    // TODO: connect signals
    static_cast<void>(pluginAddAction);
    static_cast<void>(clearSearchAction);
}

void PluginConfigDialog::createSelectedToolbar()
{
    std::unique_ptr<QToolBar> toolbar(new QToolBar());
    auto* loadAction =
        toolbar->addAction(icon::upload(), "Load list of plugins");
    auto* saveAction =
        toolbar->addAction(icon::saveAs(), "Save list of plugins");
    toolbar->addSeparator();
    auto* removeAction =
        toolbar->addAction(icon::remove(), "Remove this plugin from selected list");
    auto* clearAction =
        toolbar->addAction(icon::editClear(), "Clear selected plugins list");
    toolbar->addSeparator();
    auto* topAction =
        toolbar->addAction(icon::top(), "Move selected plugin to the top of the list");
    auto* upAction =
        toolbar->addAction(icon::up(), "Move selected plugin up");
    auto* downAction =
        toolbar->addAction(icon::down(), "Move selected plugin down");
    auto* bottomAction =
        toolbar->addAction(icon::bottom(), "Move selected plugin to the bottom of the list");

    m_ui.m_selectedLayout->insertWidget(0, toolbar.release());

    // TODO: connect signals
    static_cast<void>(loadAction);
    static_cast<void>(saveAction);
    static_cast<void>(removeAction);
    static_cast<void>(clearAction);
    static_cast<void>(topAction);
    static_cast<void>(upAction);
    static_cast<void>(downAction);
    static_cast<void>(bottomAction);
}

} /* namespace comms_champion */
