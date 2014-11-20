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

#include "RecvAreaToolBar.h"

#include <QtCore/QObject>
#include <QtWidgets/QAction>
#include <QtGui/QIcon>

#include "GuiAppMgr.h"

namespace comms_champion
{

namespace
{

const QIcon& startIcon()
{
    static const QIcon Icon(":/image/start.png");
    return Icon;
}

const QIcon& stopIcon()
{
    static const QIcon Icon(":/image/stop.png");
    return Icon;
}

const QIcon& saveIcon()
{
    static const QIcon Icon(":/image/save.png");
    return Icon;
}

const QString StartTooltip("Start Reception");
const QString SaveTooltip("Save Messages");

QAction* createStartButton(QToolBar& bar)
{
    auto* action = bar.addAction(startIcon(), StartTooltip);
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvStartClicked()));
    return action;
}

QAction* createSaveButton(QToolBar& bar)
{
    auto* action = bar.addAction(saveIcon(), SaveTooltip);
    QObject::connect(action, SIGNAL(triggered()),
                     GuiAppMgr::instance(), SLOT(recvSaveClicked()));

    return action;
}

}  // namespace

RecvAreaToolBar::RecvAreaToolBar(QWidget* parent)
  : Base(parent)
{
    m_startStopAction = createStartButton(*this);
    m_saveAction = createSaveButton(*this);
}

}  // namespace comms_champion

