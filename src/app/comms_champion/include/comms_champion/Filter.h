//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QObject>
CC_ENABLE_WARNINGS()

#include "DataInfo.h"

namespace comms_champion
{

class Filter : public QObject
{
    Q_OBJECT

public:
    Filter() = default;
    virtual ~Filter() {}

public slots:

    void feedInData(DataInfoPtr dataPtr)
    {
        feedInDataImpl(std::move(dataPtr));
    }

signals:
    void sigDataToSend(DataInfoPtr dataPtr);
    void sigErrorReport(const QString& msg);

protected:

    virtual void sendDataImpl(DataInfoPtr dataPtr) = 0;
    virtual void feedInDataImpl(DataInfoPtr dataPtr) = 0;

    void reportDataToSend(DataInfoPtr dataPtr)
    {
        emit sigDataToSend(std::move(dataPtr));
    }

    void reportError(const QString& msg)
    {
        emit sigErrorReport(msg);
    }
};

typedef std::shared_ptr<Filter> FilterPtr;

}  // namespace comms_champion

