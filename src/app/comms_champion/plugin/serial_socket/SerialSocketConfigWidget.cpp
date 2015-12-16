//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
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

#include "SerialSocketConfigWidget.h"

#include <cassert>
#include <type_traits>
#include <algorithm>
#include <iterator>

namespace comms_champion
{

namespace plugin
{

namespace serial_socket
{

namespace
{

const QSerialPort::Parity ParityMap[] = {
    QSerialPort::NoParity,
    QSerialPort::EvenParity,
    QSerialPort::OddParity,
    QSerialPort::SpaceParity,
    QSerialPort::MarkParity
};

const int NumOfParityValues =
    std::extent<decltype(ParityMap)>::value;

const QSerialPort::StopBits StopBitMap[] = {
    QSerialPort::OneStop,
    QSerialPort::OneAndHalfStop,
    QSerialPort::TwoStop
};

const int NumOfStopBitValues =
    std::extent<decltype(StopBitMap)>::value;

QSerialPort::Parity mapParityFromIdx(int idx)
{
    if ((idx < 0) || (NumOfParityValues <= idx)) {
        assert(!"Invalid index");
        return ParityMap[0];
    }

    return ParityMap[idx];
}

int mapParityToIdx(QSerialPort::Parity value)
{
    auto iter = std::find(std::begin(ParityMap), std::end(ParityMap), value);
    if (iter == std::end(ParityMap)) {
        assert(!"Invalid value");
        return 0;
    }

    return static_cast<int>(std::distance(std::begin(ParityMap), iter));
}

QSerialPort::StopBits mapStopBitsFromIdx(int idx)
{
    if ((idx < 0) || (NumOfStopBitValues <= idx)) {
        assert(!"Invalid index");
        return StopBitMap[0];
    }

    return StopBitMap[idx];
}

int mapStopBitToIdx(QSerialPort::StopBits value)
{
    auto iter = std::find(std::begin(StopBitMap), std::end(StopBitMap), value);
    if (iter == std::end(StopBitMap)) {
        assert(!"Invalid value");
        return 0;
    }

    return static_cast<int>(std::distance(std::begin(StopBitMap), iter));
}

QSerialPort::FlowControl mapFlowControlFromIdx(int idx)
{
    return static_cast<QSerialPort::FlowControl>(idx);
}

int mapFlowControlToIdx(QSerialPort::FlowControl value)
{
    return static_cast<int>(value);
}

}  // namespace

SerialSocketConfigWidget::SerialSocketConfigWidget(
    SerialSocket& socket,
    QWidget* parentObj)
  : Base(parentObj),
    m_socket(socket)
{
    m_ui.setupUi(this);
    m_ui.m_deviceLineEdit->setText(m_socket.name());
    m_ui.m_baudSpinBox->setValue(m_socket.baud());
    m_ui.m_dataBitsSpinBox->setValue(m_socket.dataBits());
    m_ui.m_parityComboBox->setCurrentIndex(mapParityToIdx(m_socket.parity()));
    m_ui.m_stopBitsComboBox->setCurrentIndex(mapStopBitToIdx(m_socket.stopBits()));
    m_ui.m_flowComboBox->setCurrentIndex(mapFlowControlToIdx(m_socket.flowControl()));

    connect(
        m_ui.m_deviceLineEdit, SIGNAL(textEdited(const QString&)),
        this, SLOT(nameChanged(const QString&)));

    connect(
        m_ui.m_baudSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(baudChanged(int)));

    connect(
        m_ui.m_dataBitsSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(dataBitsChanged(int)));

    connect(
        m_ui.m_parityComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(parityChanged(int)));

    connect(
        m_ui.m_stopBitsComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(stopBitsChanged(int)));

    connect(
        m_ui.m_flowComboBox, SIGNAL(currentIndexChanged(int)),
        this, SLOT(flowControlChanged(int)));
}

SerialSocketConfigWidget::~SerialSocketConfigWidget() = default;

void SerialSocketConfigWidget::nameChanged(const QString& value)
{
    m_socket.name() = value;
}

void SerialSocketConfigWidget::baudChanged(int value)
{
    m_socket.baud() = static_cast<SerialSocket::Baud>(value);
}

void SerialSocketConfigWidget::dataBitsChanged(int value)
{
    m_socket.dataBits() = static_cast<SerialSocket::DataBits>(value);
}

void SerialSocketConfigWidget::parityChanged(int value)
{
    m_socket.parity() = mapParityFromIdx(value);
}

void SerialSocketConfigWidget::stopBitsChanged(int value)
{
    m_socket.stopBits() = mapStopBitsFromIdx(value);
}

void SerialSocketConfigWidget::flowControlChanged(int value)
{
    m_socket.flowControl() = mapFlowControlFromIdx(value);
}

}  // namespace serial_socket

}  // namespace plugin

}  // namespace comms_champion


