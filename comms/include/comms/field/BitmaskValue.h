//
// Copyright 2014 - 2019 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

#include <limits>
#include "comms/Field.h"

#include "comms/util/SizeToType.h"
#include "details/AdaptBasicField.h"
#include "details/OptionsParser.h"
#include "comms/details/gen_enum.h"
#include "comms/details/bits_access.h"
#include "IntValue.h"
#include "tag.h"

namespace comms
{

namespace field
{

namespace details
{

template <bool THasFixedLength>
struct BitmaskUndertlyingType;

template <>
struct BitmaskUndertlyingType<true>
{
    template <typename TOptionsBundle>
    using Type = typename comms::util::SizeToType<TOptionsBundle::FixedLength, false>::Type;
};

template <>
struct BitmaskUndertlyingType<false>
{
    template <typename TOptionsBundle>
    using Type = unsigned;
};

template <typename TOptionsBundle>
using BitmaskUndertlyingTypeT =
    typename BitmaskUndertlyingType<TOptionsBundle::HasFixedLengthLimit>::template Type<TOptionsBundle>;


}  // namespace details

/// @brief Bitmask value field.
/// @details Quite often communication protocols specify bitmask values, where
///     any bit has a specific meaning. Although such masks are can be handled
///     as unsigned integer values using comms::field::IntValue field type,
///     using comms::field::Bitmask may be a bit more convenient.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field. If no option is provided, the underlying type is assumed
///     to be "unsigned", which is usually 4 bytes long. To redefined the length
///     of the bitmask field, use @ref comms::option::def::FixedLength option.
///     For example:
///     @code
///         using MyFieldBase = comms::Field<comms::option::def::BigEndian>;
///         using MyField =comms::field::EnumValue<MyFieldBase>;
///     @endcode
///     The serialised value of the field in the example above will consume
///     sizeof(unsigned) bytes, because the underlying type chosen to be "unsigned"
///     by default. Example below specifies simple bitmask value field with
///     2 bytes serialisation length:
///     @code
///         using MyFieldBase = comms::Field<comms::option::def::BigEndian>;
///         using MyField =comms::field::EnumValue<MyFieldBase, comms::option::def::FixedLength<2> >;
///     @endcode
///     Supported options are:
///     @li @ref comms::option::def::FixedLength
///     @li @ref comms::option::def::FixedBitLength
///     @li @ref comms::option::def::DefaultValueInitialiser or comms::option::def::DefaultNumValue.
///     @li @ref comms::option::def::ContentsValidator or comms::option::def::BitmaskReservedBits.
///     @li @ref comms::option::def::ContentsRefresher
///     @li @ref comms::option::def::HasCustomRead
///     @li @ref comms::option::def::HasCustomRefresh
///     @li @ref comms::option::def::FailOnInvalid
///     @li @ref comms::option::def::IgnoreInvalid
///     @li @ref comms::option::def::EmptySerialization
///     @li @ref comms::option::def::VersionStorage
/// @extends comms::Field
/// @headerfile comms/field/BitmaskValue.h
/// @see COMMS_BITMASK_BITS()
/// @see COMMS_BITMASK_BITS_ACCESS()
/// @see COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE()
/// @see COMMS_BITMASK_BITS_SEQ()
/// @see COMMS_BITMASK_BITS_SEQ_NOTEMPLATE()
template <typename TFieldBase, typename... TOptions>
class BitmaskValue : public TFieldBase
{
    using BaseImpl = TFieldBase;

    using OptionsBundle = details::OptionsParser<TOptions...>;

    using IntValueType = details::BitmaskUndertlyingTypeT<OptionsBundle>;

    using IntValueField =
        IntValue<
            TFieldBase,
            IntValueType,
            TOptions...
        >;

public:

    /// @brief Endian used for serialisation.
    using Endian = typename BaseImpl::Endian;

    /// @brief Version type
    using VersionType = typename BaseImpl::VersionType;

    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = OptionsBundle;

    /// @brief Tag indicating type of the field
    using Tag = tag::Bitmask;

    /// @brief Type of underlying integral value.
    /// @details Unsigned integral type, which depends on the length of the
    ///     mask determined by the @ref comms::option::def::FixedLength option.
    using ValueType = typename IntValueField::ValueType;

    /// @brief Default constructor.
    /// @brief Initial bitmask has all bits cleared (equals 0)
    BitmaskValue() = default;

    /// @brief Constructor
    /// @param[in] val Value of the field to initialise it with.
    explicit BitmaskValue(const ValueType& val)
       : intValue_(val)
    {
    }

    /// @brief Copy constructor
    BitmaskValue(const BitmaskValue&) = default;

    /// @brief Destructor
    ~BitmaskValue() noexcept = default;

    /// @brief Copy assignment
    BitmaskValue& operator=(const BitmaskValue&) = default;

    /// @brief Get access to underlying mask value storage.
    /// @return Const reference to the underlying stored value.
    const ValueType& value() const
    {
        return intValue_.value();
    }

    /// @brief Get access to underlying mask value storage.
    /// @return Reference to the underlying stored value.
    ValueType& value()
    {
        return intValue_.value();
    }

    /// @brief Get length required to serialise the current field value.
    /// @return Number of bytes it will take to serialise the field value.
    constexpr std::size_t length() const
    {
        return intValue_.length();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @return Maximal number of bytes required serialise the field value.
    static constexpr std::size_t maxLength()
    {
        return IntValueField::maxLength();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    /// @return Minimal number of bytes required serialise the field value.
    static constexpr std::size_t minLength()
    {
        return IntValueField::minLength();
    }

    /// @brief Read field value from input data sequence
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return intValue_.read(iter, size);
    }

    /// @brief Read field value from input data sequence without error check and status report.
    /// @details Similar to @ref read(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to read the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        intValue_.readNoStatus(iter);
    }

    /// @brief Write current field value to output data sequence
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return intValue_.write(iter, size);
    }

    /// @brief Write current field value to output data sequence  without error check and status report.
    /// @details Similar to @ref write(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to write the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        intValue_.writeNoStatus(iter);
    }

    /// @brief Check validity of the field value.
    constexpr bool valid() const
    {
        return intValue_.valid();
    }

    /// @brief Refresh contents of the field
    /// @return @b true in case the field's value has been updated, @b false otherwise
    bool refresh()
    {
        return intValue_.refresh();
    }

    /// @brief Check whether all bits from provided mask are set.
    /// @param[in] mask Mask to check against
    /// @return true in case all the bits are set, false otherwise
    bool hasAllBitsSet(ValueType mask) const
    {
        return (value() & mask) == mask;
    }

    /// @brief Check whether any bits from provided mask are set.
    /// @param[in] mask Mask to check against
    /// @return true in case at least one of the bits is set, false otherwise.
    bool hasAnyBitsSet(ValueType mask) const
    {
        return (value() & mask) != 0;
    }

    /// @brief Set all the provided bits.
    /// @details Equivalent to @code value() |= mask; @endcode
    /// @param[in] mask Mask of bits to set.
    void setBits(ValueType mask)
    {
        value() |= mask;
    }

    /// @brief Set all the provided bits.
    /// @details Equivalent to @code value() &= (~mask); @endcode
    /// @param[in] mask Mask of bits to clear.
    void clearBits(ValueType mask)
    {
        value() &= (~mask);
    }

    /// @brief Get bit value
    bool getBitValue(unsigned bitNum) const
    {
        return hasAllBitsSet(
            static_cast<ValueType>(1U) << bitNum);
    }

    /// @brief Set bit value
    void setBitValue(unsigned bitNum, bool val)
    {
        auto mask = static_cast<ValueType>(1U) << bitNum;
        if (val) {
            setBits(mask);
        }
        else {
            clearBits(mask);
        }
    }

    /// @brief Compile time check if this class is version dependent
    static constexpr bool isVersionDependent()
    {
        return IntValueField::isVersionDependent();
    }

    /// @brief Compile time check if this class has non-default refresh functionality
    static constexpr bool hasNonDefaultRefresh()
    {
        return BaseImpl::hasNonDefaultRefresh();
    }

    /// @brief Get version of the field.
    /// @details Exists only if @ref comms::option::def::VersionStorage option has been provided.
    VersionType getVersion() const
    {
        return intValue_.getVersion();
    }

    /// @brief Default implementation of version update.
    /// @return @b true in case the field contents have changed, @b false otherwise
    bool setVersion(VersionType version)
    {
        return intValue_.setVersion(version);
    }

protected:
    using BaseImpl::readData;
    using BaseImpl::writeData;

private:

    static_assert(!ParsedOptions::HasSerOffset,
        "comms::option::def::NumValueSerOffset option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasVarLengthLimits,
        "comms::option::def::VarLength option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceElemLengthForcing,
        "comms::option::def::SequenceElemLengthForcingEnabled option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceSizeForcing,
        "comms::option::def::SequenceSizeForcingEnabled option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceLengthForcing,
        "comms::option::def::SequenceLengthForcingEnabled option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceFixedSize,
        "comms::option::def::SequenceFixedSize option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceFixedSizeUseFixedSizeStorage,
        "comms::option::app::SequenceFixedSizeUseFixedSizeStorage option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceSizeFieldPrefix,
        "comms::option::def::SequenceSizeFieldPrefix option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceSerLengthFieldPrefix,
        "comms::option::def::SequenceSerLengthFieldPrefix option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceElemSerLengthFieldPrefix,
        "comms::option::def::SequenceElemSerLengthFieldPrefix option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceElemFixedSerLengthFieldPrefix,
        "comms::option::def::SequenceElemSerLengthFixedFieldPrefix option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceTrailingFieldSuffix,
        "comms::option::def::SequenceTrailingFieldSuffix option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasSequenceTerminationFieldSuffix,
        "comms::option::def::SequenceTerminationFieldSuffix option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasFixedSizeStorage,
        "comms::option::app::FixedSizeStorage option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasCustomStorageType,
        "comms::option::app::CustomStorageType option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasScalingRatio,
        "comms::option::def::ScalingRatio option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasUnits,
        "comms::option::def::Units option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasOrigDataView,
        "comms::option::app::OrigDataView option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasMultiRangeValidation,
        "comms::option::def::ValidNumValueRange (or similar) option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasVersionsRange,
        "comms::option::def::ExistsBetweenVersions (or similar) option is not applicable to BitmaskValue field");
    static_assert(!ParsedOptions::HasInvalidByDefault,
        "comms::option::def::InvalidByDefault option is not applicable to BitmaskValue field");

    IntValueField intValue_;
};

// Implementation

/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are equal, false otherwise.
/// @related BitmaskValue
template <typename TFieldBase, typename... TOptions>
bool operator==(
    const BitmaskValue<TFieldBase, TOptions...>& field1,
    const BitmaskValue<TFieldBase, TOptions...>& field2)
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are NOT equal, false otherwise.
/// @related BitmaskValue
template <typename TFieldBase, typename... TOptions>
bool operator!=(
    const BitmaskValue<TFieldBase, TOptions...>& field1,
    const BitmaskValue<TFieldBase, TOptions...>& field2)
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case value of the first field is lower than than the value of the second.
/// @related BitmaskValue
template <typename TFieldBase, typename... TOptions>
bool operator<(
    const BitmaskValue<TFieldBase, TOptions...>& field1,
    const BitmaskValue<TFieldBase, TOptions...>& field2)
{
    return field1.value() < field2.value();
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::BitmaskValue.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref BitmaskValue
/// @related comms::field::BitmaskValue
template <typename T>
constexpr bool isBitmaskValue()
{
    return std::is_same<typename T::Tag, tag::Bitmask>::value;
}

/// @brief Upcast type of the field definition to its parent comms::field::BitmaskValue type
///     in order to have access to its internal types.
/// @related comms::field::BitmaskValue
template <typename TFieldBase, typename... TOptions>
inline
BitmaskValue<TFieldBase, TOptions...>&
toFieldBase(BitmaskValue<TFieldBase, TOptions...>& field)
{
    return field;
}

/// @brief Upcast type of the field definition to its parent comms::field::BitmaskValue type
///     in order to have access to its internal types.
/// @related comms::field::BitmaskValue
template <typename TFieldBase, typename... TOptions>
inline
const BitmaskValue<TFieldBase, TOptions...>&
toFieldBase(const BitmaskValue<TFieldBase, TOptions...>& field)
{
    return field;
}


/// @brief Provide names for bits in comms::field::BitmaskValue field.
/// @details Defines BitIdx enum with all the provided values prefixed with
///     "BitIdx_". For example usage of
///     @code
///     COMMS_BITMASK_BITS(first, second, third, fourth);
///     @endcode
///     will generate the following enum type:
///     @code
///     enum BitIdx
///     {
///         BitIdx_first,
///         BitIdx_second,
///         BitIdx_third,
///         BitIdx_fourth,
///         BitIdx_numOfValues
///     };
///     @endcode
///     @b NOTE, that provided names @b first, @b second, @b third, and @b fourth have
///     found their way to the enum @b BitIdx. @n
///     Also note, that there is automatically added @b BitIdx_nameOfValues
///     value to the end of @b BitIdx enum.
///
///     It is possible to assign values to the provided names. It could be useful
///     when skipping some unused bits. For example
///     @code
///     COMMS_BITMASK_BITS(first=1, third=3, fourth);
///     @endcode
///     will generate the following enum type:
///     @code
///     enum BitIdx
///     {
///         BitIdx_first=1,
///         BitIdx_third=3,
///         BitIdx_fourth,
///         BitIdx_numOfValues
///     };
///     @endcode
///
///     The macro COMMS_BITMASK_BITS() should be used inside definition of the
///     bitmask field to provide names for the bits for external use:
///     @code
///     struct MyField : public comms::field::BitmaskValue<...>
///     {
///         COMMS_BITMASK_BITS(first, second, third, fourth);
///     }
///     @endcode
/// @related comms::field::BitmaskValue
#define COMMS_BITMASK_BITS(...) COMMS_DEFINE_ENUM(BitIdx, __VA_ARGS__)

/// @brief Generate access functions for bits in comms::field::BitmaskValue field.
/// @details The @ref COMMS_BITMASK_BITS() macro defines @b BitIdx enum to
///     be able to access internal bits. However, an ability to provide
///     values to the enumeration values using @b =val suffixes doesn't
///     allow generation of convenience access functions to the bits. That's
///     why COMMS_BITMASK_BITS_ACCESS() macro was introduced. For every name
///     listed in the parameters list, @b getBitValue_*() and @b setBitValue_*()
///     functions will be generated. For example, having the following definition
///     @code
///     struct MyField : public comms::field::BitmaskValue<...>
///     {
///         ...
///         COMMS_BITMASK_BITS_ACCESS(first, third, fourth);
///     }
///     @endcode
///     is equivalent to having following functions defined:
///     @code
///     struct MyField : public comms::field::BitmaskValue<...>
///     {
///         ...
///         bool getBitValue_first() const {
///             return getBitValue(BitIdx_first);
///         }
///
///         void setBitValue_first(bool value) {
///             setBitValue(BitIdx_first, value);
///         }
///
///         bool getBitValue_third() const {
///             return getBitValue(BitIdx_third);
///         }
///
///         void setBitValue_third(bool value) {
///             setBitValue(BitIdx_third, value);
///         }
///
///         bool getBitValue_fourth() const {
///             return getBitValue(BitIdx_fourth);
///         }
///
///         void setBitValue_fourth(bool value) {
///             setBitValue(BitIdx_fourth, value);
///         }
///     }
///     @endcode
///     @b NOTE, that generated @b getBitValue_*() and @b setBitValue_*()
///     functions use @b BitIdx_* enum values generated by
///     @ref COMMS_BITMASK_BITS(). It means that COMMS_BITMASK_BITS_ACCESS()
///     macro can NOT be used without @ref COMMS_BITMASK_BITS().
///     @code
///     struct MyField : public comms::field::BitmaskValue<...>
///     {
///         COMMS_BITMASK_BITS(first, third=2, fourth);
///         COMMS_BITMASK_BITS_ACCESS(first, third, fourth);
///     }
///     @endcode
/// @pre Must be used together with @ref COMMS_BITMASK_BITS()
/// @related comms::field::BitmaskValue
/// @warning Some compilers, such as @b clang or early versions of @b g++
///     may have problems compiling code generated by this macro even
///     though it uses valid C++11 constructs in attempt to automatically identify the
///     type of the base class. If the compilation fails,
///     and this macro resides inside a @b NON-template class, please use
///     @ref COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE() macro instead. In
///     case this macro needs to reside inside a @b template class, then
///     there is a need to define inner @b Base type, which specifies
///     exact type of the @ref comms::field::BitmaskValue class. For example:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     template <typename... TExtraOptions>
///     class MyField : public
///         comms::field::BitmaskValue<
///             MyFieldBase,
///             comms::field::FixedLength<1>,
///             comms::field::BitmaskReservedBits<0xf2, 0>,
///             TExtraOptions...
///         >
///     {
///         // Duplicate definition of the base class
///         using Base =
///             comms::field::BitmaskValue<
///                 MyFieldBase,
///                 comms::field::FixedLength<1>,
///                 scomms::field::BitmaskReservedBits<0xf2, 0>,
///                 TExtraOptions...
///             >;
///     public:
///         COMMS_BITMASK_BITS(first, third=2, fourth);
///         COMMS_BITMASK_BITS_ACCESS(first, third, fourth);
///     }
///     @endcode
#define COMMS_BITMASK_BITS_ACCESS(...) \
    COMMS_AS_BITMASK_FUNC { \
        return comms::field::toFieldBase(*this); \
    }\
    COMMS_AS_BITMASK_CONST_FUNC { \
        return comms::field::toFieldBase(*this); \
    } \
    COMMS_DO_BIT_ACC_FUNC(asBitmask(), __VA_ARGS__)

/// @brief Similar to @ref COMMS_BITMASK_BITS_ACCESS(), but dedicated for
///     non-template classes.
/// @details The @ref COMMS_BITMASK_BITS_ACCESS() macro is a generic one,
///     which can be used in any class (template, or non-template). However,
///     some compilers (such as <b>g++-4.9</b> and below, @b clang-4.0 and below) may fail
///     to compile it even though it uses valid C++11 constructs. If the
///     compilation fails and the class it is being used in is @b NOT a
///     template one, please use @ref COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE()
///     instead.
/// @related comms::field::BitmaskValue
#define COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE(...) \
    COMMS_DO_BIT_ACC_FUNC((*this), __VA_ARGS__)


/// @brief Combine usage of @ref COMMS_BITMASK_BITS() and @ref COMMS_BITMASK_BITS_ACCESS().
/// @details When assigned bit names start at bit 0 and go sequentially without
///     any gaps in the middle, i.e. don't have any @b =val suffixes, then use
///     COMMS_BITMASK_BITS_SEQ() macro to name the bits. It is defined to use
///     @ref COMMS_BITMASK_BITS() and @ref COMMS_BITMASK_BITS_ACCESS() with the
///     same bit names. For example
///     @code
///     struct MyField : public comms::field::BitmaskValue<...>
///     {
///         COMMS_BITMASK_BITS_SEQ(first, second, third, fourth);
///     }
///     @endcode
///     is equivalent to having
///     @code
///     struct MyField : public comms::field::BitmaskValue<...>
///     {
///         enum BitIdx
///         {
///             BitIdx_first,
///             BitIdx_second,
///             BitIdx_third,
///             BitIdx_fourth,
///             BitIdx_numOfValues
///         }
///
///         bool getBitValue_first() const {...}
///         void setBitValue_first(bool value) {...}
///         bool getBitValue_second() const {...}
///         void setBitValue_second(bool value) {...}
///         bool getBitValue_third() const {...}
///         void setBitValue_third(bool value) {...}
///         bool getBitValue_fourth() const {...}
///         void setBitValue_fourth(bool value) {...}
///     };
///     @endcode
/// @related comms::field::BitmaskValue
/// @warning Some compilers, such as @b clang or early versions of @b g++
///     may have problems compiling code generated by this macro even
///     though it uses valid C++11 constructs in attempt to automatically identify the
///     type of the base class. If the compilation fails,
///     and this macro resides inside a @b NON-template class, please use
///     @ref COMMS_BITMASK_BITS_SEQ_NOTEMPLATE() macro instead. In
///     case this macro needs to reside inside a @b template class, then
///     there is a need to define inner @b Base type, which specifies
///     exact type of the @ref comms::field::BitmaskValue class. For example:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     template <typename... TExtraOptions>
///     class MyField : public
///         comms::field::BitmaskValue<
///             MyFieldBase,
///             comms::field::FixedLength<1>,
///             comms::field::BitmaskReservedBits<0xf0, 0>,
///             TExtraOptions...
///         >
///     {
///         // Duplicate definition of the base class
///         using Base =
///             comms::field::BitmaskValue<
///                 MyFieldBase,
///                 comms::field::FixedLength<1>,
///                 scomms::field::BitmaskReservedBits<0xf0, 0>,
///                 TExtraOptions...
///             >;
///     public:
///         COMMS_BITMASK_BITS_SEQ(first, second, third, fourth);
///     }
///     @endcode
#define COMMS_BITMASK_BITS_SEQ(...) \
    COMMS_BITMASK_BITS(__VA_ARGS__) \
    COMMS_BITMASK_BITS_ACCESS(__VA_ARGS__)

/// @brief Similar to @ref COMMS_BITMASK_BITS_SEQ(), but dedicated for
///     non-template classes.
/// @details The @ref COMMS_BITMASK_BITS_SEQ() macro is a generic one,
///     which can be used in any class (template, or non-template). However,
///     some compilers (such as <b>g++-4.9</b> and below, @b clang-4.0 and below) may fail
///     to compile it even though it uses valid C++11 constructs. If the
///     compilation fails and the class it is being used in is @b NOT a
///     template one, please use @ref COMMS_BITMASK_BITS_SEQ_NOTEMPLATE()
///     instead.
/// @related comms::field::BitmaskValue
#define COMMS_BITMASK_BITS_SEQ_NOTEMPLATE(...) \
    COMMS_BITMASK_BITS(__VA_ARGS__) \
    COMMS_BITMASK_BITS_ACCESS_NOTEMPLATE(__VA_ARGS__)


}  // namespace field

}  // namespace comms


