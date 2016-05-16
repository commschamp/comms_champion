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
    Filter() = default;
    virtual ~Filter() {}

    void feedInData(DataInfoPtr dataPtr)
    {
        feedInDataImpl(std::move(dataPtr));
    }


    typedef std::function<void (DataInfoPtr)> DataReceivedCallback;
    template <typename TFunc>
    void setDataReceivedCallback(TFunc&& func)
    {
        m_dataReceivedCallback = std::forward<TFunc>(func);
    }

    typedef std::function<void (const QString& msg)> ErrorReportCallback;
    template <typename TFunc>
    void setErrorReportCallback(TFunc&& func)
    {
        m_errorReportCallback = std::forward<TFunc>(func);
    }

protected:

    virtual void sendDataImpl(DataInfoPtr dataPtr) = 0;
    virtual void feedInDataImpl(DataInfoPtr dataPtr) = 0;

    void reportDataReceived(DataInfoPtr dataPtr)
    {
        if (m_dataReceivedCallback) {
            m_dataReceivedCallback(std::move(dataPtr));
        }
    }

    void reportError(const QString& msg)
    {
        if (m_errorReportCallback) {
            m_errorReportCallback(msg);
        }
    }

private:
    DataReceivedCallback m_dataReceivedCallback;
    ErrorReportCallback m_errorReportCallback;
};

typedef std::shared_ptr<Filter> FilterPtr;

}  // namespace comms_champion

