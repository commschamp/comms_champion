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


#include "MessageUpdateDialog.h"

#include <cassert>
#include <limits>
#include <type_traits>
#include <algorithm>
#include <map>
#include <utility>

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
CC_ENABLE_WARNINGS()

#include "comms_champion/property/message.h"
#include "DefaultMessageDisplayWidget.h"

namespace comms_champion
{

namespace
{

QString getMessageNameForList(MessagePtr msg)
{
    assert(msg);
    return QString("(%1) %2").arg(msg->idAsString()).arg(msg->name());
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

const QString& durationToString(Duration value)
{
    static const QString Map[] = {
        "millisec",
        "sec",
        "min",
        "hours",
        "days"
    };

    static const std::size_t MapSize = std::extent<decltype(Map)>::value;

    static_assert(MapSize == static_cast<unsigned>(Duration::NumOfDurations),
        "Incorrect map");

    auto castedValue = static_cast<unsigned>(value);
    if (MapSize <= castedValue) {
        castedValue = 0;
    }

    return Map[castedValue];
}

Duration stringToDuration(const QString& value)
{
    static const std::map<QString, Duration> Map = {
        std::make_pair("milliseconds", Duration::Milliseconds),
        std::make_pair("millisecond", Duration::Milliseconds),
        std::make_pair("millisec", Duration::Milliseconds),
        std::make_pair("ms", Duration::Milliseconds),
        std::make_pair("seconds", Duration::Seconds),
        std::make_pair("second", Duration::Seconds),
        std::make_pair("sec", Duration::Seconds),
        std::make_pair("s", Duration::Seconds),
        std::make_pair("minutes", Duration::Minutes),
        std::make_pair("minute", Duration::Minutes),
        std::make_pair("min", Duration::Minutes),
        std::make_pair("m", Duration::Minutes),
        std::make_pair("hours", Duration::Hours),
        std::make_pair("hour", Duration::Hours),
        std::make_pair("h", Duration::Hours),
        std::make_pair("days", Duration::Days),
        std::make_pair("day", Duration::Days),
        std::make_pair("d", Duration::Days)
    };

    auto iter = Map.find(value);
    if (iter == Map.end()) {
        return Duration::Milliseconds;
    }

    return iter->second;
}

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

const long long unsigned DurationMul[] = {
    1UL,
    1UL * 1000,
    1UL * 1000 * 60,
    1UL * 1000 * 60 * 60,
    1UL * 1000 * 60 * 60 * 24
};

static_assert(
    std::extent<decltype(DurationMul)>::value == static_cast<long long unsigned>(Duration::NumOfDurations),
    "Incorrect mapping.");

long long unsigned durationToMs(int value, Duration dur)
{

    if (Duration::NumOfDurations <= dur) {
        assert(!"Incorrert duration");
        return static_cast<long long unsigned>(value);
    }

    return static_cast<long long unsigned>(DurationMul[static_cast<std::size_t>(dur)] * value);
}

int msToDurationUnits(long long unsigned value, Duration dur)
{

    if (Duration::NumOfDurations <= dur) {
        assert(!"Incorrert duration");
        return static_cast<int>(value);
    }

    return static_cast<int>(value / DurationMul[static_cast<std::size_t>(dur)]);
}

}  // namespace

MessageUpdateDialog::MessageUpdateDialog(
    MessagePtr& msg,
    ProtocolPtr protocol,
    QWidget* parentObj)
  : Base(parentObj),
    m_msg(msg),
    m_protocol(std::move(protocol)),
    m_allMsgs(m_protocol->createAllMessages()),
    m_msgDisplayWidget(new DefaultMessageDisplayWidget())
{
    int msgIdx = -1;
    if (m_msg) {
        m_origScrollPos = property::message::ScrollPos().getFrom(*m_msg);
        auto id = m_msg->idAsString();
        int msgIdxTmp = 0;
        for (auto& msgTmp : m_allMsgs) {
            assert(msgTmp);
            auto idTmp = msgTmp->idAsString();
            if ((idTmp == id) && msgTmp->assign(*m_msg)) {
                m_protocol->updateMessage(*msgTmp);
                property::message::ScrollPos().setTo(m_origScrollPos, *msgTmp);

                msgIdx = msgIdxTmp;
                break;
            }
           ++msgIdxTmp;
        }
    }


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
    if (0 <= msgIdx) {
        m_ui.m_msgListWidget->setCurrentRow(msgIdx);
        m_msgDisplayWidget->displayMessage(
            getMsgFromItem(m_ui.m_msgListWidget->currentItem()));

        auto delayUnits =
            stringToDuration(property::message::DelayUnits().getFrom(*m_msg));
        auto delay = msToDurationUnits(property::message::Delay().getFrom(*m_msg), delayUnits);
        if (delay != 0) {
            m_prevDelay = delay;
            m_ui.m_delayUnitsComboBox->setCurrentIndex(static_cast<int>(delayUnits));
            m_ui.m_delayCheckBox->setCheckState(Qt::Checked);
        }

        auto repeatVal = property::message::RepeatDuration().getFrom(*m_msg);
        auto repeatUnits =
            stringToDuration(
                property::message::RepeatDurationUnits().getFrom(*m_msg));
        auto repeatDuration =
            msToDurationUnits(
                repeatVal,
                repeatUnits);
        if (repeatDuration != 0) {
            m_prevRepeatDuration = repeatDuration;
            m_ui.m_repeatUnitsComboBox->setCurrentIndex(static_cast<int>(repeatUnits));
            m_ui.m_repeatCheckBox->setCheckState(Qt::Checked);
        }

        auto repeatCount = static_cast<int>(
            property::message::RepeatCount().getFrom(*m_msg));
        if (repeatCount != 0) {
            m_prevRepeatCount = repeatCount;
        }
        else {
            m_sendIndefinitelyState = Qt::Checked;
        }
    }
    refreshDelayInfo(m_ui.m_delayCheckBox->checkState());
    refreshRepeatInfo(m_ui.m_repeatCheckBox->checkState());
    refreshButtons();

    assert(parentObj);
    auto newHeight = std::max(height(), (parentObj->height() * 9) / 10);
    auto newWidth = std::max(width(), (parentObj->width() * 7) / 10);
    resize(QSize(newWidth, newHeight));

    connect(
        m_msgDisplayWidget, SIGNAL(sigMsgUpdated()),
        this, SLOT(msgUpdated()));

    connect(
        m_ui.m_msgListWidget, SIGNAL(itemSelectionChanged()),
        this, SLOT(newItemSelected()));

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
    auto msgVar = item->data(Qt::UserRole);
    assert(msgVar.isValid());
    assert(msgVar.canConvert<MessagePtr>());
    auto msg = msgVar.value<MessagePtr>();

    assert(m_protocol);
    assert(msg);
    auto status = m_protocol->updateMessage(*msg);
    bool forceUpdate = (status == Protocol::UpdateStatus::Changed);
    assert(m_msgDisplayWidget);

    // Direct invocation of m_msgDisplayWidget->displayMessage(std::move(msg))
    // in place here causes SIGSEGV. No idea why.
    QMetaObject::invokeMethod(
        this,
        "displayMessagePostponed",
        Qt::QueuedConnection,
        Q_ARG(comms_champion::MessagePtr, std::move(msg)),
        Q_ARG(bool, forceUpdate));
    //m_msgDisplayWidget->displayMessage(std::move(msg), forceUpdate);
}

void MessageUpdateDialog::newItemSelected()
{
    assert(m_ui.m_msgListWidget != nullptr);
    auto* item = m_ui.m_msgListWidget->currentItem();
    assert(item != nullptr);

    auto msg = getMsgFromItem(item);
    assert(msg);

    m_msgDisplayWidget->displayMessage(std::move(msg));
    refreshButtons();
}

void MessageUpdateDialog::displayMessagePostponed(MessagePtr msg, bool force)
{
    m_msgDisplayWidget->displayMessage(std::move(msg), force);
}

void MessageUpdateDialog::refreshDisplayedList(const QString& searchText)
{
    MessagePtr selected;
    if (0 <= m_ui.m_msgListWidget->currentRow()) {
        selected = getMsgFromItem(m_ui.m_msgListWidget->currentItem());
    }

    m_ui.m_msgListWidget->clear();

    for (auto& msg : m_allMsgs) {
        auto msgName = getMessageNameForList(msg);
        if (searchText.isEmpty() || msgName.contains(searchText, Qt::CaseInsensitive)) {
            m_ui.m_msgListWidget->addItem(msgName);
            auto* item = m_ui.m_msgListWidget->item(m_ui.m_msgListWidget->count() - 1);
            item->setData(
                Qt::UserRole,
                QVariant::fromValue(msg));

            if (selected && (msg.get() == selected.get())) {
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

        m_ui.m_repeatCountSpinBox->setMinimum(DisabledRepeatCount);
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
        m_ui.m_repeatCountSpinBox->setMinimum(EnabledMinRepeatCount);
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
    auto msg = getMsgFromItem(m_ui.m_msgListWidget->currentItem());
    assert(msg);

    auto delayUnits =
        static_cast<Duration>(m_ui.m_delayUnitsComboBox->currentIndex());
    property::message::Delay().setTo(
        durationToMs(m_ui.m_delaySpinBox->value(), delayUnits), *msg);
    property::message::DelayUnits().setTo(durationToString(delayUnits), *msg);

    auto repeatUnits =
        static_cast<Duration>(m_ui.m_repeatUnitsComboBox->currentIndex());
    property::message::RepeatDuration().setTo(
        durationToMs(m_ui.m_repeatSpinBox->value(), repeatUnits), *msg);
    property::message::RepeatDurationUnits().setTo(
        durationToString(repeatUnits), *msg);
    property::message::RepeatCount().setTo(
        m_ui.m_repeatCountSpinBox->value(), *msg);

    property::message::ScrollPos().setTo(m_origScrollPos, *msg);

    m_msg = std::move(msg);
    assert(m_msg);
    Base::accept();
}

void MessageUpdateDialog::reset()
{
    auto msg = getMsgFromItem(m_ui.m_msgListWidget->currentItem());
    assert(msg);
    msg->reset();
    m_protocol->updateMessage(*msg);
    assert(m_msgDisplayWidget);
    m_msgDisplayWidget->refresh();
    m_msgDisplayWidget->displayMessage(std::move(msg));
}

MessagePtr MessageUpdateDialog::getMsgFromItem(QListWidgetItem* item)
{
    assert(item);
    auto var = item->data(Qt::UserRole);
    assert(var.canConvert<MessagePtr>());
    return var.value<MessagePtr>();
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


