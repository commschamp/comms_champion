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

#include <iostream>

#include <QtWidgets/QApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QDir>

#include "comms_champion/comms_champion.h"
#include "GuiAppMgr.h"
#include "GlobalConstants.h"
#include "ConfigMgr.h"
#include "PluginMgr.h"

#include "widget/MainWindowWidget.h"


namespace
{

namespace cc = comms_champion;

void metaTypesRegisterAll()
{
    qRegisterMetaType<cc::MessageInfoPtr>();
    qRegisterMetaType<cc::ProtocolPtr>();
    qRegisterMetaType<cc::PluginMgr::PluginInfoPtr>();
}

void initSingletons()
{
    static_cast<void>(cc::MsgMgr::instanceRef());
    static_cast<void>(cc::GuiAppMgr::instance());
    static_cast<void>(cc::ConfigMgr::instanceRef());
    static_cast<void>(cc::PluginMgr::instanceRef());
}

}  // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    metaTypesRegisterAll();
    initSingletons();

    cc::MainWindowWidget window;
    window.showMaximized();

    QDir dir(app.applicationDirPath());
    dir.cdUp();
    if (!dir.cd("plugin")) {
        std::cerr << "Failed to find plugin dir" << std::endl;
        return -1;
    }

    app.addLibraryPath(dir.path());
    cc::PluginMgr::instanceRef().setPluginsDir(dir.path());

    // TODO: use PluginMgr

//    QPluginLoader loader("demo");
//    auto* pluginObj = qobject_cast<cc::Plugin*>(loader.instance());
//    if (pluginObj == nullptr) {
//        std::cerr << "Failed to load plugin: " << loader.errorString().toStdString() << std::endl;
//        return -1;
//    }
//
//    pluginObj->initialize();
//    cc::PluginControlInterface controlInterface;
//    pluginObj->apply(controlInterface);
//    auto& msgMgr = cc::MsgMgr::instanceRef();
//    msgMgr.addSocket(cc::makeDummySocket());
//    msgMgr.start();
    auto retval = app.exec();
//    pluginObj->finalize();
    return retval;
}

