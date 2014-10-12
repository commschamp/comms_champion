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


#include "MsgMgr.h"

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QtQml>

#include <cassert>
#include <QtCore/QTimer>

namespace cc
{

namespace
{

QObject *getMsgMgr(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return MsgMgr::instance();
}

}  // namespace

MsgMgr* MsgMgr::instance()
{
    static cc::MsgMgr* mgr = new cc::MsgMgr();
    return mgr;
}

void MsgMgr::qmlRegister()
{
    qmlRegisterSingletonType<MsgMgr>("cc.MsgMgr", 1, 0, "MsgMgr", &getMsgMgr);
}

void MsgMgr::timeout()
{
    emit msgReceived(nullptr);
}

QString MsgMgr::name() const
{
    return "Hello";
}

void MsgMgr::setName(const QString& name)
{
    Q_UNUSED(name);
    emit nameChanged();
}

MsgMgr::MsgMgr(QObject* parent)
  : Base(parent)
{
    auto* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(2000);
}

}  // namespace cc
