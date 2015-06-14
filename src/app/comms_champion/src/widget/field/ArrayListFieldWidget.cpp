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

#include "ArrayListFieldWidget.h"

#include <algorithm>
#include <cassert>

#include "comms_champion/Property.h"

namespace comms_champion
{

ArrayListElementWidget::ArrayListElementWidget(
    FieldWidget* fieldWidget,
    QWidget* parent)
  : Base(parent)
{
    m_ui.setupUi(this);
    m_fieldWidget = fieldWidget;
    m_ui.m_layout->addWidget(fieldWidget);

    connect(
        m_fieldWidget, SIGNAL(sigFieldUpdated()),
        this, SIGNAL(sigFieldUpdated()));

    connect(
        m_ui.m_removePushButton, SIGNAL(clicked()),
        this, SIGNAL(sigRemoveRequested()));

    updateUi();
}

void ArrayListElementWidget::refresh()
{
    m_fieldWidget->refresh();
}

void ArrayListElementWidget::setEditEnabled(bool enabled)
{
    m_editEnabled = enabled;
    m_fieldWidget->setEditEnabled(enabled);
    updateUi();
}

void ArrayListElementWidget::updateProperties(const QVariantMap& props)
{
    assert(m_fieldWidget != nullptr);

    m_fieldWidget->updateProperties(props);
}

void ArrayListElementWidget::updateUi()
{
    m_ui.m_buttonWidget->setVisible(m_editEnabled);
    m_ui.m_sepLine->setVisible(m_editEnabled);
}

ArrayListFieldWidget::ArrayListFieldWidget(
    WrapperPtr wrapper,
    CreateMissingDataFieldsFunc&& updateFunc,
    QWidget* parent)
  : Base(parent),
    m_wrapper(std::move(wrapper)),
    m_createMissingDataFieldsCallback(std::move(updateFunc))
{
    m_ui.setupUi(this);
    setNameLabelWidget(m_ui.m_nameLabel);
    setValueWidget(m_ui.m_valueWidget);
    setSeparatorWidget(m_ui.m_sepLine);
    setSerialisedValueWidget(m_ui.m_serValueWidget);

    refreshInternal();
    addMissingFields();

    updateUi();

    connect(
        m_ui.m_addFieldPushButton, SIGNAL(clicked()),
        this, SLOT(addNewField()));
}

ArrayListFieldWidget::~ArrayListFieldWidget() = default;

void ArrayListFieldWidget::refreshImpl()
{
    while (!m_elements.empty()) {
        assert(m_elements.back() != nullptr);
        delete m_elements.back();
        m_elements.pop_back();
    }

    refreshInternal();
    addMissingFields();
}

void ArrayListFieldWidget::setEditEnabledImpl(bool enabled)
{
    for (auto* elem : m_elements) {
        elem->setEditEnabled(enabled);
    }
    updateUi();
}

void ArrayListFieldWidget::updatePropertiesImpl(const QVariantMap& props)
{
    auto elemPropsVar = props.value(Property::data());
    if ((!elemPropsVar.isValid()) || (!elemPropsVar.canConvert<QVariantMap>())) {
        return;
    }

    auto elemProps = elemPropsVar.value<QVariantMap>();

    for (auto* elem : m_elements) {
        elem->updateProperties(elemProps);
    }
}

void ArrayListFieldWidget::dataFieldUpdated()
{
    refreshInternal();
    emitFieldUpdated();
}

void ArrayListFieldWidget::addNewField()
{
    m_wrapper->addField();
    addMissingFields();
    dataFieldUpdated();
}

void ArrayListFieldWidget::removeField()
{
    auto* sigSender = sender();
    auto iter = std::find(m_elements.begin(), m_elements.end(), sigSender);
    if (iter == m_elements.end()) {
        assert(!"Something is not right");
        return;
    }

    auto idx = static_cast<int>(std::distance(m_elements.begin(), iter));
    m_wrapper->removeField(idx);

    assert((*iter) != nullptr);
    delete *iter;
    m_elements.erase(iter);

    refreshInternal();
    emitFieldUpdated();
}

void ArrayListFieldWidget::addDataField(FieldWidget* dataFieldWidget)
{
    auto* wrapperWidget = new ArrayListElementWidget(dataFieldWidget);
    wrapperWidget->setEditEnabled(isEditEnabled());

    auto propsVar = Property::getDataVal(*this);
    if (propsVar.isValid() && propsVar.canConvert<QVariantMap>()) {
        auto props = propsVar.value<QVariantMap>();
        wrapperWidget->updateProperties(props);
    }

    connect(
        wrapperWidget, SIGNAL(sigFieldUpdated()),
        this, SLOT(dataFieldUpdated()));

    connect(
        wrapperWidget, SIGNAL(sigRemoveRequested()),
        this, SLOT(removeField()));

    m_elements.push_back(wrapperWidget);
    m_ui.m_membersLayout->addWidget(wrapperWidget);
}

void ArrayListFieldWidget::refreshInternal()
{
    QString serValueStr;
    auto serValue = m_wrapper->getSerialisedValue();

    for (auto byte : serValue) {
        if (!serValueStr.isEmpty()) {
            serValueStr.append(QChar(' '));
        }
        serValueStr.append(QString("%1").arg(byte, 2, 16, QChar('0')));
    }

    assert(m_ui.m_serValuePlainTextEdit != nullptr);
    m_ui.m_serValuePlainTextEdit->setPlainText(serValueStr);


    bool valid = m_wrapper->valid();
    setValidityStyleSheet(*m_ui.m_nameLabel, valid);
    setValidityStyleSheet(*m_ui.m_serFrontLabel, valid);
    setValidityStyleSheet(*m_ui.m_serValuePlainTextEdit, valid);
    setValidityStyleSheet(*m_ui.m_serBackLabel, valid);
}

void ArrayListFieldWidget::updateUi()
{
    bool enabled = isEditEnabled();
    m_ui.m_addSepLine->setVisible(enabled);
    m_ui.m_addFieldPushButton->setVisible(enabled);
}

void ArrayListFieldWidget::addMissingFields()
{
    if (!m_createMissingDataFieldsCallback) {
        return;
    }

    auto fieldWidgets = m_createMissingDataFieldsCallback(m_elements.size());
    for (auto& fieldWidgetPtr : fieldWidgets) {
        addDataField(fieldWidgetPtr.release());
    }
}

}  // namespace comms_champion


