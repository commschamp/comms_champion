//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
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

#include "DefaultMessageDisplayHandler.h"

#include <cassert>

#include "comms_champion/field_wrapper/FieldWrapperHandler.h"
#include "widget/field/IntValueFieldWidget.h"
#include "widget/field/BitmaskValueFieldWidget.h"
#include "widget/field/EnumValueFieldWidget.h"
#include "widget/field/StringFieldWidget.h"
#include "widget/field/BitfieldFieldWidget.h"
#include "widget/field/OptionalFieldWidget.h"
#include "widget/field/BundleFieldWidget.h"
#include "widget/field/ArrayListRawDataFieldWidget.h"
#include "widget/field/ArrayListFieldWidget.h"
#include "widget/field/FloatValueFieldWidget.h"
#include "widget/field/UnknownValueFieldWidget.h"

namespace comms_champion
{

namespace
{

class WidgetCreator : public field_wrapper::FieldWrapperHandler
{
public:
    typedef field_wrapper::FieldWrapperPtr FieldWrapperPtr;

    virtual void handle(field_wrapper::IntValueWrapper& wrapper) override
    {
        m_widget.reset(new IntValueFieldWidget(wrapper.clone()));
    }

    virtual void handle(field_wrapper::BitmaskValueWrapper& wrapper) override
    {
        m_widget.reset(new BitmaskValueFieldWidget(wrapper.clone()));
    }

    virtual void handle(field_wrapper::EnumValueWrapper& wrapper) override
    {
        m_widget.reset(new EnumValueFieldWidget(wrapper.clone()));
    }

    virtual void handle(field_wrapper::StringWrapper& wrapper) override
    {
        m_widget.reset(new StringFieldWidget(wrapper.clone()));
    }

    virtual void handle(field_wrapper::BitfieldWrapper& wrapper) override
    {
        auto& membersWrappers = wrapper.getMembers();
        std::vector<FieldWidgetPtr> membersWidgets;
        membersWidgets.reserve(membersWrappers.size());
        for (auto& mem : membersWrappers) {
            mem->dispatch(*this);
            membersWidgets.push_back(getWidget());
        }

        std::unique_ptr<BitfieldFieldWidget> widget(new BitfieldFieldWidget(wrapper.clone()));
        for (auto& memWidget : membersWidgets) {
            widget->addMemberField(memWidget.release());
        }

        m_widget = std::move(widget);
    }

    virtual void handle(field_wrapper::OptionalWrapper& wrapper) override
    {
        wrapper.getFieldWrapper().dispatch(*this);
        auto wrappedWidget = getWidget();

        std::unique_ptr<OptionalFieldWidget> widget(new OptionalFieldWidget(wrapper.clone()));
        widget->setField(wrappedWidget.release());
        m_widget = std::move(widget);
    }

    virtual void handle(field_wrapper::BundleWrapper& wrapper) override
    {
        auto& membersWrappers = wrapper.getMembers();
        std::vector<FieldWidgetPtr> membersWidgets;
        membersWidgets.reserve(membersWrappers.size());
        for (auto& mem : membersWrappers) {
            mem->dispatch(*this);
            membersWidgets.push_back(getWidget());
        }

        std::unique_ptr<BundleFieldWidget> widget(new BundleFieldWidget());
        for (auto& memWidget : membersWidgets) {
            widget->addMemberField(memWidget.release());
        }

        m_widget = std::move(widget);
    }

    virtual void handle(field_wrapper::ArrayListRawDataWrapper& wrapper) override
    {
        m_widget.reset(new ArrayListRawDataFieldWidget(wrapper.clone()));
    }

    virtual void handle(field_wrapper::ArrayListWrapper& wrapper) override
    {
        auto createMembersWidgetsFunc =
            [](field_wrapper::ArrayListWrapper& wrap) -> std::vector<FieldWidgetPtr>
            {
                std::vector<FieldWidgetPtr> allFieldsWidgets;
                WidgetCreator otherCreator;
                auto& memWrappers = wrap.getMembers();
                allFieldsWidgets.reserve(memWrappers.size());
                assert(memWrappers.size() == wrap.size());

                for (auto& memWrap : memWrappers) {
                    memWrap->dispatch(otherCreator);
                    allFieldsWidgets.push_back(otherCreator.getWidget());
                }

                assert(allFieldsWidgets.size() == wrap.size());
                return allFieldsWidgets;
            };

        assert(wrapper.size() == wrapper.getMembers().size());
        m_widget.reset(new ArrayListFieldWidget(wrapper.clone(), std::move(createMembersWidgetsFunc)));
    }

    virtual void handle(field_wrapper::FloatValueWrapper& wrapper) override
    {
        m_widget.reset(new FloatValueFieldWidget(wrapper.clone()));
    }

    virtual void handle(field_wrapper::UnknownValueWrapper& wrapper) override
    {
        m_widget.reset(new UnknownValueFieldWidget(wrapper.clone()));
    }

    virtual void handle(field_wrapper::FieldWrapper& wrapper) override
    {
        static_cast<void>(wrapper);
        assert(!"Unexpected wrapper");
    }

    FieldWidgetPtr getWidget()
    {
        return std::move(m_widget);
    }


private:
    FieldWidgetPtr m_widget;
};



}  // namespace

DefaultMessageDisplayHandler::~DefaultMessageDisplayHandler() = default;

DefaultMessageDisplayHandler::MsgWidgetPtr DefaultMessageDisplayHandler::getMsgWidget()
{
    return std::move(m_widget);
}

void DefaultMessageDisplayHandler::beginMsgHandlingImpl(
    Message& msg)
{
    m_widget.reset(new DefaultMessageWidget(msg));
}

void DefaultMessageDisplayHandler::addFieldImpl(FieldWrapperPtr wrapper)
{
    assert(m_widget);
    WidgetCreator creator;
    wrapper->dispatch(creator);
    m_widget->addFieldWidget(creator.getWidget().release());
}

}  // namespace comms_champion


