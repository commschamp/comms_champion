//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
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


#include <memory>

#include "comms/comms.h"

#include "comms_champion/field_wrapper/IntValueWrapper.h"
#include "comms_champion/field_wrapper/BitmaskValueWrapper.h"
#include "comms_champion/field_wrapper/EnumValueWrapper.h"
#include "comms_champion/field_wrapper/StringWrapper.h"
#include "comms_champion/field_wrapper/BitfieldWrapper.h"
#include "comms_champion/field_wrapper/OptionalWrapper.h"
#include "comms_champion/field_wrapper/BundleWrapper.h"
#include "comms_champion/field_wrapper/ArrayListRawDataWrapper.h"
#include "comms_champion/field_wrapper/ArrayListWrapper.h"
#include "comms_champion/field_wrapper/FloatValueWrapper.h"
#include "comms_champion/field_wrapper/UnknownValueWrapper.h"

#include "FieldWrapperTag.h"

namespace comms_champion
{

namespace details
{

class FieldWrapperCreator
{
public:
    typedef comms_champion::field_wrapper::FieldWrapperPtr FieldWrapperPtr;

    template <typename TField>
    static FieldWrapperPtr createWrapper(TField& field)
    {
        typedef typename std::decay<decltype(field)>::type DecayedField;
        typedef details::FieldWrapperTagOfT<DecayedField> Tag;
        return createWrapperInternal(field, Tag());
    }
private:
    typedef FieldWrapperIntValueTag IntValueTag;
    typedef FieldWrapperBitmaskValueTag BitmaskValueTag;
    typedef FieldWrapperEnumValueTag EnumValueTag;
    typedef FieldWrapperStringTag StringTag;
    typedef FieldWrapperBitfieldTag BitfieldTag;
    typedef FieldWrapperOptionalTag OptionalTag;
    typedef FieldWrapperBundleTag BundleTag;
    typedef FieldWrapperRawDataArrayListTag RawDataArrayListTag;
    typedef FieldWrapperFieldsArrayListTag FieldsArrayListTag;
    typedef FieldWrapperFloatValueTag FloatValueTag;
    typedef FieldWrapperUnknownValueTag UnknownValueTag;

    class SubfieldsCreateHelper
    {
    public:
        typedef std::function <void (FieldWrapperPtr)> WrapperDispatchFunc;
        SubfieldsCreateHelper(WrapperDispatchFunc&& dispatchOp)
          : m_dispatchOp(std::move(dispatchOp))
        {
        }

        template <typename TField>
        void operator()(TField&& field)
        {
            auto fieldWidget =
                FieldWrapperCreator::createWrapper(std::forward<TField>(field));
            m_dispatchOp(std::move(fieldWidget));
        }

    private:
        WrapperDispatchFunc m_dispatchOp;
    };


    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, IntValueTag)
    {
        return field_wrapper::makeIntValueWrapper(field);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, BitmaskValueTag)
    {
        return field_wrapper::makeBitmaskValueWrapper(field);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, EnumValueTag)
    {
        return field_wrapper::makeEnumValueWrapper(field);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, StringTag)
    {
        return field_wrapper::makeStringWrapper(field);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, BitfieldTag)
    {
        auto wrapper = field_wrapper::makeBitfieldWrapper(field);

        typedef typename std::decay<decltype(wrapper)>::type WrapperPtrType;
        typedef typename WrapperPtrType::element_type WrapperType;
        typedef typename WrapperType::Members MembersWrappersList;

        MembersWrappersList subWrappers;
        auto& memberFields = field.value();
        comms::util::tupleForEach(
            memberFields,
            SubfieldsCreateHelper(
                [&subWrappers](FieldWrapperPtr fieldWrapper)
                {
                    subWrappers.push_back(std::move(fieldWrapper));
                }));

        wrapper->setMembers(std::move(subWrappers));
        return std::move(wrapper);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, OptionalTag)
    {
        auto wrapper = field_wrapper::makeOptionalWrapper(field);
        auto& wrappedField = field.field();
        auto fieldWrapper = createWrapper(wrappedField);
        wrapper->setFieldWrapper(std::move(fieldWrapper));
        return std::move(wrapper);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, BundleTag)
    {
        auto wrapper = field_wrapper::makeBundleWrapper(field);

        typedef typename std::decay<decltype(wrapper)>::type WrapperPtrType;
        typedef typename WrapperPtrType::element_type WrapperType;
        typedef typename WrapperType::Members MembersWrappersList;

        MembersWrappersList subWrappers;
        auto& memberFields = field.value();
        comms::util::tupleForEach(
            memberFields,
            SubfieldsCreateHelper(
                [&subWrappers](FieldWrapperPtr fieldWrapper)
                {
                    subWrappers.push_back(std::move(fieldWrapper));
                }));

        wrapper->setMembers(std::move(subWrappers));
        return std::move(wrapper);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, RawDataArrayListTag)
    {
        return field_wrapper::makeArrayListRawDataWrapper(field);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, FieldsArrayListTag)
    {
        typedef typename std::decay<decltype(field)>::type DecayedField;
        typedef typename DecayedField::ValueType CollectionType;
        typedef typename CollectionType::value_type ElementType;

        auto wrapper = field_wrapper::makeDowncastedArrayListWrapper(field);
        wrapper->setWrapFieldCallback(
            [](ElementType& memField) -> FieldWrapperPtr
            {
                return FieldWrapperCreator::createWrapper(memField);
            });
        return std::move(wrapper);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, FloatValueTag)
    {
        return field_wrapper::makeFloatValueWrapper(field);
    }

    template <typename TField>
    static FieldWrapperPtr createWrapperInternal(TField& field, UnknownValueTag)
    {
        return field_wrapper::makeUnknownValueWrapper(field);
    }
};

}  // namespace details

}  // namespace comms_champion





