//
// Copyright 2015 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Contains definition of @ref comms::field::IntValue

#pragma once

#include <ratio>
#include <limits>
#include <type_traits>

#include "comms/CompileControl.h"
#include "comms/ErrorStatus.h"
#include "comms/options.h"
#include "comms/util/type_traits.h"
#include "basic/IntValue.h"
#include "details/AdaptBasicField.h"
#include "tag.h"
#include "comms/details/tag.h"

COMMS_MSVC_WARNING_PUSH
COMMS_MSVC_WARNING_DISABLE(4127) // Disable warning about constant conditional expressions

namespace comms
{

namespace field
{

/// @brief Field that represent integral value.
/// @tparam TFieldBase Base class for this field, expected to be a variant of
///     comms::Field.
/// @tparam T Basic underlying integral type.
/// @tparam TOptions Zero or more options that modify/refine default behaviour
///     of the field. If no option is provided The field's value is serialised as is.
///     @code
///         using MyFieldBase = comms::Field<comms::option::def::BigEndian>;
///         using MyField =comms::field::IntValue<MyFieldBase, std::uint16_t>;
///     @endcode
///     In the example above it will
///     consume 2 bytes (because sizeof(std::uint16_t) == 2) and will
///     be serialised using big endian notation.@n
///     Supported options are:
///     @li @ref comms::option::def::FixedLength
///     @li @ref comms::option::def::FixedBitLength
///     @li @ref comms::option::def::VarLength
///     @li @ref comms::option::def::NumValueSerOffset
///     @li @ref comms::option::def::DefaultValueInitialiser or @ref comms::option::def::DefaultNumValue.
///     @li @ref comms::option::def::ContentsValidator
///     @li @ref comms::option::def::ValidNumValueRange, @ref comms::option::def::ValidNumValue,
///         @ref comms::option::def::ValidBigUnsignedNumValueRange, @ref comms::option::def::ValidBigUnsignedNumValue
///     @li @ref comms::option::def::ValidRangesClear
///     @li @ref comms::option::def::ContentsRefresher
///     @li @ref comms::option::def::HasCustomRead
///     @li @ref comms::option::def::HasCustomRefresh
///     @li @ref comms::option::def::FailOnInvalid
///     @li @ref comms::option::def::IgnoreInvalid
///     @li @ref comms::option::def::ScalingRatio
///     @li @b comms::option::def::Units* - all variants of value units, see
///         @ref sec_field_tutorial_int_value_units for details.
///     @li comms::option::def::EmptySerialization
///     @li @ref comms::option::def::InvalidByDefault
///     @li @ref comms::option::def::VersionStorage
/// @extends comms::Field
/// @headerfile comms/field/IntValue.h
template <typename TFieldBase, typename T, typename... TOptions>
class IntValue : private details::AdaptBasicFieldT<basic::IntValue<TFieldBase, T>, TOptions...>
{
    using BaseImpl = details::AdaptBasicFieldT<basic::IntValue<TFieldBase, T>, TOptions...>;
public:

    /// @brief Endian used for serialisation.
    using Endian = typename BaseImpl::Endian;

    /// @brief Version type
    using VersionType = typename BaseImpl::VersionType;

    /// @brief All the options provided to this class bundled into struct.
    using ParsedOptions = details::OptionsParser<TOptions...>;

    /// @brief Tag indicating type of the field
    using Tag = tag::Int;

    /// @brief Type of underlying integral value.
    /// @details Same as template parameter T to this class.
    using ValueType = typename BaseImpl::ValueType;

    /// @brief Default constructor
    /// @details Initialises internal value to 0.
    IntValue() = default;

    /// @brief Constructor
    explicit IntValue(const ValueType& val)
      : BaseImpl(val)
    {
    }

    /// @brief Copy constructor
    IntValue(const IntValue&) = default;

    /// @brief Destructor
    ~IntValue() noexcept = default;

    /// @brief Copy assignment
    IntValue& operator=(const IntValue&) = default;

    /// @brief Scales value according to ratio specified in provided
    ///     @ref comms::option::def::ScalingRatio option.
    /// @details If @ref comms::option::def::ScalingRatio option wasn't used, then
    ///     comms::option::def::ScalingRatio<1,1> is assumed.
    /// @tparam TRet Return type for the scaled value.
    /// @return "(value() * Scaling_Num) / Scaling_Denom" when all values are
    ///     casted to TRet type.
    template <typename TRet>
    constexpr TRet getScaled() const
    {
        using TagTmp = 
            typename comms::util::LazyShallowConditional<
                ParsedOptions::HasScalingRatio
            >::template Type<
                HasScalingRatioTag,
                NoScalingRatioTag
            >;

        return scaleAsInternal<TRet>(TagTmp());
    }

    /// @brief Same as getScaled()
    template <typename TRet>
    constexpr TRet scaleAs() const
    {
        return getScaled<TRet>();
    }

    /// @brief Opposite operation to getScaled().
    /// @details Allows to assign scaled value, assigns "(val * Scaling_Denom) / Scaling_Num"
    ///     to the value of the field.
    /// @param[in] val Scaled value.
    template <typename TScaled>
    void setScaled(TScaled val)
    {
        using TagTmp = 
            typename comms::util::LazyShallowConditional<
                ParsedOptions::HasScalingRatio
            >::template Type<
                HasScalingRatioTag,
                NoScalingRatioTag
            >;

        return setScaledInternal(val, TagTmp());
    }

    /// @brief Get access to integral value storage.
    const ValueType& value() const
    {
        return BaseImpl::value();
    }

    /// @brief Get access to integral value storage.
    ValueType& value()
    {
        return BaseImpl::value();
    }

    /// @brief Get length required to serialise the current field value.
    /// @return Number of bytes it will take to serialise the field value.
    constexpr std::size_t length() const
    {
        return BaseImpl::length();
    }

    /// @brief Get minimal length that is required to serialise field of this type.
    /// @return Minimal number of bytes required serialise the field value.
    static constexpr std::size_t minLength()
    {
        return BaseImpl::minLength();
    }

    /// @brief Get maximal length that is required to serialise field of this type.
    /// @return Maximal number of bytes required serialise the field value.
    static constexpr std::size_t maxLength()
    {
        return BaseImpl::maxLength();
    }

    /// @brief Check validity of the field value.
    bool valid() const
    {
        return BaseImpl::valid();
    }

    /// @brief Refresh the field's value
    /// @return @b true if the value has been updated, @b false otherwise
    bool refresh()
    {
        return BaseImpl::refresh();
    }

    /// @brief Read field value from input data sequence
    /// @param[in, out] iter Iterator to read the data.
    /// @param[in] size Number of bytes available for reading.
    /// @return Status of read operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus read(TIter& iter, std::size_t size)
    {
        return BaseImpl::read(iter, size);
    }

    /// @brief Compile time check of whether the field has @b proper
    ///     @ref readNoStatus() member function.
    static constexpr bool hasReadNoStatus()
    {
        return BaseImpl::hasReadNoStatus();
    }

    /// @brief Read field value from input data sequence without error check and status report.
    /// @details Similar to @ref read(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to read the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void readNoStatus(TIter& iter)
    {
        BaseImpl::readNoStatus(iter);
    }

    /// @brief Check of whether the field has a consistent value for writing.
    bool canWrite() const
    {
        return BaseImpl::canWrite();
    }

    /// @brief Write current field value to output data sequence
    /// @param[in, out] iter Iterator to write the data.
    /// @param[in] size Maximal number of bytes that can be written.
    /// @return Status of write operation.
    /// @post Iterator is advanced.
    template <typename TIter>
    ErrorStatus write(TIter& iter, std::size_t size) const
    {
        return BaseImpl::write(iter, size);
    }

    /// @brief Compile time check of whether the field has @b proper
    ///     @ref writeNoStatus() member function.
    static constexpr bool hasWriteNoStatus()
    {
        return BaseImpl::hasWriteNoStatus();
    }

    /// @brief Write current field value to output data sequence  without error check and status report.
    /// @details Similar to @ref write(), but doesn't perform any correctness
    ///     checks and doesn't report any failures.
    /// @param[in, out] iter Iterator to write the data.
    /// @post Iterator is advanced.
    template <typename TIter>
    void writeNoStatus(TIter& iter) const
    {
        BaseImpl::writeNoStatus(iter);
    }

    /// @brief Compile time check if this class is version dependent
    static constexpr bool isVersionDependent()
    {
        return ParsedOptions::HasCustomVersionUpdate || BaseImpl::isVersionDependent();
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
        return BaseImpl::getVersion();
    }

    /// @brief Default implementation of version update.
    /// @return @b true in case the field contents have changed, @b false otherwise
    bool setVersion(VersionType version)
    {
        return BaseImpl::setVersion(version);
    }

protected:
    using BaseImpl::readData;
    using BaseImpl::writeData;

private:
    template <typename... TParams>
    using HasScalingRatioTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using NoScalingRatioTag = comms::details::tag::Tag2<>;

    template <typename... TParams>
    using ScaleAsFpTag = comms::details::tag::Tag3<>;

    template <typename... TParams>
    using ScaleAsIntTag = comms::details::tag::Tag4<>;

    template <typename TRet, typename... TParams>
    TRet scaleAsInternal(HasScalingRatioTag<TParams...>) const
    {
        using TagTmp = 
            typename comms::util::LazyShallowConditional<
                std::is_floating_point<TRet>::value
            >::template Type<
                ScaleAsFpTag,
                ScaleAsIntTag
            >;

        return scaleAsInternal<TRet>(TagTmp());
    }

    template <typename TRet, typename... TParams>
    TRet scaleAsInternal(ScaleAsFpTag<TParams...>) const
    {
        static_assert(std::is_floating_point<TRet>::value,
            "TRet is expected to be floating point type");
        return static_cast<TRet>(BaseImpl::value()) * (static_cast<TRet>(ParsedOptions::ScalingRatio::num) / static_cast<TRet>(ParsedOptions::ScalingRatio::den));
    }

    template <typename TRet, typename... TParams>
    TRet scaleAsInternal(ScaleAsIntTag<TParams...>) const
    {
        static_assert(std::is_integral<TRet>::value,
            "TRet is expected to be integral type");

        using CastType = 
            typename comms::util::Conditional<
                std::is_signed<TRet>::value
            >::template Type<
                std::intmax_t,
                std::uintmax_t
            >;

        return
            static_cast<TRet>(
                (static_cast<CastType>(BaseImpl::value()) * ParsedOptions::ScalingRatio::num) / ParsedOptions::ScalingRatio::den);
    }

    template <typename TRet, typename... TParams>
    TRet scaleAsInternal(NoScalingRatioTag<TParams...>) const
    {
        return static_cast<TRet>(BaseImpl::value());
    }

    template <typename TScaled, typename... TParams>
    void setScaledInternal(TScaled val, HasScalingRatioTag<TParams...>)
    {
        using TagTmp = 
            typename comms::util::LazyShallowConditional<
                std::is_floating_point<typename std::decay<decltype(val)>::type>::value
            >::template Type<
                ScaleAsFpTag,
                ScaleAsIntTag
            >;

        setScaledInternal(val, TagTmp());
    }

    template <typename TScaled, typename... TParams>
    void setScaledInternal(TScaled val, ScaleAsFpTag<TParams...>)
    {
        using DecayedType = typename std::decay<decltype(val)>::type;
        auto epsilon = DecayedType(0);
        if (ParsedOptions::ScalingRatio::num < ParsedOptions::ScalingRatio::den) {
            epsilon = static_cast<DecayedType>(ParsedOptions::ScalingRatio::num) / static_cast<DecayedType>(ParsedOptions::ScalingRatio::den + 1);
        }

        if (epsilon < DecayedType(0)) {
            epsilon = -epsilon;
        }

        if (val < DecayedType(0)) {
            epsilon = -epsilon;
        }

        BaseImpl::value() =
            static_cast<ValueType>(
                ((val + epsilon) * static_cast<DecayedType>(ParsedOptions::ScalingRatio::den)) / static_cast<DecayedType>(ParsedOptions::ScalingRatio::num));
    }

    template <typename TScaled, typename... TParams>
    void setScaledInternal(TScaled val, ScaleAsIntTag<TParams...>)
    {
        using CastType = 
            typename comms::util::Conditional<
                std::is_signed<typename std::decay<decltype(val)>::type>::value
            >::template Type<
                std::intmax_t,
                std::uintmax_t
            >;

        BaseImpl::value() =
            static_cast<ValueType>(
                (static_cast<CastType>(val) * ParsedOptions::ScalingRatio::den) / static_cast<CastType>(ParsedOptions::ScalingRatio::num));
    }

    template <typename TScaled, typename... TParams>
    void setScaledInternal(TScaled val, NoScalingRatioTag<TParams...>)
    {
        BaseImpl::value() = static_cast<ValueType>(val);
    }

    static_assert(!ParsedOptions::HasSequenceElemLengthForcing,
        "comms::option::def::SequenceElemLengthForcingEnabled option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceSizeForcing,
        "comms::option::def::SequenceSizeForcingEnabled option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceLengthForcing,
        "comms::option::def::SequenceLengthForcingEnabled option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceFixedSize,
        "comms::option::def::SequenceFixedSize option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceFixedSizeUseFixedSizeStorage,
        "comms::option::app::SequenceFixedSizeUseFixedSizeStorage option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceSizeFieldPrefix,
        "comms::option::def::SequenceSizeFieldPrefix option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceSerLengthFieldPrefix,
        "comms::option::def::SequenceSerLengthFieldPrefix option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceElemSerLengthFieldPrefix,
        "comms::option::def::SequenceElemSerLengthFieldPrefix option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceElemFixedSerLengthFieldPrefix,
        "comms::option::def::SequenceElemSerLengthFixedFieldPrefix option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceTrailingFieldSuffix,
        "comms::option::def::SequenceTrailingFieldSuffix option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasSequenceTerminationFieldSuffix,
        "comms::option::def::SequenceTerminationFieldSuffix option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasFixedSizeStorage,
        "comms::option::app::FixedSizeStorage option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasCustomStorageType,
        "comms::option::app::CustomStorageType option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasOrigDataView,
        "comms::option::app::OrigDataView option is not applicable to IntValue field");
    static_assert(!ParsedOptions::HasVersionsRange,
        "comms::option::def::ExistsBetweenVersions (or similar) option is not applicable to IntValue field");
};


/// @brief Equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are equal, false otherwise.
/// @related IntValue
template <typename TFieldBase, typename T, typename... TOptions>
bool operator==(
    const IntValue<TFieldBase, T, TOptions...>& field1,
    const IntValue<TFieldBase, T, TOptions...>& field2) noexcept
{
    return field1.value() == field2.value();
}

/// @brief Non-equality comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case fields are NOT equal, false otherwise.
/// @related IntValue
template <typename TFieldBase, typename T, typename... TOptions>
bool operator!=(
    const IntValue<TFieldBase, T, TOptions...>& field1,
    const IntValue<TFieldBase, T, TOptions...>& field2) noexcept
{
    return field1.value() != field2.value();
}

/// @brief Equivalence comparison operator.
/// @param[in] field1 First field.
/// @param[in] field2 Second field.
/// @return true in case value of the first field is lower than than the value of the second.
/// @related IntValue
template <typename TFieldBase, typename T, typename... TOptions>
bool operator<(
    const IntValue<TFieldBase, T, TOptions...>& field1,
    const IntValue<TFieldBase, T, TOptions...>& field2) noexcept
{
    return field1.value() < field2.value();
}

/// @brief Compile time check function of whether a provided type is any
///     variant of comms::field::IntValue.
/// @tparam T Any type.
/// @return true in case provided type is any variant of @ref IntValue
/// @related comms::field::IntValue
template <typename T>
constexpr bool isIntValue()
{
    return std::is_same<typename T::Tag, tag::Int>::value;
}

/// @brief Upcast type of the field definition to its parent comms::field::IntValue type
///     in order to have access to its internal types.
/// @related comms::field::IntValue
template <typename TFieldBase, typename T, typename... TOptions>
inline
IntValue<TFieldBase, T, TOptions...>&
toFieldBase(IntValue<TFieldBase, T, TOptions...>& field)
{
    return field;
}

/// @brief Upcast type of the field definition to its parent comms::field::IntValue type
///     in order to have access to its internal types.
/// @related comms::field::IntValue
template <typename TFieldBase, typename T, typename... TOptions>
inline
const IntValue<TFieldBase, T, TOptions...>&
toFieldBase(const IntValue<TFieldBase, T, TOptions...>& field)
{
    return field;
}

}  // namespace field

}  // namespace comms

COMMS_MSVC_WARNING_POP

