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
#include "DummySocket.h"

#include "widget/MainWindowWidget.h"


namespace
{

namespace cc = comms_champion;

void metaTypesRegisterAll()
{
    qRegisterMetaType<cc::MessageInfoPtr>();
    qRegisterMetaType<cc::ProtocolPtr>();
}

}  // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    cc::MainWindowWidget window;
    window.showMaximized();

    QDir dir(app.applicationDirPath());
    dir.cdUp();
    if (!dir.cd("plugin")) {
        std::cerr << "Failed to find plugin dir" << std::endl;
        return -1;
    }

    app.addLibraryPath(dir.path());

    QPluginLoader loader("demo");
    auto* pluginObj = qobject_cast<cc::ProtocolPlugin*>(loader.instance());
    if (pluginObj == nullptr) {
        std::cerr << "Failed to load plugin: " << loader.errorString().toStdString() << std::endl;
        return -1;
    }

    pluginObj->initialize();
    pluginObj->configure();
    auto* msgMgr = cc::MsgMgr::instance();
    msgMgr->setProtocol(pluginObj->alloc());
    msgMgr->addSocket(cc::makeDummySocket());
    auto retval = app.exec();
    pluginObj->finalize();
    return retval;
}

