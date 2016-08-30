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


#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
CC_ENABLE_WARNINGS()

#include "DataInfo.h"
#include "Api.h"

namespace comms_champion
{

class CC_API Filter : public QObject
{
public:
    Filter();
    virtual ~Filter();

    bool start();

    void stop();

    DataInfoPtr recvData(DataInfoPtr dataPtr);

    DataInfoPtr sendData(DataInfoPtr dataPtr);

    typedef std::function<void (DataInfoPtr)> DataToSendCallback;
    template <typename TFunc>
    void setDataToSendCallback(TFunc&& func)
    {
        m_dataToSendCallback = std::forward<TFunc>(func);
    }

    typedef std::function<void (const QString& msg)> ErrorReportCallback;
    template <typename TFunc>
    void setErrorReportCallback(TFunc&& func)
    {
        m_errorReportCallback = std::forward<TFunc>(func);
    }

protected:
    virtual bool startImpl();
    virtual void stopImpl();
    virtual DataInfoPtr recvDataImpl(DataInfoPtr dataPtr) = 0;
    virtual DataInfoPtr sendDataImpl(DataInfoPtr dataPtr) = 0;

    void reportDataToSend(DataInfoPtr dataPtr);

    void reportError(const QString& msg);

private:
    DataToSendCallback m_dataToSendCallback;
    ErrorReportCallback m_errorReportCallback;
};

typedef std::shared_ptr<Filter> FilterPtr;

}  // namespace comms_champion

