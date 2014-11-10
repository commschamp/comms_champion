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

#include <memory>

#include <QtQml/QtQml>

#include "WidgetProxy.h"

#include <cassert>

namespace comms_champion
{

void WidgetProxy::qmlRegister()
{
    qmlRegisterType<WidgetProxy>("cc.WidgetProxy", 1, 0, "WidgetProxy");
}

void WidgetProxy::paint(QPainter *painter)
{
    if (m_widget != nullptr) {
        m_widget->render(painter);
    }
}

void WidgetProxy::updateWidget(QWidget* widget)
{
    setContentsSize(QSize(width(), height()));
    m_widget = widget;
    m_widget->setMinimumWidth(width());
    m_widget->setMinimumHeight(height());
    m_widget->adjustSize();
    update();
}

}  // namespace comms_champion

