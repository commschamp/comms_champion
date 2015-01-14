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


#include "MessageUpdateDialog.h"

#include <cassert>
#include <limits>
#include <type_traits>
#include <algorithm>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>

#include "DefaultMessageDisplayWidget.h"
#include "GlobalConstants.h"

namespace comms_champion
{

namespace
{

QString getMessageNameForList(MessageInfoPtr msgInfo)
{
    auto msgPtr = msgInfo->getAppMessage();
    return QString("(%1) %2").arg(msgPtr->idAsString()).arg(msgPtr->name());
}

enum class Duration
{
    Milliseconds,
    Seconds,
    Minutes,
    Hours,
    Days,
    NumOfDurations
};

void fillDurationComboBox(QComboBox& box)
{
    static const QString Strings[] = {
        "millisecond(s)",
        "second(s)",
        "minute(s)",
        "hour(s)",
        "day(s)"
    };

    static_assert(
        std::extent<decltype(Strings)>::value == (std::size_t)Duration::NumOfDurations,
        "Incorrect mapping of strings.");

    for (auto s : Strings) {
        box.addItem(s);
    }
}

long long unsigned durationToMs(int value, Duration dur)
{
    static const long long unsigned Mul[] = {
        1UL,
        1UL * 1000,
        1UL * 1000 * 60,
        1UL * 1000 * 60 * 60,
        1UL * 1000 * 60 * 60 * 24
    };

    static_assert(
        std::extent<decltype(Mul)>::value == static_cast<long long unsigned>(Duration::NumOfDurations),
        "Incorrect mapping.");

    if (Duration::NumOfDurations <= dur) {
        assert(!"Incorrert duration");
        return static_cast<long long unsigned>(value);
    }

    return static_cast<long long unsigned>(Mul[static_cast<std::size_t>(dur)] * value);
}

}  // namespace

MessageUpdateDialog::MessageUpdateDialog(
    MessageInfoPtr& msgInfo,
    ProtocolPtr protocol,
    QWidget* parent)
  : Base(parent),
    m_msgInfo(msgInfo),
    m_protocol(std::move(protocol)),
    m_allMsgs(m_protocol->createAllMessages()),
    m_msgDisplayWidget(new DefaultMessageDisplayWidget())
{
    m_msgDisplayWidget->setEditEnabled(true);

    m_ui.setupUi(this);
    assert(m_ui.m_delayUnitsComboBox != nullptr);
    fillDurationComboBox(*m_ui.m_delayUnitsComboBox);
    assert(m_ui.m_repeatUnitsComboBox != nullptr);
    fillDurationComboBox(*m_ui.m_repeatUnitsComboBox);

    assert(m_ui.m_msgDetailsWidget);
    m_ui.m_msgDetailsWidget->setLayout(new QVBoxLayout());
    m_ui.m_msgDetailsWidget->layout()->addWidget(m_msgDisplayWidget);

    m_ui.m_delaySpinBox->setRange(0, std::numeric_limits<int>::max());
    m_ui.m_repeatSpinBox->setRange(0, std::numeric_limits<int>::max());
    m_ui.m_repeatCountSpinBox->setRange(0, std::numeric_limits<int>::max());

    refreshDisplayedList(m_ui.m_searchLineEdit->text());
    refreshDelayInfo(m_ui.m_delayCheckBox->checkState());
    refreshRepeatInfo(m_ui.m_repeatCheckBox->checkState());
    refreshButtons();

    assert(parent);
    auto newHeight = std::max(height(), (parent->height() * 9) / 10);
    auto newWidth = std::max(width(), (parent->width() * 7) / 10);
    resize(QSize(newWidth, newHeight));

    connect(
        m_msgDisplayWidget, SIGNAL(sigMsgUpdated()),
        this, SLOT(msgUpdated()));

    connect(
        m_ui.m_msgListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(itemClicked(QListWidgetItem*)));

    connect(
        m_ui.m_searchLineEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(refreshDisplayedList(const QString&)));

    connect(
        m_ui.m_clearSearchToolButton, SIGNAL(clicked()),
        m_ui.m_searchLineEdit, SLOT(clear()));

    connect(
        m_ui.m_delayCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(refreshDelayInfo(int)));

    connect(
        m_ui.m_delaySpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(delayUpdated(int)));

    connect(
        m_ui.m_repeatCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(refreshRepeatInfo(int)));

    connect(
        m_ui.m_repeatSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(repeatDurationUpdated(int)));

    connect(
        m_ui.m_repeatCountSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(repeatCountUpdated(int)));

    connect(
        m_ui.m_indefinitelyCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(indefinitelyUpdated(int)));

    auto* resetButton = m_ui.m_buttonBox->button(QDialogButtonBox::Reset);
    assert(resetButton);
    connect(
        resetButton, SIGNAL(clicked()),
        this, SLOT(reset()));
}

void MessageUpdateDialog::msgUpdated()
{
    auto* item = m_ui.m_msgListWidget->currentItem();
    auto msgInfoVar = item->data(Qt::UserRole);
    assert(msgInfoVar.isValid());
    assert(msgInfoVar.canConvert<MessageInfoPtr>());
    auto msgInfo = msgInfoVar.value<MessageInfoPtr>();

    assert(m_protocol);
    assert(msgInfo);
    m_protocol->updateMessageInfo(*msgInfo);
    assert(m_msgDisplayWidget);

    // Direct invocation of m_msgDisplayWidget->displayMessage(std::move(msgInfo))
    // in place here causes SIGSEGV. No idea why.
    QMetaObject::invokeMethod(
        this,
        "displayMessagePostponed",
        Qt::QueuedConnection,
        Q_ARG(comms_champion::MessageInfoPtr, std::move(msgInfo)));
}

void MessageUpdateDialog::itemClicked(QListWidgetItem* item)
{
    auto msgInfo = getMsgFromItem(item);
    assert(msgInfo);

    m_msgDisplayWidget->displayMessage(std::move(msgInfo));
    refreshButtons();
}

void MessageUpdateDialog::displayMessagePostponed(MessageInfoPtr msgInfo)
{
    m_msgDisplayWidget->displayMessage(std::move(msgInfo));
}

void MessageUpdateDialog::refreshDisplayedList(const QString& searchText)
{
    MessageInfoPtr selectedInfo;
    if (0 <= m_ui.m_msgListWidget->currentRow()) {
        selectedInfo = getMsgFromItem(m_ui.m_msgListWidget->currentItem());
    }

    m_ui.m_msgListWidget->clear();

    for (auto& msgInfo : m_allMsgs) {
        auto msgName = getMessageNameForList(msgInfo);
        if (searchText.isEmpty() || msgName.contains(searchText)) {
            m_ui.m_msgListWidget->addItem(msgName);
            auto* item = m_ui.m_msgListWidget->item(m_ui.m_msgListWidget->count() - 1);
            item->setData(
                Qt::UserRole,
                QVariant::fromValue(msgInfo));

            if (selectedInfo && (msgInfo.get() == selectedInfo.get())) {
                m_ui.m_msgListWidget->setCurrentItem(item);
            }
        }
    }

    refreshButtons();

    if (m_ui.m_msgListWidget->currentRow() < 0) {
        m_msgDisplayWidget->clear();
    }
}

void MessageUpdateDialog::refreshDelayInfo(int checkboxValue)
{
    bool checked = checkboxValue != Qt::Unchecked;
    if (!checked) {
        m_ui.m_delaySpinBox->setMinimum(0);
        m_ui.m_delaySpinBox->setValue(DisabledDelayValue);
        m_ui.m_delaySpinBox->setEnabled(false);

        m_ui.m_delayUnitsComboBox->setEnabled(false);
        return;
    }

    m_ui.m_delaySpinBox->setValue(m_prevDelay);
    m_ui.m_delaySpinBox->setMinimum(1);
    m_ui.m_delaySpinBox->setEnabled(true);

    m_ui.m_delayUnitsComboBox->setEnabled(true);
}

void MessageUpdateDialog::delayUpdated(int value)
{
    if (m_ui.m_delayCheckBox->checkState() == Qt::Checked) {
        m_prevDelay = value;
    }
}

void MessageUpdateDialog::refreshRepeatInfo(int checkboxValue)
{
    bool checked = checkboxValue != Qt::Unchecked;
    if (!checked) {
        m_ui.m_repeatSpinBox->setMinimum(0);
        m_ui.m_repeatSpinBox->setValue(DisabledRepeatDuration);
        m_ui.m_repeatSpinBox->setEnabled(false);

        m_ui.m_repeatUnitsComboBox->setEnabled(false);

        m_ui.m_repeatCountSpinBox->setValue(DisabledRepeatCount);
        m_ui.m_repeatCountSpinBox->setEnabled(false);

        m_ui.m_indefinitelyCheckBox->setCheckState(DisabledSendIndefinitelyState);
        m_ui.m_indefinitelyCheckBox->setEnabled(false);
        return;
    }

    m_ui.m_repeatSpinBox->setValue(m_prevRepeatDuration);
    m_ui.m_repeatSpinBox->setMinimum(1);
    m_ui.m_repeatSpinBox->setEnabled(true);

    m_ui.m_repeatUnitsComboBox->setEnabled(true);

    m_ui.m_indefinitelyCheckBox->setCheckState(m_sendIndefinitelyState);
    m_ui.m_indefinitelyCheckBox->setEnabled(true);

    if (m_sendIndefinitelyState == Qt::Unchecked) {
        m_ui.m_repeatCountSpinBox->setValue(m_prevRepeatCount);
        m_ui.m_repeatCountSpinBox->setMinimum(1);
        m_ui.m_repeatCountSpinBox->setEnabled(true);
    }
    else {
        m_ui.m_repeatCountSpinBox->setMinimum(0);
        m_ui.m_repeatCountSpinBox->setValue(0);
        m_ui.m_repeatCountSpinBox->setEnabled(false);
    }
}

void MessageUpdateDialog::repeatDurationUpdated(int value)
{
    if (m_ui.m_repeatCheckBox->checkState() == Qt::Checked) {
        m_prevRepeatDuration = value;
    }
}

void MessageUpdateDialog::repeatCountUpdated(int value)
{
    if ((m_ui.m_repeatCheckBox->checkState() == Qt::Checked) &&
        (m_ui.m_indefinitelyCheckBox->checkState() == Qt::Unchecked)) {
        m_prevRepeatCount = value;
    }
}

void MessageUpdateDialog::indefinitelyUpdated(int checkboxValue)
{
    auto repeatState = m_ui.m_repeatCheckBox->checkState();
    if (repeatState == Qt::Checked) {
        m_sendIndefinitelyState = static_cast<decltype(m_sendIndefinitelyState)>(checkboxValue);
        refreshRepeatInfo(m_ui.m_repeatCheckBox->checkState());
    }
}

void MessageUpdateDialog::accept()
{
    m_msgInfo = getMsgFromItem(m_ui.m_msgListWidget->currentItem());
    assert(m_msgInfo);

    m_msgInfo->setExtraProperty(
        GlobalConstants::msgDelayPropertyName(),
        QVariant::fromValue(
            durationToMs(
                m_ui.m_delaySpinBox->value(),
                static_cast<Duration>(m_ui.m_delayUnitsComboBox->currentIndex()))));
    m_msgInfo->setExtraProperty(
        GlobalConstants::msgRepeatDurationPropertyName(),
        QVariant::fromValue(
            durationToMs(
                m_ui.m_repeatSpinBox->value(),
                static_cast<Duration>(m_ui.m_repeatUnitsComboBox->currentIndex()))));
    m_msgInfo->setExtraProperty(
        GlobalConstants::msgRepeatCountPropertyName(),
        QVariant::fromValue(m_ui.m_repeatCountSpinBox->value()));
    Base::accept();
}

void MessageUpdateDialog::reset()
{
    auto msgInfo = getMsgFromItem(m_ui.m_msgListWidget->currentItem());
    assert(msgInfo);
    auto msgPtr = msgInfo->getAppMessage();
    assert(msgPtr);
    msgPtr->reset();
    m_protocol->updateMessageInfo(*msgInfo);
    assert(m_msgDisplayWidget);
    m_msgDisplayWidget->displayMessage(std::move(msgInfo));
}

MessageInfoPtr MessageUpdateDialog::getMsgFromItem(QListWidgetItem* item)
{
    assert(item);
    auto var = item->data(Qt::UserRole);
    assert(var.canConvert<MessageInfoPtr>());
    return var.value<MessageInfoPtr>();
}

void MessageUpdateDialog::refreshButtons()
{
    auto msgSelected = (0 <= m_ui.m_msgListWidget->currentRow());

    auto setButtonEnabledFunc =
        [this](QDialogButtonBox::StandardButton buttonType, bool enabled)
        {
            auto* button = m_ui.m_buttonBox->button(buttonType);
            assert(button);
            button->setEnabled(enabled);
        };

    setButtonEnabledFunc(QDialogButtonBox::Ok, msgSelected);
    setButtonEnabledFunc(QDialogButtonBox::Reset, msgSelected);
}

}  // namespace comms_champion


