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


#include "RawHexDataDialog.h"

#include <cassert>
#include <utility>
#include <vector>
#include <algorithm>
#include <iterator>

CC_DISABLE_WARNINGS()
#include <QtWidgets/QPushButton>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"

namespace comms_champion
{

RawHexDataDialog::RawHexDataDialog(
    MessagesList& msgs,
    ProtocolPtr protocol,
    QWidget* parentObj)
  : Base(parentObj),
    m_msgs(msgs),
    m_protocol(std::move(protocol))
{
    assert(m_protocol);
    m_ui.setupUi(this);

    connect(
        m_ui.m_rawDataText, SIGNAL(textChanged()),
        this, SLOT(valueChanged()));

    auto* resetButton = m_ui.m_buttonBox->button(QDialogButtonBox::Reset);
    assert(resetButton);
    connect(
        resetButton, SIGNAL(clicked()),
        this, SLOT(reset()));
}

RawHexDataDialog::~RawHexDataDialog() noexcept = default;

void RawHexDataDialog::valueChanged()
{
    typedef std::pair<QChar, QChar> CharsRange;
    static const CharsRange Ranges[] = {
        std::make_pair(QChar('0'), QChar('9')),
        std::make_pair(QChar('a'), QChar('f')),
        std::make_pair(QChar('A'), QChar('F')),
        std::make_pair(QChar(' '), QChar(' '))
    };

    auto str = m_ui.m_rawDataText->toPlainText();

    std::vector<QChar> invalidChars;
    invalidChars.reserve(str.size());
    std::copy_if(
        str.begin(), str.end(), std::back_inserter(invalidChars),
        [](QChar ch) -> bool
        {
            return std::none_of(
                std::begin(Ranges), std::end(Ranges),
                [ch](const CharsRange& range) -> bool
                {
                    return (range.first <= ch) && (ch <= range.second);
                });
        });

    invalidChars.erase(
        std::unique(invalidChars.begin(), invalidChars.end()),
        invalidChars.end());

    for (auto ch : invalidChars) {
        str.replace(ch, "");
    }

    if (m_ui.m_rawDataText->toPlainText() != str) {
        auto curs = m_ui.m_rawDataText->textCursor();
        auto newPosition = std::min(curs.position(), str.size());
        m_ui.m_rawDataText->setPlainText(str);
        curs.setPosition(newPosition);
        m_ui.m_rawDataText->setTextCursor(curs);
    }
}

void RawHexDataDialog::accept()
{
    auto str = m_ui.m_rawDataText->toPlainText();

    DataInfo dataInfo;
    dataInfo.m_timestamp = DataInfo::TimestampClock::now();
    dataInfo.m_data.reserve(str.size() / 2);

    QString numStr;
    auto addValueFunc =
        [&dataInfo, &numStr]()
        {
            bool ok = false;
            auto byte = static_cast<DataInfo::DataSeq::value_type>(numStr.toUInt(&ok, 16));
            static_cast<void>(ok);
            assert(ok);
            dataInfo.m_data.push_back(byte);
            numStr.clear();
        };

    for (auto ch : str) {
        if (ch.isSpace()) {
           if (!numStr.isEmpty()) {
               addValueFunc();
           }

           continue;
        }

        numStr.append(ch);
        if (numStr.size() < 2) {
            continue;
        }

        addValueFunc();
    }

    if (!numStr.isEmpty()) {
        addValueFunc();
    }

    if (!m_ui.m_convertCheckBox->isChecked()) {
        auto msg = m_protocol->createInvalidMessage(dataInfo.m_data);
        if (!msg) {
            assert(!"Invalid message was not created by the protocol");
            Base::accept();
            return;
        }

        m_msgs.push_back(std::move(msg));
        Base::accept();
        return;
    }

    m_msgs = m_protocol->read(dataInfo, true);
    Base::accept();
}

void RawHexDataDialog::reset()
{
    m_ui.m_rawDataText->clear();
}

}  // namespace comms_champion
