//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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

#include <iostream>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCommandLineParser>
#include <QtCore/QStringList>
CC_ENABLE_WARNINGS()

#include "comms_champion/comms_champion.h"
#include "PluginMgrG.h"
#include "GuiAppMgr.h"
#include "GlobalConstants.h"

#include "widget/MainWindowWidget.h"
#include "icon.h"

namespace cc = comms_champion;

namespace
{

const QString CleanOptStr("clean");

void metaTypesRegisterAll()
{
    qRegisterMetaType<cc::MessageInfoPtr>();
    qRegisterMetaType<cc::ProtocolPtr>();
    qRegisterMetaType<cc::GuiAppMgr::ActionPtr>();
    qRegisterMetaType<cc::PluginMgr::PluginInfoPtr>();
    qRegisterMetaType<cc::DataInfoPtr>();
}

void initSingletons()
{
    static_cast<void>(cc::PluginMgrG::instanceRef());
    static_cast<void>(cc::MsgMgrG::instanceRef());
    static_cast<void>(cc::GuiAppMgr::instance());
}

void prepareCommandLineOptions(QCommandLineParser& parser)
{
    parser.addHelpOption();

    QCommandLineOption cleanOpt(
        QStringList() << "c" << CleanOptStr,
        QCoreApplication::translate("main", "Clean start.")
    );
    parser.addOption(cleanOpt);
}

}  // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    metaTypesRegisterAll();
    initSingletons();

    QCommandLineParser parser;
    prepareCommandLineOptions(parser);
    parser.process(app);

    cc::MainWindowWidget window;
    window.setWindowIcon(cc::icon::appIcon());
    window.showMaximized();

    QDir dir(app.applicationDirPath());
    dir.cdUp();
    if (!dir.cd("plugin")) {
        std::cerr << "Failed to find plugin dir" << std::endl;
        return -1;
    }

    app.addLibraryPath(dir.path());

    auto& pluginMgr = cc::PluginMgrG::instanceRef();
    pluginMgr.setPluginsDir(dir.path());

    auto& guiAppMgr = cc::GuiAppMgr::instanceRef();
    if (parser.isSet(CleanOptStr)) {
        guiAppMgr.clean();
    }

    guiAppMgr.start();

    QObject::connect(
        &app, &QCoreApplication::aboutToQuit,
        []()
        {
            cc::MsgMgrG::instanceRef().deleteAllMsgs();
        });

    auto retval = app.exec();
    return retval;
}

