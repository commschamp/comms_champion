//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QApplication>
#include <QtCore/QPluginLoader>
#include <QtCore/QDir>
#include <QtCore/QCommandLineParser>
#include <QtCore/QStringList>
CC_ENABLE_WARNINGS()

#include "comms_champion/comms_champion.h"

namespace
{

const QString PluginsOptStr("plugins");

void prepareCommandLineOptions(QCommandLineParser& parser)
{
    parser.addHelpOption();

    QCommandLineOption pluginsOpt(
        QStringList() << "p" << PluginsOptStr,
        QCoreApplication::translate("main", "Provide plugins configuration file."),
        QCoreApplication::translate("main", "plugins_config_file")
    );
    parser.addOption(pluginsOpt);
}

}  // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    metaTypesRegisterAll();
//    initSingletons();

    QCommandLineParser parser;
    prepareCommandLineOptions(parser);
    parser.process(app);

    if (!parser.isSet(PluginsOptStr)) {
        std::cerr << "ERROR: Unknown plugins configuration, please use \"--" <<
            PluginsOptStr.toStdString() << "\" option to provide the file.\n" << std::endl;
        return -1;
    }

    std::cout << "Plugins file is: " << parser.value(PluginsOptStr).toStdString() << std::endl;

    QDir dir(app.applicationDirPath());
    dir.cdUp();
    if (!dir.cd("plugin")) {
        std::cerr << "Failed to find plugin dir" << std::endl;
        return -1;
    }

    app.addLibraryPath(dir.path());

//    auto& pluginMgr = cc::PluginMgrG::instanceRef();
//    pluginMgr.setPluginsDir(dir.path());
//
//    if (parser.isSet(CleanOptStr)) {
//        pluginMgr.clean();
//    }
//
//    pluginMgr.start();

    auto retval = app.exec();
    return retval;
}


