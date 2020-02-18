//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtGui/QIcon>
CC_ENABLE_WARNINGS()

namespace comms_champion
{

namespace icon
{

const QIcon& appIcon();
const QIcon& add();
const QIcon& addRaw();
const QIcon& remove();
const QIcon& clear();
const QIcon& editClear();
const QIcon& top();
const QIcon& up();
const QIcon& down();
const QIcon& bottom();
const QIcon& upload();
const QIcon& save();
const QIcon& saveAs();
const QIcon& start();
const QIcon& startAll();
const QIcon& stop();
const QIcon& edit();
const QIcon& showRecv();
const QIcon& showSent();
const QIcon& pluginEdit();
const QIcon& errorLog();
const QIcon& connect();
const QIcon& disconnect();
const QIcon& comment();
const QIcon& dup();

}  // namespace icon

}  // namespace comms_champion




