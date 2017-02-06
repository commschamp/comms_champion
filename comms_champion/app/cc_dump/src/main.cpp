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

#include "comms_champion/Message.h"
#include "comms_champion/Protocol.h"
#include "comms_champion/PluginMgr.h"
#include "comms_champion/DataInfo.h"

#include "AppMgr.h"

namespace cc = comms_champion;

namespace
{

const QString PluginsOptStr("plugins");
const QString OutMsgsOptStr("msgs-to-send");
const QString InMsgsOptStr("received-msgs");
const QString LastWaitOptStr("last-wait");
const QString RecordSentOptStr("record-sent");
const QString QuietOptStr("quiet");

void metaTypesRegisterAll()
{
    qRegisterMetaType<cc::MessagePtr>();
    qRegisterMetaType<cc::ProtocolPtr>();
    qRegisterMetaType<cc::PluginMgr::PluginInfoPtr>();
    qRegisterMetaType<cc::DataInfoPtr>();
}

void prepareCommandLineOptions(QCommandLineParser& parser)
{
    parser.addHelpOption();

    QCommandLineOption pluginsOpt(
        QStringList() << "p" << PluginsOptStr,
        QCoreApplication::translate("main", "Provide plugins configuration file."),
        QCoreApplication::translate("main", "filename")
    );
    parser.addOption(pluginsOpt);

    QCommandLineOption outMsgsOpt(
        QStringList() << "s" << OutMsgsOptStr,
        QCoreApplication::translate("main", "Messages to send configuration file."),
        QCoreApplication::translate("main", "filename")
    );
    parser.addOption(outMsgsOpt);

    QCommandLineOption inMsgsOpt(
        QStringList() << "r" << InMsgsOptStr,
        QCoreApplication::translate("main", "Received messages storage file."),
        QCoreApplication::translate("main", "filename")
    );
    parser.addOption(inMsgsOpt);

    QCommandLineOption lastWaitOpt(
        QStringList() << "w" << LastWaitOptStr,
        QCoreApplication::translate("main", "Wait period (in milliseconds) from "
                                            "last sent message till dump termination. "
                                            "Default is 100 ms. 0 means infinite wait."),
        QCoreApplication::translate("main", "ms")
    );
    parser.addOption(lastWaitOpt);

    QCommandLineOption recordSentOpt(
        RecordSentOptStr,
        QCoreApplication::translate("main", "Record/Show sent messages as well.")
    );
    parser.addOption(recordSentOpt);

    QCommandLineOption quietOpt(
        QStringList() << "q" << QuietOptStr,
        QCoreApplication::translate("main", "Quiet mode, don't dump CSV output to stdout.")
    );
    parser.addOption(quietOpt);

}

}  // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    metaTypesRegisterAll();

    QCommandLineParser parser;
    prepareCommandLineOptions(parser);
    parser.process(app);

    if (!parser.isSet(PluginsOptStr)) {
        std::cerr << "ERROR: Unknown plugins configuration, please use \"--" <<
            PluginsOptStr.toStdString() << "\" option to provide the file.\n" << std::endl;
        return -1;
    }

    QDir dir(app.applicationDirPath());
    dir.cdUp();
    if (!dir.cd("plugin")) {
        std::cerr << "Failed to find plugin dir" << std::endl;
        return -1;
    }

    app.addLibraryPath(dir.path());

    auto config = comms_dump::AppMgr::Config();
    config.m_pluginsDir = dir.path();

    if (parser.isSet(PluginsOptStr)) {
        config.m_pluginConfigFile = parser.value(PluginsOptStr);
    }

    if (parser.isSet(OutMsgsOptStr)) {
        config.m_outMsgsFile = parser.value(OutMsgsOptStr);
    }

    if (parser.isSet(InMsgsOptStr)) {
        config.m_inMsgsFile = parser.value(InMsgsOptStr);
    }

    config.m_lastWait = 100;
    if (parser.isSet(LastWaitOptStr)) {
        auto valueStr = parser.value(LastWaitOptStr);
        bool ok = false;
        unsigned value = valueStr.toUInt(&ok);
        if (ok) {
            config.m_lastWait = value;
        }
    }

    if (parser.isSet(RecordSentOptStr)) {
        config.m_recordOutgoing = true;
    }

    if (parser.isSet(QuietOptStr)) {
        config.m_quiet = true;
    }

    comms_dump::AppMgr appMgr;
    if (!appMgr.start(config)) {
        std::cerr << "Failed to start!" << std::endl;
        return -1;
    }

    auto retval = app.exec();
    return retval;
}

