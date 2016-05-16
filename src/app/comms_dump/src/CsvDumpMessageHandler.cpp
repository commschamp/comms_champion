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


#include "CsvDumpMessageHandler.h"

#include <iomanip>

#include "comms_champion/field_wrapper/FieldWrapperHandler.h"
#include "comms_champion/property/message.h"

namespace cc = comms_champion;

namespace comms_dump
{

namespace
{

const char Endl = '\n';

}  // namespace

class CsvDumpFieldsHandler : public cc::field_wrapper::FieldWrapperHandler
{
public:
    CsvDumpFieldsHandler(std::ostream& out, const std::string& sep)
      : m_out(out),
        m_sep(sep)
    {
    }

    virtual ~CsvDumpFieldsHandler() = default;

    virtual void handle(cc::field_wrapper::IntValueWrapper& wrapper) override
    {
        m_out << m_sep << wrapper.getValue();
    }

    virtual void handle(cc::field_wrapper::BitmaskValueWrapper& wrapper) override
    {
        m_out << m_sep << "0x" << std::setw(wrapper.length() * 2) <<
            std::setfill('0') << std::hex << wrapper.getValue() << std::dec;
    }

    virtual void handle(cc::field_wrapper::EnumValueWrapper& wrapper) override
    {
        m_out << m_sep << wrapper.getValue();
    }

    virtual void handle(cc::field_wrapper::StringWrapper& wrapper) override
    {
        m_out << m_sep << '\"' << wrapper.getValue().toStdString() << '\"' ;
    }

    virtual void handle(cc::field_wrapper::BitfieldWrapper& wrapper) override
    {
        auto& membersWrappers = wrapper.getMembers();
        for (auto& mem : membersWrappers) {
            mem->dispatch(*this);
        }
    }

    virtual void handle(cc::field_wrapper::OptionalWrapper& wrapper) override
    {
        auto mode = wrapper.getMode();
        if (mode == comms::field::OptionalMode::Missing) {
            return;
        }
        
        wrapper.getFieldWrapper().dispatch(*this);
    }

    virtual void handle(cc::field_wrapper::BundleWrapper& wrapper) override
    {
        auto& membersWrappers = wrapper.getMembers();
        for (auto& mem : membersWrappers) {
            mem->dispatch(*this);
        }
    }

    virtual void handle(cc::field_wrapper::ArrayListRawDataWrapper& wrapper) override
    {
        m_out << m_sep << '\"' << wrapper.getValue().toStdString() << '\"';
    }

    virtual void handle(cc::field_wrapper::ArrayListWrapper& wrapper) override
    {
        auto& members = wrapper.getMembers();
        if (!wrapper.hasFixedSize()) {
            m_out << m_sep << members.size();
        }

        for (auto& mem : members) {
            mem->dispatch(*this);
        }
    }

    virtual void handle(cc::field_wrapper::FloatValueWrapper& wrapper) override
    {
        m_out << m_sep << wrapper.getValue();
    }

    virtual void handle(cc::field_wrapper::UnknownValueWrapper& wrapper) override
    {
        m_out << m_sep << '\"' << wrapper.getSerialisedString().toStdString() << '\"';
    }

    virtual void handle(cc::field_wrapper::FieldWrapper& wrapper) override
    {
        static_cast<void>(wrapper);
        assert(!"Unexpected wrapper");
    }

private:
    std::ostream& m_out;
    std::string m_sep;
};

CsvDumpMessageHandler::CsvDumpMessageHandler(
    std::ostream& out,
    const std::string& sep)
  : m_out(out),
    m_sep(sep),
    m_fieldsDump(new CsvDumpFieldsHandler(out, sep))
{
}

CsvDumpMessageHandler::~CsvDumpMessageHandler() = default;


void CsvDumpMessageHandler::beginMsgHandlingImpl(cc::Message& msg)
{
    if (m_showType) {
        static const std::string DirMap[] = {
            "Unknown",
            "Received",
            "Sent"
        };

        static const auto DirMapSize =
                                std::extent<decltype(DirMap)>::value;

        static_assert(DirMapSize == static_cast<unsigned>(cc::Message::Type::NumOfValues),
            "The map above is incorrect");

        auto type = cc::property::message::Type().getFrom(msg);
        assert((type == cc::Message::Type::Sent) ||
               (type == cc::Message::Type::Received));

        if (DirMapSize <= static_cast<unsigned>(type)) {
            type = cc::Message::Type::Invalid;
        }

        m_out << DirMap[static_cast<unsigned>(type)] << m_sep;
    }

    auto timestamp = cc::property::message::Timestamp().getFrom(msg);
    if (timestamp != 0) {
        m_out << timestamp << m_sep;
    }

    m_out << msg.idAsString().toStdString();
}

void CsvDumpMessageHandler::addFieldImpl(FieldWrapperPtr wrapper)
{
    wrapper->dispatch(*m_fieldsDump);
}

void CsvDumpMessageHandler::endMsgHandlingImpl()
{
    m_out << Endl;
}

}  // namespace comms_dump

