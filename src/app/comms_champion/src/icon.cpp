//
// Copyright 2015 - 2016 (C). Alex Robenko. All rights reserved.
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


#include "icon.h"

namespace comms_champion
{

namespace icon
{

const QIcon& appIcon()
{
    static const QIcon IconObj(":/image/app_icon.png");
    return IconObj;
}

const QIcon& add()
{
    static const QIcon iconObj(":/image/add.png");
    return iconObj;
}

const QIcon& remove()
{
    static const QIcon iconObj(":/image/delete.png");
    return iconObj;
}

const QIcon& clear()
{
    static const QIcon iconObj(":/image/clear.png");
    return iconObj;
}

const QIcon& editClear()
{
    static const QIcon iconObj(":/image/edit-clear.png");
    return iconObj;
}

const QIcon& top()
{
    static const QIcon iconObj(":/image/top.png");
    return iconObj;
}

const QIcon& up()
{
    static const QIcon iconObj(":/image/up.png");
    return iconObj;
}

const QIcon& down()
{
    static const QIcon iconObj(":/image/down.png");
    return iconObj;
}

const QIcon& bottom()
{
    static const QIcon iconObj(":/image/bottom.png");
    return iconObj;
}

const QIcon& upload()
{
    static const QIcon iconObj(":/image/upload.png");
    return iconObj;
}

const QIcon& save()
{
    static const QIcon iconObj(":/image/save.png");
    return iconObj;
}

const QIcon& saveAs()
{
    static const QIcon iconObj(":/image/save_as.png");
    return iconObj;
}

const QIcon& start()
{
    static const QIcon iconObj(":/image/start.png");
    return iconObj;
}

const QIcon& startAll()
{
    static const QIcon iconObj(":/image/start_all.png");
    return iconObj;
}

const QIcon& stop()
{
    static const QIcon iconObj(":/image/stop.png");
    return iconObj;
}

const QIcon& edit()
{
    static const QIcon iconObj(":/image/edit.png");
    return iconObj;
}

const QIcon& showRecv()
{
    static const QIcon iconObj(":/image/msg_recv.png");
    return iconObj;
}

const QIcon& showSent()
{
    static const QIcon iconObj(":/image/msg_send.png");
    return iconObj;
}

const QIcon& pluginEdit()
{
    static const QIcon iconObj(":/image/plugin_edit.png");
    return iconObj;
}

const QIcon& errorLog()
{
    static const QIcon iconObj(":/image/error_log.png");
    return iconObj;
}

}  // namespace icon

}  // namespace comms_champion

