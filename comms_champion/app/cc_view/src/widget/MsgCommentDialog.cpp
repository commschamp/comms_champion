//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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


#include "MsgCommentDialog.h"

#include <cassert>
#include "comms_champion/property/message.h"

namespace comms_champion
{

MsgCommentDialog::MsgCommentDialog(
    MessagePtr msg,
    QWidget* parentObj)
  : Base(parentObj),
    m_msg(std::move(msg))
{
    m_ui.setupUi(this);
    m_ui.m_commentLineEdit->setText(property::message::Comment().getFrom(*m_msg));
}

void MsgCommentDialog::accept()
{
    property::message::Comment().setTo(m_ui.m_commentLineEdit->text(), *m_msg);
    Base::accept();
}

}  // namespace comms_champion


