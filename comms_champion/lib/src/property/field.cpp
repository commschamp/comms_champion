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

#include "comms_champion/property/field.h"

namespace comms_champion
{

namespace property
{

namespace field
{

namespace
{

const QString& nameKey()
{
    static const QString Str("cc.name");
    return Str;
}

const QString& dataKey()
{
    static const QString Str("cc.data");
    return Str;
}

const QString& serialisedHiddenKey()
{
    static const QString Str("cc.ser_hidden");
    return Str;
}

const QString& fieldHiddenKey()
{
    static const QString Str("cc.field_hidden");
    return Str;
}

const QString& readOnlyKey()
{
    static const QString Str("cc.read_only");
    return Str;
}

const QString& floatDecimalsKey()
{
    static const QString Str("cc.float_dec");
    return Str;
}

const QString& uncheckableKey()
{
    static const QString Str("cc.uncheckable");
    return Str;
}

const QString& numValueDisplayOffsetKey()
{
    static const QString Str("cc.num_display_offset");
    return Str;
}

const QString& indexHiddenKey()
{
    static const QString Str("cc.index_hidden");
    return Str;
}

const QString& showPrefixKey()
{
    static const QString Str("cc.show_prefix");
    return Str;
}

const QString& prefixNameKey()
{
    static const QString Str("cc.prefix_name");
    return Str;
}

}  // namespace

Common::Common() = default;
Common::Common(const Common&) = default;
Common::Common(Common&&) = default;

Common::Common(const QVariantMap& props)
{
    getFrom(props);
}

Common::Common(const QVariant& props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
}

Common::~Common() = default;

Common& Common::operator=(const Common&) = default;
Common& Common::operator=(Common&&) = default;

const QString& Common::name() const
{
    return m_name;
}

bool Common::isHidden() const
{
    return m_hidden;
}

bool Common::isSerialisedHidden() const
{
    return m_serialisedHidden;
}

bool Common::isReadOnly() const
{
    return m_readOnly;
}

void Common::setName(const QString& value)
{
    m_name = value;
}

void Common::setName(const char* value)
{
    m_name = value;
}

void Common::hidden(bool value)
{
    m_hidden = value;
}

void Common::serialisedHidden(bool value)
{
    m_serialisedHidden = value;
}

void Common::readOnly(bool value)
{
    m_readOnly = value;
}

void Common::setTo(QVariantMap& props) const
{
    setElemTo(m_name, nameKey(), props);
    setElemTo(m_hidden, fieldHiddenKey(), props);
    setElemTo(m_serialisedHidden, serialisedHiddenKey(), props);
    setElemTo(m_readOnly, readOnlyKey(), props);
}

void Common::getFrom(const QVariantMap& props)
{
    m_name = getElemFrom<decltype(m_name)>(props, nameKey());
    m_hidden = getElemFrom<decltype(m_hidden)>(props, fieldHiddenKey());
    m_serialisedHidden = getElemFrom<decltype(m_serialisedHidden)>(props, serialisedHiddenKey());
    m_readOnly = getElemFrom<decltype(m_readOnly)>(props, readOnlyKey());
}

IntValue::IntValue() = default;
IntValue::IntValue(const IntValue&) = default;
IntValue::IntValue(IntValue&&) = default;
IntValue::IntValue(const QVariantMap& props) : Base(props)
{
    getFrom(props);
};

IntValue::IntValue(const QVariant& props) : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
};
IntValue::~IntValue() = default;

IntValue& IntValue::operator=(const IntValue&) = default;
IntValue& IntValue::operator=(IntValue&&) = default;

long long IntValue::displayOffset() const
{
    return m_displayOffset;
}

IntValue& IntValue::displayOffset(long long value)
{
    m_displayOffset = value;
    return *this;
}

int IntValue::scaledDecimals() const
{
    return m_scaledDecimals;
}

bool IntValue::hasScaledDecimals() const
{
    return 0 < scaledDecimals();
}

IntValue& IntValue::scaledDecimals(int value)
{
    m_scaledDecimals = value;
    return *this;
}

QVariantMap IntValue::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_displayOffset, numValueDisplayOffsetKey(), props);
    Base::setElemTo(m_scaledDecimals, floatDecimalsKey(), props);
    return props;
}

void IntValue::getFrom(const QVariantMap& props)
{
    m_displayOffset =
        getElemFrom<decltype(m_displayOffset)>(props, numValueDisplayOffsetKey());

    m_scaledDecimals =
        getElemFrom<decltype(m_scaledDecimals)>(props, floatDecimalsKey());
}

EnumValue::EnumValue() = default;
EnumValue::EnumValue(const EnumValue&) = default;
EnumValue::EnumValue(EnumValue&&) = default;
EnumValue::EnumValue(const QVariantMap& props) : Base(props)
{
    getFrom(props);
};

EnumValue::EnumValue(const QVariant& props) : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
};

EnumValue::~EnumValue() = default;

EnumValue& EnumValue::operator=(const EnumValue&) = default;
EnumValue& EnumValue::operator=(EnumValue&&) = default;

const EnumValue::ElemsList& EnumValue::values() const
{
    return m_elems;
}

EnumValue& EnumValue::add(const QString& elemName, long long value)
{
    m_elems.append(ElemType(elemName, value));
    return *this;
}

EnumValue& EnumValue::add(const QString& elemName)
{
    if (m_elems.isEmpty()) {
        return add(elemName, 0);
    }

    auto& lastElem = m_elems.back();
    return add(elemName, lastElem.second + 1);
}

QVariantMap EnumValue::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_elems, dataKey(), props);
    return props;
}

void EnumValue::getFrom(const QVariantMap& props)
{
    m_elems = getElemFrom<ElemsList>(props, dataKey());
}

BitmaskValue::BitmaskValue() = default;
BitmaskValue::BitmaskValue(const BitmaskValue&) = default;
BitmaskValue::BitmaskValue(BitmaskValue&&) = default;
BitmaskValue::BitmaskValue(const QVariantMap& props) : Base(props)
{
    getFrom(props);
};

BitmaskValue::BitmaskValue(const QVariant& props) : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
};

BitmaskValue::~BitmaskValue() = default;

BitmaskValue& BitmaskValue::operator=(const BitmaskValue&) = default;
BitmaskValue& BitmaskValue::operator=(BitmaskValue&&) = default;

const BitmaskValue::BitsList& BitmaskValue::bits() const
{
    return m_bits;
}

BitmaskValue& BitmaskValue::add(int idx, const QString& bitName)
{
    if (idx < m_bits.size()) {
        m_bits[idx] = bitName;
        return *this;
    }

    while (m_bits.size() < idx) {
        m_bits.append(QVariant());
    }

    m_bits.append(bitName);
    return *this;
}

BitmaskValue& BitmaskValue::add(const QString& bitName)
{
    return add(m_bits.size(), bitName);
}

QVariantMap BitmaskValue::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_bits, dataKey(), props);
    return props;
}

void BitmaskValue::getFrom(const QVariantMap& props)
{
    m_bits = getElemFrom<BitsList>(props, dataKey());
}

Bitfield::Bitfield() = default;
Bitfield::Bitfield(const Bitfield&) = default;
Bitfield::Bitfield(Bitfield&&) = default;
Bitfield::Bitfield(const QVariantMap& props) : Base(props)
{
    getFrom(props);
};

Bitfield::Bitfield(const QVariant& props) : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
};

Bitfield::~Bitfield() = default;

Bitfield& Bitfield::operator=(const Bitfield&) = default;
Bitfield& Bitfield::operator=(Bitfield&&) = default;

const Bitfield::MembersList& Bitfield::members() const
{
    return m_members;
}

Bitfield& Bitfield::add(QVariantMap&& memberProps)
{
    m_members.append(std::move(memberProps));
    return *this;
}

Bitfield& Bitfield::add(const QVariantMap& memberProps)
{
    m_members.append(memberProps);
    return *this;
}

QVariantMap Bitfield::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_members, dataKey(), props);
    return props;
}

void Bitfield::getFrom(const QVariantMap& props)
{
    m_members = getElemFrom<MembersList>(props, dataKey());
}

Bundle::Bundle() = default;
Bundle::Bundle(const Bundle&) = default;
Bundle::Bundle(Bundle&&) = default;
Bundle::Bundle(const QVariantMap& props) : Base(props)
{
    getFrom(props);
};

Bundle::Bundle(const QVariant& props) : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
};

Bundle::~Bundle() = default;

Bundle& Bundle::operator=(const Bundle&) = default;
Bundle& Bundle::operator=(Bundle&&) = default;

const Bundle::MembersList& Bundle::members() const
{
    return m_members;
}

Bundle& Bundle::add(QVariantMap&& memberProps)
{
    m_members.append(std::move(memberProps));
    return *this;
}

Bundle& Bundle::add(const QVariantMap& memberProps)
{
    m_members.append(memberProps);
    return *this;
}

QVariantMap Bundle::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_members, dataKey(), props);
    return props;
}

void Bundle::getFrom(const QVariantMap& props)
{
    m_members = getElemFrom<MembersList>(props, dataKey());
}

String::String() = default;
String::String(const String&) = default;
String::String(String&&) = default;
String::String(const QVariantMap& props) : Base(props) {};
String::String(const QVariant& props) : Base(props) {};
String::~String() = default;

String& String::operator=(const String&) = default;
String& String::operator=(String&&) = default;

QVariantMap String::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    return props;
}

ArrayList::ArrayList() = default;
ArrayList::ArrayList(const ArrayList&) = default;
ArrayList::ArrayList(ArrayList&&) = default;
ArrayList::ArrayList(const QVariantMap& props)
  : Base(props)
{
    getFrom(props);
}

ArrayList::ArrayList(const QVariant& props)
  : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
}

ArrayList::~ArrayList() = default;

ArrayList& ArrayList::operator=(const ArrayList&) = default;
ArrayList& ArrayList::operator=(ArrayList&&) = default;

const ArrayList::ElemsList& ArrayList::elements() const
{
    return m_elems;
}

ArrayList& ArrayList::add(QVariantMap&& elemProps)
{
    m_elems.append(std::move(elemProps));
    return *this;
}

ArrayList& ArrayList::add(const QVariantMap& elemProps)
{
    m_elems.append(elemProps);
    return *this;
}

bool ArrayList::isPrefixVisible() const
{
    return m_showPrefix;
}

ArrayList& ArrayList::showPrefix(bool value)
{
    m_showPrefix = value;
    return *this;
}

const QString& ArrayList::prefixName() const
{
    return m_prefixName;
}

ArrayList& ArrayList::prefixName(const QString& name)
{
    m_prefixName = name;
    return *this;
}

QVariantMap ArrayList::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_elems, dataKey(), props);
    Base::setElemTo(m_showPrefix, showPrefixKey(), props);
    Base::setElemTo(m_prefixName, prefixNameKey(), props);
    return props;
}

void ArrayList::getFrom(const QVariantMap& props)
{
    m_elems = getElemFrom<ElemsList>(props, dataKey());
    m_showPrefix = getElemFrom<bool>(props, showPrefixKey());
    m_prefixName = getElemFrom<QString>(props, prefixNameKey());
}

Optional::Optional() = default;
Optional::Optional(const Optional&) = default;
Optional::Optional(Optional&&) = default;
Optional::Optional(const QVariantMap& props)
  : Base(props)
{
    getFrom(props);
}

Optional::Optional(const QVariant& props)
  : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
}

Optional::~Optional() = default;

Optional& Optional::operator=(const Optional&) = default;
Optional& Optional::operator=(Optional&&) = default;

const QVariantMap& Optional::field() const
{
    return m_field;
}

Optional& Optional::field(QVariantMap&& fieldProps)
{
    m_field = std::move(fieldProps);
    return *this;
}

Optional& Optional::field(const QVariantMap& fieldProps)
{
    m_field = fieldProps;
    return *this;
}

bool Optional::isUncheckable() const
{
    return m_uncheckable;
}

Optional& Optional::uncheckable(bool value)
{
    m_uncheckable = value;
    return *this;
}

QVariantMap Optional::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_field, dataKey(), props);
    Base::setElemTo(m_uncheckable, uncheckableKey(), props);
    return props;
}

void Optional::getFrom(const QVariantMap& props)
{
    m_field = getElemFrom<QVariantMap>(props, dataKey());
    m_uncheckable = getElemFrom<bool>(props, uncheckableKey());
}

FloatValue::FloatValue() = default;
FloatValue::FloatValue(const FloatValue&) = default;
FloatValue::FloatValue(FloatValue&&) = default;
FloatValue::FloatValue(const QVariantMap& props) : Base(props)
{
    getFrom(props);
};

FloatValue::FloatValue(const QVariant& props) : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
};
FloatValue::~FloatValue() = default;

FloatValue& FloatValue::operator=(const FloatValue&) = default;
FloatValue& FloatValue::operator=(FloatValue&&) = default;

int FloatValue::decimals() const
{
    return m_decimals;
}

FloatValue& FloatValue::decimals(int value)
{
    m_decimals = value;
    return *this;
}

QVariantMap FloatValue::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_decimals, floatDecimalsKey(), props);
    return props;
}

void FloatValue::getFrom(const QVariantMap& props)
{
    m_decimals = getElemFrom<decltype(m_decimals)>(props, floatDecimalsKey());
}

Variant::Variant() = default;
Variant::Variant(const Variant&) = default;
Variant::Variant(Variant&&) = default;
Variant::Variant(const QVariantMap& props) : Base(props)
{
    getFrom(props);
};

Variant::Variant(const QVariant& props) : Base(props)
{
    if (props.isValid() && props.canConvert<QVariantMap>()) {
        getFrom(props.value<QVariantMap>());
    }
};

Variant::~Variant() = default;

Variant& Variant::operator=(const Variant&) = default;
Variant& Variant::operator=(Variant&&) = default;

const Variant::MembersList& Variant::members() const
{
    return m_members;
}

Variant& Variant::add(QVariantMap&& memberProps)
{
    m_members.append(std::move(memberProps));
    return *this;
}

Variant& Variant::add(const QVariantMap& memberProps)
{
    m_members.append(memberProps);
    return *this;
}

bool Variant::isIndexHidden() const
{
    return m_indexHidden;
}

Variant& Variant::setIndexHidden(bool hidden)
{
    m_indexHidden = hidden;
    return *this;
}

QVariantMap Variant::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    Base::setElemTo(m_members, dataKey(), props);
    Base::setElemTo(m_indexHidden, indexHiddenKey(), props);
    return props;
}

void Variant::getFrom(const QVariantMap& props)
{
    m_members = getElemFrom<MembersList>(props, dataKey());
    m_indexHidden = getElemFrom<bool>(props, indexHiddenKey());
}

NoValue::NoValue() = default;
NoValue::NoValue(const NoValue&) = default;
NoValue::NoValue(NoValue&&) = default;
NoValue::NoValue(const QVariantMap& props) : Base(props)
{
};

NoValue::NoValue(const QVariant& props) : Base(props)
{
};
NoValue::~NoValue() = default;

NoValue& NoValue::operator=(const NoValue&) = default;
NoValue& NoValue::operator=(NoValue&&) = default;

QVariantMap NoValue::asMap() const
{
    QVariantMap props;
    Base::setTo(props);
    return props;
}

}  // namespace field

}  // namespace property

}  // namespace comms_champion


