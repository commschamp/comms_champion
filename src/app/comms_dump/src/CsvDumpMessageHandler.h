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


#pragma once

#include <iostream>
#include <memory>

#include "comms_champion/MessageHandler.h"

namespace comms_dump
{

class CsvDumpFieldsHandler;
class CsvDumpMessageHandler : public comms_champion::MessageHandler
{
public:
    CsvDumpMessageHandler(
        std::ostream& out,
        const std::string& sep);

    virtual ~CsvDumpMessageHandler();

    std::ostream& outStream()
    {
        return m_out;
    }

    void setShowType(bool enabled)
    {
        m_showType = enabled;
    }

protected:
    virtual void beginMsgHandlingImpl(comms_champion::Message& msg) override;
    virtual void addFieldImpl(FieldWrapperPtr wrapper) override;
    virtual void endMsgHandlingImpl() override;

private:
    std::ostream& m_out;
    std::string m_sep;
    std::unique_ptr<CsvDumpFieldsHandler> m_fieldsDump;
    bool m_showType = false;
};

}  // namespace comms_dump


