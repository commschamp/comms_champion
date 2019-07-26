//
// Copyright 2016 - 2019 (C). Alex Robenko. All rights reserved.
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

/// @brief Common set of properties
/// @details Common base class for all other properties description classes
/// @headerfile comms_champion/property/field.h
class CC_API Common
{
public:
    /// @brief Default constructor
    Common();

    /// @brief Copy constructor
    Common(const Common&);

    /// @brief Move constructor
    Common(Common&&);

    /// @brief Construct from QVariantMap
    explicit Common(const QVariantMap& props);

    /// @brief Construct from QVariant that contains QVariantMap.
    explicit Common(const QVariant& props);

    /// @brief Destructor
    ~Common() noexcept;

    /// @brief Copy assignment operator
    Common& operator=(const Common&);

    /// @brief Move assignment operator
    Common& operator=(Common&&);

    /// @brief Get name of the field
    const QString& name() const;

    /// @brief Update the name value
    void setName(const QString& value);

    /// @brief Update the name value
    void setName(const char* value);

    /// @brief Check the field is hidden
    bool isHidden() const;

    /// @brief Set whether the field is hidden
    Common& hidden(bool value = true);

    /// @brief Check the serialisation part is hidden
    bool isSerialisedHidden() const;

    /// @brief Set whether the serialised part is hidden
    Common& serialisedHidden(bool value = true);

    /// @brief Check whether the field cannot be modified.
    bool isReadOnly() const;

    /// @brief Set whether the field cannot be modified
    Common& readOnly(bool value = true);

    /// @brief Check the field is hidden when readOnly
    bool isHiddenWhenReadOnly() const;

    /// @brief Set whether the field must be hidden when cannot be modified
    Common& hiddenWhenReadOnly(bool value = true);

    /// @brief Copy all the properties value into provided properties map
    void setTo(QVariantMap& props) const;

    /// @brief Read the properties values from the provided map
    void getFrom(const QVariantMap& props);

protected:

    /// @brief set element value to the map
    /// @param[in] val Value to set
    /// @param[in] name Name of the property
    /// @param[in, out] Map to update
    template <typename U>
    static void setElemTo(U&& val, const QString& name, QVariantMap& props)
    {
        props.insert(name, QVariant::fromValue(std::forward<U>(val)));
    }

    /// @brief read element value from the map
    /// @param[in] props Properties map
    /// @param[in] name Property name
    /// @param[in] defaultValue Value to return if not found in map
    /// @return Value of found property in the map
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
    bool m_hiddenWhenReadOnly = false;
};

/// @brief Intermediate helper class to define properties describing one
/// @tparam TDerived Actual derived class
/// @headerfile comms_champion/property/field.h
template <typename TDerived>
class CommonBase : public Common
{
    using Base = Common;
public:
    /// @brief Default constructor
    CommonBase() = default;

    /// @brief Copy constructor
    CommonBase(const CommonBase&) = default;

    /// @brief Move constructor
    CommonBase(CommonBase&&) = default;

    /// @brief Construct from QVariantMap
    explicit CommonBase(const QVariantMap& props) : Base(props) {}

    /// @brief Construct from QVariant that contains QVariantMap.
    explicit CommonBase(const QVariant& props) : Base(props) {}

    /// @brief Destructor
    ~CommonBase() noexcept = default;

    /// @brief Copy assignment
    CommonBase& operator=(const CommonBase&) = default;

    /// @brief Move assignment
    CommonBase& operator=(CommonBase&&) = default;

    using Base::name;

    /// @brief Set name value
    /// @return reference to derived class
    TDerived& name(const QString& value)
    {
        Base::setName(value);
        return static_cast<TDerived&>(*this);
    }

    /// @brief Set name value
    /// @return reference to derived class
    TDerived& name(const char* value)
    {
        Base::setName(value);
        return static_cast<TDerived&>(*this);
    }

    /// @brief Set whether the field is hidden
    /// @return reference to derived class
    TDerived& hidden(bool value = true)
    {
        return static_cast<TDerived&>(Base::hidden(value));
    }

    /// @brief Set whether the serialised part is hidden
    /// @return reference to derived class
    TDerived& serialisedHidden(bool value = true)
    {
        return static_cast<TDerived&>(Base::serialisedHidden(value));
    }

    /// @brief Set whether the field cannot be modified
    /// @return reference to derived class
    TDerived& readOnly(bool value = true)
    {
        return static_cast<TDerived&>(Base::readOnly(value));
    }

    /// @brief Set whether the field must be hidden when cannot be modified
    /// @return reference to derived class
    TDerived& hiddenWhenReadOnly(bool value = true)
    {
        return static_cast<TDerived&>(Base::hiddenWhenReadOnly(value));
    }
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::IntValue fields.
/// @headerfile comms_champion/property/field.h
class CC_API IntValue : public CommonBase<IntValue>
{
    using Base = CommonBase<IntValue>;
public:
    /// @brief Default constructor
    IntValue();

    /// @brief Copy constructor
    IntValue(const IntValue&);

    /// @brief Move constructor
    IntValue(IntValue&&);

    /// @brief Construct from QVariantMap
    IntValue(const QVariantMap& props);

    /// @brief Construct from QVariant that contains QVariantMap.
    IntValue(const QVariant& props);

    /// @brief Destructor
    ~IntValue() noexcept;

    /// @brief Copy assignement
    IntValue& operator=(const IntValue&);

    /// @brief Move assignment
    IntValue& operator=(IntValue&&);

    /// @brief Get numeric offset of the displayed field
    long long displayOffset() const;

    /// @brief Set numeric offset for displayed field
    IntValue& displayOffset(long long value);

    /// @brief Check whether property of having scaled decimals being set
    bool hasScaledDecimals() const;

    /// @brief Get number of digits after decimal point when displaying
    ///     scaled value.
    int scaledDecimals() const;

    /// @brief Set number of digits after decimal point when displaying scaled
    ///     value.
    IntValue& scaledDecimals(int value);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    long long m_displayOffset = 0;
    int m_scaledDecimals = 0;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::EnumValue fields.
/// @headerfile comms_champion/property/field.h
class CC_API EnumValue : public CommonBase<EnumValue>
{
    using Base = CommonBase<EnumValue>;
public:

    /// @brief The enum value is described as string containing name and
    ///     the actual numeric value.
    using ElemType = QPair<QString, long long>;

    /// @brief List of properties describing enum value
    using ElemsList = QList<ElemType>;

    /// @brief Default constructor
    EnumValue();

    /// @brief Copy constructor
    EnumValue(const EnumValue&);

    /// @brief Move constructor
    EnumValue(EnumValue&&);

    /// @brief Construct from QVariantMap
    EnumValue(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    EnumValue(const QVariant& props);

    /// @brief Destructor
    ~EnumValue() noexcept;

    /// @brief Copy assignement
    EnumValue& operator=(const EnumValue&);

    /// @brief Move assignment
    EnumValue& operator=(EnumValue&&);

    /// @brief Get access to all the values information
    const ElemsList& values() const;

    /// @brief Add value description
    /// @param[in] elemName Name of the value.
    /// @param[in] value Numeric value
    EnumValue& add(const QString& elemName, long long value);

    /// @brief Add value description
    /// @details The assigned numeric value is the last inserted one incremented
    ///     by 1. This function is convenient to use when describing enum
    ///     with sequential values.
    /// @param[in] elemName Name of the value.
    EnumValue& add(const QString& elemName);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    ElemsList m_elems;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::BitmaskValue fields.
/// @headerfile comms_champion/property/field.h
class CC_API BitmaskValue : public CommonBase<BitmaskValue>
{
    using Base = CommonBase<BitmaskValue>;
public:

    /// @brief List of bits descriptions.
    using BitsList = QVariantList;

    /// @brief Default constructor
    BitmaskValue();

    /// @brief Copy constructor
    BitmaskValue(const BitmaskValue&);

    /// @brief Move constructor
    BitmaskValue(BitmaskValue&&);

    /// @brief Construct from QVariantMap
    BitmaskValue(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    BitmaskValue(const QVariant& props);

    /// @brief Destructor
    ~BitmaskValue() noexcept;

    /// @brief Copy assignment
    BitmaskValue& operator=(const BitmaskValue&);

    /// @brief Move assignment
    BitmaskValue& operator=(BitmaskValue&&);

    /// @brief Get access to bits information
    const BitsList& bits() const;

    /// @brief Add bit description
    /// @param[in] idx Bit index
    /// @param[in] bitName Name of the bit
    BitmaskValue& add(int idx, const QString& bitName);

    /// @brief Add bit description.
    /// @param[in] The bit index is assumed to be the last inserted one
    ///     incremented by 1.
    /// @param[in] bitName Name of the bit
    BitmaskValue& add(const QString& bitName);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    BitsList m_bits;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::Bitfield fields.
/// @headerfile comms_champion/property/field.h
class CC_API Bitfield : public CommonBase<Bitfield>
{
    using Base = CommonBase<Bitfield>;
public:

    /// @brief Properties of contained fields.
    using MembersList = QList<QVariantMap>;

    /// @brief Default constructor
    Bitfield();

    /// @brief Copy constructor
    Bitfield(const Bitfield&);

    /// @brief Move constructor
    Bitfield(Bitfield&&);

    /// @brief Construct from QVariantMap
    Bitfield(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    Bitfield(const QVariant& props);

    /// @brief Desctructor
    ~Bitfield() noexcept;

    /// @brief Copy assignment
    Bitfield& operator=(const Bitfield&);

    /// @brief Move assignment
    Bitfield& operator=(Bitfield&&);

    /// @brief Get access to the properties of contained members
    const MembersList& members() const;

    /// @brief Add properties of the next member
    Bitfield& add(QVariantMap&& memberProps);

    /// @brief Add properties of the next member
    Bitfield& add(const QVariantMap& memberProps);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    MembersList m_members;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::Bundle fields.
/// @headerfile comms_champion/property/field.h
class CC_API Bundle : public CommonBase<Bundle>
{
    using Base = CommonBase<Bundle>;
public:

    /// @brief Properties of contained fields.
    using MembersList = QList<QVariantMap>;

    /// @brief Default constructor
    Bundle();

    /// @brief Copy constructor
    Bundle(const Bundle&);

    /// @brief Move constructor
    Bundle(Bundle&&);

    /// @brief Construct from QVariantMap
    Bundle(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    Bundle(const QVariant& props);

    ///  @brief Destructor
    ~Bundle() noexcept;

    /// @brief Copy assignment
    Bundle& operator=(const Bundle&);

    /// @brief Move assignment
    Bundle& operator=(Bundle&&);

    /// @brief Get access to the properties of contained members
    const MembersList& members() const;

    /// @brief Add properties of the next member
    Bundle& add(QVariantMap&& memberProps);

    /// @brief Add properties of the next member
    Bundle& add(const QVariantMap& memberProps);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    MembersList m_members;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::String fields.
/// @headerfile comms_champion/property/field.h
class CC_API String : public CommonBase<String>
{
    typedef CommonBase<String> Base;
public:
    /// @brief Default constructor
    String();

    /// @brief Copy constructor
    String(const String&);

    /// @brief Move constructor
    String(String&&);

    /// @brief Construct from QVariantMap
    String(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    String(const QVariant& props);

    /// @brief Destructor
    ~String() noexcept;

    /// @brief Copy assignment
    String& operator=(const String&);

    /// @brief Move assignement
    String& operator=(String&&);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::ArrayList fields.
/// @headerfile comms_champion/property/field.h
class CC_API ArrayList : public CommonBase<ArrayList>
{
    typedef CommonBase<ArrayList> Base;
public:
    /// @brief List of data elements' properties.
    using ElemsList = QList<QVariantMap>;

    /// @brief Default constructor
    ArrayList();

    /// @brief Copy constructor
    ArrayList(const ArrayList&);

    /// @brief Move constructor
    ArrayList(ArrayList&&);

    /// @brief Construct from QVariantMap
    ArrayList(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    ArrayList(const QVariant& props);

    /// @brief Destructor
    ~ArrayList() noexcept;

    /// @brief Copy assignment
    ArrayList& operator=(const ArrayList&);

    /// @brief Move assignment
    ArrayList& operator=(ArrayList&&);

    /// @brief Get access to the properties of elements.
    const ElemsList& elements() const;

    /// @brief Add properties of the next element.
    ArrayList& add(QVariantMap&& elemProps);

    /// @brief Add properties of the next element.
    ArrayList& add(const QVariantMap& elemProps);

    /// @brief Check whether the size/length prefix field should be displayed
    ///     separately.
    bool isPrefixVisible() const;

    /// @brief Set whether the size/length prefix field should be displayed
    ///     separately.
    ArrayList& showPrefix(bool value = true);

    /// @brief Get name of the size/length prefix displayed separately.
    const QString& prefixName() const;

    /// @brief Set name of the size/length prefix displayed separately.
    ArrayList& prefixName(const QString& nameParam);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    ElemsList m_elems;
    bool m_showPrefix = false;
    QString m_prefixName;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::Optional fields.
/// @headerfile comms_champion/property/field.h
class CC_API Optional : public CommonBase<Optional>
{
    typedef CommonBase<Optional> Base;
public:

    /// @brief Default constructor
    Optional();

    /// @brief Copy constructor
    Optional(const Optional&);

    /// @brief Move constructor
    Optional(Optional&&);

    /// @brief Construct from QVariantMap
    Optional(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    Optional(const QVariant& props);

    /// @brief Destructor
    ~Optional() noexcept;

    /// @brief Copy assignment
    Optional& operator=(const Optional&);

    /// @brief Move assignment
    Optional& operator=(Optional&&);

    /// @brief Get access to contained field's properties
    const QVariantMap& field() const;

    /// @brief Set contained field's properties.
    Optional& field(QVariantMap&& fieldProps);

    /// @brief Set contained field's properties.
    Optional& field(const QVariantMap& fieldProps);

    /// @brief Check field is uncheckable.
    /// @details Uncheckable means that the user cannot manually mark the field
    ///     as existing/missing independently.
    bool isUncheckable() const;

    /// @brief (Un)Mark the field uncheckable.
    Optional& uncheckable(bool value = true);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    QVariantMap m_field;
    bool m_uncheckable = false;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::FloatValue fields.
/// @headerfile comms_champion/property/field.h
class CC_API FloatValue : public CommonBase<FloatValue>
{
    typedef CommonBase<FloatValue> Base;
public:
    /// @brief Default constructor
    FloatValue();

    /// @brief Copy constructor
    FloatValue(const FloatValue&);

    /// @brief Move constructor
    FloatValue(FloatValue&&);

    /// @brief Construct from QVariantMap
    FloatValue(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    FloatValue(const QVariant& props);

    /// @brief Destructor
    ~FloatValue() noexcept;

    /// @brief Copy assignment
    FloatValue& operator=(const FloatValue&);

    /// @brief Move assignment
    FloatValue& operator=(FloatValue&&);

    /// @brief Get number of decimal digits to display
    int decimals() const;

    /// @brief Set number of decimal digits to display
    FloatValue& decimals(int value);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;

private:
    void getFrom(const QVariantMap& props);

    int m_decimals = 0;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::Variant fields.
/// @headerfile comms_champion/property/field.h
class CC_API Variant : public CommonBase<Variant>
{
    typedef CommonBase<Variant> Base;
public:

    /// @brief List of properties of the contained fields.
    using MembersList = QList<QVariantMap>;

    /// @brief Default constructor
    Variant();

    /// @brief Copy constructor
    Variant(const Variant&);

    /// @brief Move constructor
    Variant(Variant&&);

    /// @brief Construct from QVariantMap
    Variant(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    Variant(const QVariant& props);

    /// @brief Destructor
    ~Variant() noexcept;

    /// @brief Copy assignment
    Variant& operator=(const Variant&);

    /// @brief Move assignment
    Variant& operator=(Variant&&);

    /// @brief Get access to the contained fields' properties.
    const MembersList& members() const;

    /// @brief Add properties of the next member
    Variant& add(QVariantMap&& memberProps);

    /// @brief Add properties of the next member.
    Variant& add(const QVariantMap& memberProps);

    /// @brief Check the member index should be hidden when displaying field.
    bool isIndexHidden() const;

    /// @brief Set the member index should be hidden when displaying field.
    Variant& setIndexHidden(bool hiddenVal = true);

    /// @brief Retrieve all properties as map.
    QVariantMap asMap() const;
private:
    void getFrom(const QVariantMap& props);

    MembersList m_members;
    bool m_indexHidden = false;
};

/// @brief Class to contain all the properties relevant to
///     @b comms::field::NoValue fields.
/// @headerfile comms_champion/property/field.h
class CC_API NoValue : public CommonBase<NoValue>
{
    typedef CommonBase<NoValue> Base;
public:
    /// @brief Default constructor
    NoValue();

    /// @brief Copy constructor
    NoValue(const NoValue&);

    /// @brief Move constructor
    NoValue(NoValue&&);

    /// @brief Construct from QVariantMap
    NoValue(const QVariantMap& props);

    /// @brief Construct from QVariant containing QVariantMap
    NoValue(const QVariant& props);

    /// @brief Descructor
    ~NoValue() noexcept;

    /// @brief Copy assignment
    NoValue& operator=(const NoValue&);

    /// @brief Move assignment
    NoValue& operator=(NoValue&&);

    /// @brief Retrieve all properties as map.
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

/// @brief Get proper properties management class for field.
/// @tparam TField Type of the field
/// @headerfile comms_champion/property/field.h
template <typename TField>
using ForField = typename details::ForTag<typename TField::Tag>::Type;

}  // namespace field

}  // namespace property

}  // namespace comms_champion

