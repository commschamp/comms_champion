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


#pragma once

#include "comms/CompileControl.h"

CC_DISABLE_WARNINGS()
#include <QtCore/QString>
#include <QtCore/QVariantMap>
#include <QtCore/QVariantList>
#include <QtCore/QList>
#include <QtCore/QPair>
CC_ENABLE_WARNINGS()

#include "comms/comms.h"
#include "comms_champion/Api.h"

namespace comms_champion
{

namespace property
{

namespace field
{

class CC_API Common
{
public:
    Common();
    Common(const Common&);
    Common(Common&&);
    Common(const QVariantMap& props);
    Common(const QVariant& props);
    ~Common();

    Common& operator=(const Common&);
    Common& operator=(Common&&);

    const QString& name() const;
    bool isHidden() const;
    bool isSerialisedHidden() const;
    bool isReadOnly() const;

protected:

    void setName(const QString& value);

    void setName(const char* value);

    void hidden(bool value = true);

    void serialisedHidden(bool value = true);

    void readOnly(bool value = true);

    void setTo(QVariantMap& props) const;

    void getFrom(const QVariantMap& props);

    template <typename U>
    static void setElemTo(U&& val, const QString& name, QVariantMap& props)
    {
        props.insert(name, QVariant::fromValue(std::forward<U>(val)));
    }

    template <typename TValueType>
    static TValueType getElemFrom(
        const QVariantMap& props,
        const QString& name,
        const TValueType& defaultVal = TValueType())
    {
        auto var = props.value(name);
        if ((!var.isValid()) || (!var.canConvert<TValueType>())) {
            return defaultVal;
        }

        return var.value<TValueType>();
    }

private:
    QString m_name;
    bool m_hidden = false;
    bool m_serialisedHidden = false;
    bool m_readOnly = false;
};

template <typename TDerived>
class CommonBase : public Common
{
    typedef Common Base;
public:
    CommonBase() = default;
    CommonBase(const CommonBase&) = default;
    CommonBase(CommonBase&&) = default;
    CommonBase(const QVariantMap& props) : Base(props) {}
    CommonBase(const QVariant& props) : Base(props) {}
    ~CommonBase() = default;

    CommonBase& operator=(const CommonBase&) = default;
    CommonBase& operator=(CommonBase&&) = default;

    using Base::name;

    TDerived& name(const QString& value)
    {
        Base::setName(value);
        return static_cast<TDerived&>(*this);
    }

    TDerived& name(const char* value)
    {
        Base::setName(value);
        return static_cast<TDerived&>(*this);
    }

    TDerived& hidden(bool value = true)
    {
        Base::hidden(value);
        return static_cast<TDerived&>(*this);
    }

    TDerived& serialisedHidden(bool value = true)
    {
        Base::serialisedHidden(value);
        return static_cast<TDerived&>(*this);
    }

    TDerived& readOnly(bool value = true)
    {
        Base::readOnly(value);
        return static_cast<TDerived&>(*this);
    }
};

class CC_API IntValue : public CommonBase<IntValue>
{
    typedef CommonBase<IntValue> Base;
public:
    IntValue();
    IntValue(const IntValue&);
    IntValue(IntValue&&);
    IntValue(const QVariantMap& props);
    IntValue(const QVariant& props);
    ~IntValue();

    IntValue& operator=(const IntValue&);
    IntValue& operator=(IntValue&&);

    long long displayOffset() const;
    IntValue& displayOffset(long long value);

    int scaledDecimals() const;
    bool hasScaledDecimals() const;
    IntValue& scaledDecimals(int value);

    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    long long m_displayOffset = 0;
    int m_scaledDecimals = 0;
};

class CC_API EnumValue : public CommonBase<EnumValue>
{
    typedef CommonBase<EnumValue> Base;
public:

    typedef QPair<QString, long long> ElemType;
    typedef QList<ElemType> ElemsList;

    EnumValue();
    EnumValue(const EnumValue&);
    EnumValue(EnumValue&&);
    EnumValue(const QVariantMap& props);
    EnumValue(const QVariant& props);
    ~EnumValue();

    EnumValue& operator=(const EnumValue&);
    EnumValue& operator=(EnumValue&&);

    const ElemsList& values() const;

    EnumValue& add(const QString& elemName, long long value);
    EnumValue& add(const QString& elemName);

    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    ElemsList m_elems;
};

class CC_API BitmaskValue : public CommonBase<BitmaskValue>
{
    typedef CommonBase<BitmaskValue> Base;
public:

    typedef QVariantList BitsList;

    BitmaskValue();
    BitmaskValue(const BitmaskValue&);
    BitmaskValue(BitmaskValue&&);
    BitmaskValue(const QVariantMap& props);
    BitmaskValue(const QVariant& props);
    ~BitmaskValue();

    BitmaskValue& operator=(const BitmaskValue&);
    BitmaskValue& operator=(BitmaskValue&&);

    const BitsList& bits() const;

    BitmaskValue& add(int idx, const QString& bitName);
    BitmaskValue& add(const QString& bitName);

    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    BitsList m_bits;
};

class CC_API Bitfield : public CommonBase<Bitfield>
{
    typedef CommonBase<Bitfield> Base;
public:

    typedef QList<QVariantMap> MembersList;

    Bitfield();
    Bitfield(const Bitfield&);
    Bitfield(Bitfield&&);
    Bitfield(const QVariantMap& props);
    Bitfield(const QVariant& props);
    ~Bitfield();

    Bitfield& operator=(const Bitfield&);
    Bitfield& operator=(Bitfield&&);

    const MembersList& members() const;

    Bitfield& add(QVariantMap&& memberProps);
    Bitfield& add(const QVariantMap& memberProps);

    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    MembersList m_members;
};

class CC_API Bundle : public CommonBase<Bundle>
{
    typedef CommonBase<Bundle> Base;
public:

    typedef QList<QVariantMap> MembersList;

    Bundle();
    Bundle(const Bundle&);
    Bundle(Bundle&&);
    Bundle(const QVariantMap& props);
    Bundle(const QVariant& props);
    ~Bundle();

    Bundle& operator=(const Bundle&);
    Bundle& operator=(Bundle&&);

    const MembersList& members() const;

    Bundle& add(QVariantMap&& memberProps);
    Bundle& add(const QVariantMap& memberProps);

    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    MembersList m_members;
};

class CC_API String : public CommonBase<String>
{
    typedef CommonBase<String> Base;
public:
    String();
    String(const String&);
    String(String&&);
    String(const QVariantMap& props);
    String(const QVariant& props);
    ~String();

    String& operator=(const String&);
    String& operator=(String&&);

    QVariantMap asMap() const;
};

class CC_API ArrayList : public CommonBase<ArrayList>
{
    typedef CommonBase<ArrayList> Base;
public:
    typedef QList<QVariantMap> ElemsList;

    ArrayList();
    ArrayList(const ArrayList&);
    ArrayList(ArrayList&&);
    ArrayList(const QVariantMap& props);
    ArrayList(const QVariant& props);
    ~ArrayList();

    ArrayList& operator=(const ArrayList&);
    ArrayList& operator=(ArrayList&&);

    const ElemsList& elements() const;
    ArrayList& add(QVariantMap&& elemProps);
    ArrayList& add(const QVariantMap& elemProps);

    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    ElemsList m_elems;
};

class CC_API Optional : public CommonBase<Optional>
{
    typedef CommonBase<Optional> Base;
public:
    Optional();
    Optional(const Optional&);
    Optional(Optional&&);
    Optional(const QVariantMap& props);
    Optional(const QVariant& props);
    ~Optional();

    Optional& operator=(const Optional&);
    Optional& operator=(Optional&&);

    const QVariantMap& field() const;
    Optional& field(QVariantMap&& fieldProps);
    Optional& field(const QVariantMap& fieldProps);

    bool isUncheckable() const;
    Optional& uncheckable(bool value = true);

    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    QVariantMap m_field;
    bool m_uncheckable = false;
};

class CC_API FloatValue : public CommonBase<FloatValue>
{
    typedef CommonBase<FloatValue> Base;
public:
    FloatValue();
    FloatValue(const FloatValue&);
    FloatValue(FloatValue&&);
    FloatValue(const QVariantMap& props);
    FloatValue(const QVariant& props);
    ~FloatValue();

    FloatValue& operator=(const FloatValue&);
    FloatValue& operator=(FloatValue&&);

    int decimals() const;
    FloatValue& decimals(int value);

    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    int m_decimals = 0;
};

class CC_API Variant : public CommonBase<Variant>
{
    typedef CommonBase<Variant> Base;
public:

    typedef QList<QVariantMap> MembersList;

    Variant();
    Variant(const Variant&);
    Variant(Variant&&);
    Variant(const QVariantMap& props);
    Variant(const QVariant& props);
    ~Variant();

    Variant& operator=(const Variant&);
    Variant& operator=(Variant&&);

    const MembersList& members() const;

    Variant& add(QVariantMap&& memberProps);
    Variant& add(const QVariantMap& memberProps);

    bool isIndexHidden() const;
    Variant& setIndexHidden(bool hidden = true);

    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    MembersList m_members;
    bool m_indexHidden = false;
};

class CC_API NoValue : public CommonBase<NoValue>
{
    typedef CommonBase<NoValue> Base;
public:
    NoValue();
    NoValue(const NoValue&);
    NoValue(NoValue&&);
    NoValue(const QVariantMap& props);
    NoValue(const QVariant& props);
    ~NoValue();

    NoValue& operator=(const NoValue&);
    NoValue& operator=(NoValue&&);

    QVariantMap asMap() const;
};


namespace details
{

template <typename TField>
struct ForTag;

template <>
struct ForTag<comms::field::tag::Int>
{
    typedef comms_champion::property::field::IntValue Type;
};

template <>
struct ForTag<comms::field::tag::Enum>
{
    typedef comms_champion::property::field::EnumValue Type;
};

template <>
struct ForTag<comms::field::tag::Bitmask>
{
    typedef comms_champion::property::field::BitmaskValue Type;
};

template <>
struct ForTag<comms::field::tag::Bitfield>
{
    typedef comms_champion::property::field::Bitfield Type;
};

template <>
struct ForTag<comms::field::tag::Bundle>
{
    typedef comms_champion::property::field::Bundle Type;
};

template <>
struct ForTag<comms::field::tag::String>
{
    typedef comms_champion::property::field::String Type;
};

template <>
struct ForTag<comms::field::tag::RawArrayList>
{
    typedef comms_champion::property::field::ArrayList Type;
};

template <>
struct ForTag<comms::field::tag::ArrayList>
{
    typedef comms_champion::property::field::ArrayList Type;
};

template <>
struct ForTag<comms::field::tag::Optional>
{
    typedef comms_champion::property::field::Optional Type;
};

template <>
struct ForTag<comms::field::tag::Float>
{
    typedef comms_champion::property::field::FloatValue Type;
};

template <>
struct ForTag<comms::field::tag::Variant>
{
    typedef comms_champion::property::field::Variant Type;
};

template <>
struct ForTag<comms::field::tag::NoValue>
{
    typedef comms_champion::property::field::NoValue Type;
};

}  // namespace details

template <typename TField>
using ForField = typename details::ForTag<typename TField::Tag>::Type;

}  // namespace field

}  // namespace property

}  // namespace comms_champion

