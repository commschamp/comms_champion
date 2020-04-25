//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
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

/// @file
/// Contains definition of all the options used by the @b COMMS library

#pragma once

#include <tuple>
#include <type_traits>
#include <limits>
#include <ratio>
#include <cstdint>
#include <cstddef>

#include "comms/traits.h"
#include "comms/ErrorStatus.h"
#include "comms/field/OptionalMode.h"

namespace comms
{

namespace option
{

namespace details
{

template <typename T>
struct IsRatio
{
    static const bool Value = false;
};

template <std::intmax_t TNum, std::intmax_t TDen>
struct IsRatio<std::ratio<TNum, TDen> >
{
    static const bool Value = true;
};

template <typename T>
constexpr bool isRatio()
{
    return IsRatio<T>::Value;
}

template<typename T, T TVal>
struct DefaultNumValueInitialiser
{
    template <typename TField>
    void operator()(TField&& field)
    {
        using FieldType = typename std::decay<TField>::type;
        using ValueType = typename FieldType::ValueType;
        field.value() = static_cast<ValueType>(TVal);
    }
};

template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
struct NumValueRangeValidator
{
    static_assert(
        TMinValue <= TMaxValue,
        "Min value must be not greater than Max value");

    template <typename TField>
    constexpr bool operator()(const TField& field) const
    {
        using MinTag = typename std::conditional<
            (std::numeric_limits<decltype(MinValue)>::min() < MinValue),
            CompareTag,
            ReturnTrueTag
        >::type;

        using MaxTag = typename std::conditional<
            (MaxValue < std::numeric_limits<decltype(MaxValue)>::max()),
            CompareTag,
            ReturnTrueTag
        >::type;

        return aboveMin(field.value(), MinTag()) && belowMax(field.value(), MaxTag());
    }

private:
    struct ReturnTrueTag {};
    struct CompareTag {};

    template <typename TValue>
    static constexpr bool aboveMin(const TValue& value, CompareTag)
    {
        using ValueType = typename std::decay<decltype(value)>::type;
        return (static_cast<ValueType>(MinValue) <= static_cast<ValueType>(value));
    }

    template <typename TValue>
    static constexpr bool aboveMin(const TValue&, ReturnTrueTag)
    {
        return true;
    }

    template <typename TValue>
    static constexpr bool belowMax(const TValue& value, CompareTag)
    {
        using ValueType = typename std::decay<decltype(value)>::type;
        return (value <= static_cast<ValueType>(MaxValue));
    }

    template <typename TValue>
    static constexpr bool belowMax(const TValue&, ReturnTrueTag)
    {
        return true;
    }


    static const auto MinValue = TMinValue;
    static const auto MaxValue = TMaxValue;
};

template<std::uintmax_t TMask, std::uintmax_t TValue>
struct BitmaskReservedBitsValidator
{
    template <typename TField>
    constexpr bool operator()(TField&& field) const
    {
        using FieldType = typename std::decay<TField>::type;
        using ValueType = typename FieldType::ValueType;

        return (field.value() & static_cast<ValueType>(TMask)) == static_cast<ValueType>(TValue);
    }
};

template <comms::field::OptionalMode TVal>
struct DefaultOptModeInitialiser
{
    template <typename TField>
    void operator()(TField& field) const
    {
        field.setMode(TVal);
    }
};

template<std::size_t TIdx>
struct DefaultVariantIndexInitialiser
{
    template <typename TField>
    void operator()(TField& field)
    {
        field.template initField<TIdx>();
    }
};

}  // namespace details


namespace def {

/// @brief Options to specify endian.
/// @tparam TEndian Endian type. Must be either comms::traits::endian::Big or
///     comms::traits::endian::Little.
/// @headerfile comms/options.h
template <typename TEndian>
struct Endian
{
};

/// @brief Alias option to Endian specifying Big endian.
/// @headerfile comms/options.h
using BigEndian = Endian<comms::traits::endian::Big>;

/// @brief Alias option to Endian specifying Little endian.
/// @headerfile comms/options.h
using LittleEndian = Endian<comms::traits::endian::Little>;

/// @brief Option used to specify type of the ID.
/// @tparam T Type of the message ID.
/// @headerfile comms/options.h
template <typename T>
struct MsgIdType {};

/// @brief Option used to specify numeric ID of the message.
/// @tparam TId Numeric ID value.
/// @headerfile comms/options.h
template <std::intmax_t TId>
struct StaticNumIdImpl {};

/// @brief Option used to specify that message doesn't have valid ID.
/// @headerfile comms/options.h
struct NoIdImpl {};

/// @brief Option used to specify actual type of the message.
/// @headerfile comms/options.h
template <typename TMsg>
struct MsgType {};

/// @brief Option used to specify some extra fields from transport framing.
/// @details Some fields from transport framing may influence the way on how
///     message fields get read or written. It may also have an influence on
///     how message is handled. This option is intended to provide a list
///     of such fields, bundled in @b std::tuple, to @ref comms::Message interface
///     class.
/// @tparam TFields The fields of the message bundled in std::tuple.
/// @headerfile comms/options.h
template <typename TFields>
struct ExtraTransportFields {};

/// @brief Option used to specify index of the version field inside
///     extra transport fields tuple provided with @ref
///     comms::option::ExtraTransportFields option.
/// @tparam TIdx Index of the field inside the tuple.
/// @headerfile comms/options.h
template <std::size_t TIdx>
struct VersionInExtraTransportFields {};

/// @brief Option used to specify fields of the message and force implementation
///     of default read, write, validity check, and length retrieval information
///     of the message.
/// @tparam TFields The fields of the message bundled in std::tuple.
/// @headerfile comms/options.h
template <typename TFields>
struct FieldsImpl;

/// @cond SKIP_DOC
template <typename... TFields>
struct FieldsImpl<std::tuple<TFields...> >
{
};
/// @endcond

/// @brief Alias to FieldsImpl<std::tuple<> >
/// @headerfile comms/options.h
using ZeroFieldsImpl = FieldsImpl<std::tuple<> >;

/// @brief Option that notifies @ref comms::MessageBase about existence of
///     @b doGetId() member function in derived class.
/// @headerfile comms/options.h
struct HasDoGetId {};

/// @brief Option that notifies comms::MessageBase about existence of
///     access to fields.
/// @details Can be useful when there is a chain of inheritances from
///     comms::MessageBase.
/// @headerfile comms/options.h
struct AssumeFieldsExistence {};

/// @brief Option used to specify number of bytes that is used for field serialisation.
/// @details Applicable only to numeric fields, such as comms::field::IntValue or
///     comms::field::EnumValue.
///
///     For example, protocol specifies that some field is serialised using
///     only 3 bytes. There is no basic integral type that takes 3 bytes
///     of space exactly. The closest alternative is std::int32_t or
///     std::uint32_t. Such field may be defined as:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::IntValue<
///             MyFieldBase,
///             std::uint32_t,
///             comms::option::FixedLength<3>
///         >;
///     @endcode
/// @tparam TLen Length of the serialised value.
/// @tparam TSignExtend Perform sign extension, relevant only to signed types.
/// @headerfile comms/options.h
template<std::size_t TLen, bool TSignExtend = true>
struct FixedLength {};

/// @brief Option used to specify number of bits that is used for field serialisation
///     when a field is a member of comms::field::Bitfield.
/// @details For example, the protocol specifies that two independent integer
///     values of 6 and 10 bits respectively packed into two bytes to save space.
///     Such combined field may be defined as:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::Bitfield<
///             MyFieldBase,
///             std::tuple<
///                 comms::field::IntValue<
///                     MyFieldBase,
///                     std::uint8_t,
///                     comms::option::FixedBitLength<6>
///                 >,
///                 comms::field::IntValue<
///                     MyFieldBase,
///                     std::uint16_t,
///                     comms::option::FixedBitLength<10>
///                 >
///             >
///         >;
///     @endcode
/// @tparam TLen Length of the serialised value in bits.
/// @headerfile comms/options.h
template<std::size_t TLen>
struct FixedBitLength {};

/// @brief Option used to specify that field may have variable serialisation length
/// @details Applicable only to numeric fields, such as comms::field::IntValue
///     or comms::field::EnumValue.
///     Use this option to specify that serialised value has
///     <a href="https://en.wikipedia.org/wiki/Variable-length_quantity">Base-128</a>
///     encoding, i.e. the most significant bit in the byte indicates whether
///     the encoding of the value is complete or the next byte in
///     sequence still encodes the current integer value. For example field
///     which value can be serialised using between 1 and 4 bytes can be
///     defined as:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::IntValue<
///             MyFieldBase,
///             std::uint32_t,
///             comms::option::VarLength<1, 4>
///         >;
///         @endcode
/// @tparam TMin Minimal length the field may consume.
/// @tparam TMax Maximal length the field may consume.
/// @pre TMin <= TMax
/// @headerfile comms/options.h
template<std::size_t TMin, std::size_t TMax>
struct VarLength
{
    static_assert(TMin <= TMax, "TMin must not be greater that TMax.");
};

/// @brief Option to specify numeric value serialisation offset.
/// @details Applicable only to numeric fields such as comms::field::IntValue or
///     comms::field::EnumValue.
///     The provided value will be added to the field's value and the
///     result will be written to the buffer when serialising. Good example
///     for such option would be serialising a "current year" value. Most protocols
///     now specify it as an offset from year 2000 or later and written as a
///     single byte, i.e. to specify year 2015 is to write value 15.
///     However it may be inconvenient to manually adjust serialised/deserialised
///     value by predefined offset 2000. To help with such case option
///     comms::option::NumValueSerOffset can be used. For example:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::IntValue<
///             MyFieldBase,
///             std::uint16_t,
///             comms::option::FixedLength<1>,
///             comms::option::NumValueSerOffset<-2000>
///         >;
///     @endcode
///     Note that in the example above the field value (accessible by @b value() member
///     function of the field) will have type std::uint16_t and will be equal to
///     say 2015, while when serialised it consumes only 1 byte (thanks to
///     comms::option::FixedLength option) and reduced value of 15 is written.
/// @tparam TOffset Offset value to be added when serialising field.
/// @headerfile comms/options.h
template<std::intmax_t TOffset>
struct NumValueSerOffset {};

/// @brief Option to specify scaling ratio.
/// @details Applicable only to comms::field::IntValue.
///     Sometimes the protocol specifies values being transmitted in
///     one units while when handling the message they are better to be handled
///     in another. For example, some distance information is transmitted as
///     integer value of millimetres, but while processing it should be handled as floating
///     point value of meters. Such field is defined as:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::IntValue<
///             MyFieldBase,
///             std::int32_t,
///             comms::option::ScalingRatio<1, 100>
///         >;
///     @endcode
///     Then, to accessed the scaled value of the field use @b scaleAs() or
///     @b setScaled() methods of comms::field::IntValue field:
///     @code
///     void processField(const MyField& field)
///     {
///         auto distInMillimetres = field.value();
///         auto distInMeters = field.scaleAs<double>();
///     }
///     @endcode
/// @tparam TNum Numerator of the scaling ratio.
/// @tparam TDenom Denominator of the scaling ratio.
/// @headerfile comms/options.h
template <std::intmax_t TNum, std::intmax_t TDenom>
struct ScalingRatio
{
    static_assert(TNum != 0, "Wrong scaling ratio");
    static_assert(TDenom != 0, "Wrong scaling ratio");
};

/// @brief Option that modifies the default behaviour of collection fields to
///     prepend the serialised data with number of @b elements information.
/// @details Quite often when collection of fields is serialised it must be
///     prepended with one or more bytes indicating number of elements that will
///     follow.
///     Applicable to fields that represent collection of raw data or other
///     fields, such as comms::field::ArrayList or comms::field::String.@n
///     For example sequence of raw bytes must be prefixed with 2 bytes stating
///     the size of the sequence:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::ArrayList<
///             MyFieldBase,
///             std::uint8_t,
///             comms::option::SequenceSizeFieldPrefix<
///                 comms::field::IntValue<MyFieldBase, std::uint16_t>
///             >
///         >;
///     @endcode
/// @tparam TField Type of the field that represents size
/// @headerfile comms/options.h
template <typename TField>
struct SequenceSizeFieldPrefix {};

/// @brief Option that modifies the default behaviour of collection fields to
///     prepend the serialised data with number of @b bytes information.
/// @details Similar to @ref SequenceSizeFieldPrefix, but instead of
///     number of @b elements to follow, the prefix field contains number of
///     @b bytes that will follow.
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::ArrayList<
///             MyFieldBase,
///             comms::field::Bundle<
///                 std::tuple<
///                     comms::field::IntValue<MyFieldBase, std::uint32_t>,
///                     comms::field::String<MyFieldBase>
///                 >
///             >,
///             comms::option::SequenceSerLengthFieldPrefix<
///                 comms::field::IntValue<MyFieldBase, std::uint16_t>
///             >
///         >;
///     @endcode
/// @tparam TField Type of the field that represents serialisation length
/// @tparam TReadErrorStatus Error status to return in case read operation fails when should not
/// @headerfile comms/options.h
template <typename TField, comms::ErrorStatus TReadErrorStatus = comms::ErrorStatus::InvalidMsgData>
struct SequenceSerLengthFieldPrefix {};

/// @brief Option that forces <b>every element</b> of @ref comms::field::ArrayList to
///     be prefixed with its serialisation length.
/// @details Similar to @ref SequenceSerLengthFieldPrefix but instead of the whole
///     list, every element is prepended with its serialisation length.
/// @tparam TField Type of the field that represents serialisation length
/// @tparam TReadErrorStatus Error status to return in case read operation fails when should not
/// @headerfile comms/options.h
template <typename TField, comms::ErrorStatus TReadErrorStatus = comms::ErrorStatus::InvalidMsgData>
struct SequenceElemSerLengthFieldPrefix {};

/// @brief Option that forces @b first element only of @ref comms::field::ArrayList to
///     be prefixed with its serialisation length.
/// @details Similar to @ref SequenceElemSerLengthFieldPrefix, but
///     applicable only to the lists where elements are of the same
///     fixed size, where there is no need to prefix @b every element
///     with its size.
/// @tparam TField Type of the field that represents serialisation length
/// @tparam TReadErrorStatus Error status to return in case read operation fails when should not
/// @headerfile comms/options.h
template <typename TField, comms::ErrorStatus TReadErrorStatus = comms::ErrorStatus::InvalidMsgData>
struct SequenceElemFixedSerLengthFieldPrefix {};

/// @brief Option that forces termination of the sequence when predefined value
///     is encountered.
/// @details Sometimes protocols use zero-termination for strings instead of
///     prefixing them with their size. Below is an example of how to achieve
///     such termination using SequenceTerminationFieldSuffix option.
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::String<
///             MyFieldBase,
///             comms::option::SequenceTerminationFieldSuffix<
///                 comms::field::IntValue<MyFieldBase, char, comms::option::DefaultNumValue<0> >
///             >
///         >;
///     @endcode
/// @tparam TField Type of the field that represents suffix
/// @headerfile comms/options.h
template <typename TField>
struct SequenceTerminationFieldSuffix {};

/// @brief Option that forces collection fields to append provides suffix every
///     time it is serialised.
/// @details It is a bit looser version than SequenceTerminationFieldSuffix.
///     Encountering the expected termination value doesn't terminate the
///     read operation on the sequence. The size of the sequence should
///     be defined by other means. For example, zero termination string that
///     occupies exactly 6 bytes when serialised (padded with zeroes at the end)
///     will be defined like this:
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::String<
///             MyFieldBase,
///             comms::option::SequenceFixedSize<5>,
///             comms::option::SequenceTrailingFieldSuffix<
///                 comms::field::IntValue<MyFieldBase, char, comms::option::DefaultNumValue<0> >
///             >
///         >;
///     @endcode
/// @tparam TField Type of the field that represents suffix
/// @headerfile comms/options.h
template <typename TField>
struct SequenceTrailingFieldSuffix {};

/// @brief Option to enable external forcing of the collection's elements count.
/// @details Sometimes the size information is detached from the data sequence
///     itself, i.e. there may be one or more independent fields between the
///     size field and the first byte of the collection. In such case it becomes
///     impossible to use @ref SequenceSizeFieldPrefix option. Instead, the size
///     information must be provided by external calls. Usage of this option
///     enables @b forceReadElemCount() and @b clearReadElemCount() functions in
///     the collection fields, such as comms::field::ArrayList or comms::field::String
///     which can be used to specify the size information after it was read
///     independently.
/// @headerfile comms/options.h
struct SequenceSizeForcingEnabled {};

/// @brief Option to enable external forcing of the collection's serialisation length
///     duting "read" operation.
/// @details Sometimes the length information is detached from the data sequence
///     itself, i.e. there may be one or more independent fields between the
///     length field and the first byte of the collection. In such case it becomes
///     impossible to use @ref SequenceSerLengthFieldPrefix option. Instead, the length
///     information must be provided by external calls. Usage of this option
///     enables @b forceReadLength() and @b clearReadLengthForcing() functions in
///     the collection fields, such as comms::field::ArrayList or comms::field::String
///     which can be used to specify the size information after it was read
///     independently.
/// @headerfile comms/options.h
struct SequenceLengthForcingEnabled {};

/// @brief Option to enable external forcing of the collection element
///     serialisation length.
/// @details Some protocols may prefix the variable length lists with serialisation
///     length of a <b>single element</b> in addition to the number of elements
///     in the list. Usage of this option
///     enables @b forceReadElemLength() and @b clearReadElemLengthForcing() functions in
///     the comms::field::ArrayList
///     which can be used to specify the element serialisation length after it was read
///     independently. @n
/// @headerfile comms/options.h
struct SequenceElemLengthForcingEnabled {};

/// @brief Option used to define exact number of elements in the collection field.
/// @details Protocol specification may define that there is exact number of
///     elements in the sequence. Use SequenceFixedSize option to convey
///     this information to the field definition, which will force @b read() and
///     @b write() member functions of the collection field to behave as expected.
/// @headerfile comms/options.h
template <std::size_t TSize>
struct SequenceFixedSize {};

/// @brief Option that specifies default initialisation class.
/// @details Use this option when default constructor of the field must assign
///     some special value. The initialiser class provided as template argument
///     must define the following member function:
///     @code
///     struct MyInitialiser
///     {
///         template <typename TField>
///         void operator()(TField& field) {...}
///     };
///     @endcode
///     For example, we want string field that will have "hello" as its default
///     value. The provided initialiser class with the option will be instantiated
///     and its operator() is invoked which is responsible to assign proper
///     value to the field.
///     @code
///     struct MyStringInitialiser
///     {
///         template <typename TField>
///         void operator()(TField& field) const
///         {
///             field.value() = hello;
///         }
///     };
///
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::String<
///             MyFieldBase,
///             comms::option::DefaultValueInitialiser<MyStringInitialiser>
///         >;
///     @endcode
/// @tparam T Type of the initialiser class.
/// @headerfile comms/options.h
template <typename T>
struct DefaultValueInitialiser {};

/// @brief Option that specifies custom validation class.
/// @details By default, value of every field is considered to be valid
///     (@b valid() member function of the field returns @b true). If there is a need
///     to validate the value of the function, use this option to define
///     custom validation logic for the field. The validation class provided as
///     a template argument to this option must define the following member function:
///     @code
///     struct MyValidator
///     {
///         template <typename TField>
///         bool operator()(const TField& field) {...}
///     };
///     @endcode
///     For example, value of the string field considered to be valid if it's
///     not empty and starts with '$' character.
///     The provided validator class with the option will be instantiated
///     and its operator() will be invoked.
///     @code
///     struct MyStringValidator
///     {
///         template <typename TField>
///         bool operator()(TField& field) const
///         {
///             auto& str = field.value();
///             return (!str.empty()) && (str[0] == '$');
///         }
///     };
///
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::String<
///             MyFieldBase,
///             comms::option::ContentsValidator<MyStringValidator>
///         >;
///     @endcode
///     Note that in the example above the default constructed MyField will
///     have invalid value. To fix that you must also use
///     comms::option::DefaultValueInitialiser option to specify proper default
///     value.
/// @tparam T Type of the validator class.
/// @headerfile comms/options.h
template <typename T>
struct ContentsValidator {};

/// @brief Option that specifies custom refreshing class.
/// @details The "refreshing" functionality is there to allow bringing field's
///     contents into a consistent state if it's not. The default "refreshing"
///     functionality does nothing and returns @b false (meaning nothing has
///     been changed). If there is a need to provide custom refreshing functionality
///     use this option and provide custom refresher class. It must
///     define the following member function:
///     @code
///     struct MyRefresher
///     {
///         template <typename TField>
///         bool operator()(TField& field) {
///             ... // return true if field's contents changed
///         }
///     };
///     @endcode
/// @tparam T Type of the refresher class.
/// @headerfile comms/options.h
template <typename T>
struct ContentsRefresher {};

/// @brief Option that specifies custom value reader class.
/// @details It may be useful to override default reading functionality for complex
///     fields, such as comms::field::Bundle, where the way members are read is
///     defined by the values of other members. For example, bundle of two integer
///     fields, the first one is normal, and the second one is optional.
///     The optional mode of the latter is determined by
///     the value of the first field. If its value is 0, than the second
///     member exists, otherwise it's missing.
///     @code
///     typedef comms::field::Bundle<
///         comms::Field<BigEndianOpt>,
///         std::tuple<
///             comms::field::IntValue<
///                 comms::Field<BigEndianOpt>,
///                 std::uint8_t
///             >,
///             comms::field::Optional<
///                 comms::field::IntValue<
///                     comms::Field<BigEndianOpt>,
///                     std::uint16_t
///                 >
///             >
///         >,
///         comms::option::CustomValueReader<MyCustomReader>
///     > Field;
///     @endcode
///     The @b MyCustomReader custom reading class may implement required
///     functionality of reading the first member, analysing its value, setting
///     appropriate mode for the second one and read the second member.
///
///     The custom value reader class provided as template argument
///     must define the following member function:
///     @code
///     struct MyCustomReader
///     {
///         template <typename TField, typename TIter>
///         comms::ErrorStatus operator()(TField& field, TIter& iter, std::size_t len) {...}
///     };
///     @endcode
///
///     The custom reader for the example above may be implemented as:
///     @code
///     struct MyCustomReader
///     {
///         template <typename TField, typename TIter>
///         comms::ErrorStatus operator()(TField& field, TIter& iter, std::size_t len) const
///         {
///             auto& members = field.value();
///             auto& first = std::get<0>(members);
///             auto& second = std::get<1>(members);
///
///             auto es = first.read(iter, len);
///             if (es != comms::ErrorStatus::Success) {
///                 return es;
///             }
///
///             if (first.value() != 0) {
///                 second.setMode(comms::field::OptionalMode::Missing);
///             }
///             else {
///                 second.setMode(comms::field::OptionalMode::Exists);
///             }
///
///             return second.read(iter, len - first.length());
///         }
///     };
///     @endcode
/// @tparam T Type of the custom reader class.
/// @headerfile comms/options.h
template <typename T>
struct CustomValueReader {};

/// @brief Option that forces field's read operation to fail if invalid value
///     is received.
/// @details Sometimes protocol is very strict about what field's values are
///     allowed and forces to abandon a message if invalid value is received.
///     If comms::option::FailOnInvalid is provided as an option to a field,
///     the validity is going to checked automatically after the read. If invalid
///     value is identified, error will be returned from the @b read() operation.
/// @tparam TStatus Error status to return when the content of the read field is invalid.
/// @headerfile comms/options.h
template <comms::ErrorStatus TStatus = comms::ErrorStatus::InvalidMsgData>
struct FailOnInvalid {};

/// @brief Option that forces field's read operation to ignore read data if invalid value
///     is received.
/// @details If this option is provided to the field, the read operation will
///     check the validity of the read value. If it is identified as invalid,
///     the read value is not assigned to the field, i.e. the field's value
///     remains unchanged, although no error is reported.
/// @headerfile comms/options.h
struct IgnoreInvalid {};

/// @brief Options to specify units of the field.
/// @tparam TType Type of the unints, can be any type from comms::traits::units
///     namespace.
/// @tparam TRatio Ratio within the units type, must be a variant of
///     @b std::ratio type.
/// @headerfile comms/options.h
template <typename TType, typename TRatio>
struct Units
{
    static_assert(details::isRatio<TRatio>(),
        "TRatio parameter must be a variant of std::ratio");

    static_assert(TRatio::num != 0, "Wrong ratio value");
    static_assert(TRatio::den != 0, "Wrong ratio value");
};

/// @brief Alias option, specifying field value units are "nanoseconds".
/// @headerfile comms/options.h
using UnitsNanoseconds =
    Units<comms::traits::units::Time, comms::traits::units::NanosecondsRatio>;

/// @brief Alias option, specifying field value units are "microseconds".
/// @headerfile comms/options.h
using UnitsMicroseconds =
    Units<comms::traits::units::Time, comms::traits::units::MicrosecondsRatio>;

/// @brief Alias option, specifying field value units are "milliseconds".
/// @headerfile comms/options.h
using UnitsMilliseconds =
    Units<comms::traits::units::Time, comms::traits::units::MillisecondsRatio>;

/// @brief Alias option, specifying field value units are "seconds".
/// @headerfile comms/options.h
using UnitsSeconds =
    Units<comms::traits::units::Time, comms::traits::units::SecondsRatio>;

/// @brief Alias option, specifying field value units are "minutes".
/// @headerfile comms/options.h
using UnitsMinutes =
    Units<comms::traits::units::Time, comms::traits::units::MinutesRatio>;

/// @brief Alias option, specifying field value units are "hours".
/// @headerfile comms/options.h
using UnitsHours =
    Units<comms::traits::units::Time, comms::traits::units::HoursRatio>;

/// @brief Alias option, specifying field value units are "days".
/// @headerfile comms/options.h
using UnitsDays =
    Units<comms::traits::units::Time, comms::traits::units::DaysRatio>;

/// @brief Alias option, specifying field value units are "weeks".
/// @headerfile comms/options.h
using UnitsWeeks =
    Units<comms::traits::units::Time, comms::traits::units::WeeksRatio>;

/// @brief Alias option, specifying field value units are "nanometers".
/// @headerfile comms/options.h
using UnitsNanometers =
    Units<comms::traits::units::Distance, comms::traits::units::NanometersRatio>;

/// @brief Alias option, specifying field value units are "micrometers".
/// @headerfile comms/options.h
using UnitsMicrometers =
    Units<comms::traits::units::Distance, comms::traits::units::MicrometersRatio>;

/// @brief Alias option, specifying field value units are "millimeters".
/// @headerfile comms/options.h
using UnitsMillimeters =
    Units<comms::traits::units::Distance, comms::traits::units::MillimetersRatio>;

/// @brief Alias option, specifying field value units are "centimeters".
/// @headerfile comms/options.h
using UnitsCentimeters =
    Units<comms::traits::units::Distance, comms::traits::units::CentimetersRatio>;

/// @brief Alias option, specifying field value units are "meters".
/// @headerfile comms/options.h
using UnitsMeters =
    Units<comms::traits::units::Distance, comms::traits::units::MetersRatio>;

/// @brief Alias option, specifying field value units are "kilometers".
/// @headerfile comms/options.h
using UnitsKilometers =
    Units<comms::traits::units::Distance, comms::traits::units::KilometersRatio>;

/// @brief Alias option, specifying field value units are "nanometers per second".
/// @headerfile comms/options.h
using UnitsNanometersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::NanometersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "micrometers per second".
/// @headerfile comms/options.h
using UnitsMicrometersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::MicrometersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "millimeters per second".
/// @headerfile comms/options.h
using UnitsMillimetersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::MillimetersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "centimeters per second".
/// @headerfile comms/options.h
using UnitsCentimetersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::CentimetersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "meters per second".
/// @headerfile comms/options.h
using UnitsMetersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::MetersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "kilometers per second".
/// @headerfile comms/options.h
using UnitsKilometersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::KilometersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "kilometers per hour".
/// @headerfile comms/options.h
using UnitsKilometersPerHour =
    Units<comms::traits::units::Speed, comms::traits::units::KilometersPerHourRatio>;

/// @brief Alias option, specifying field value units are "hertz".
/// @headerfile comms/options.h
using UnitsHertz =
    Units<comms::traits::units::Frequency, comms::traits::units::HzRatio>;

/// @brief Alias option, specifying field value units are "kilohertz".
/// @headerfile comms/options.h
using UnitsKilohertz =
    Units<comms::traits::units::Frequency, comms::traits::units::KiloHzRatio>;

/// @brief Alias option, specifying field value units are "megahertz".
/// @headerfile comms/options.h
using UnitsMegahertz =
    Units<comms::traits::units::Frequency, comms::traits::units::MegaHzRatio>;

/// @brief Alias option, specifying field value units are "gigahertz".
/// @headerfile comms/options.h
using UnitsGigahertz =
    Units<comms::traits::units::Frequency, comms::traits::units::GigaHzRatio>;

/// @brief Alias option, specifying field value units are "degrees".
/// @headerfile comms/options.h
using UnitsDegrees =
    Units<comms::traits::units::Angle, comms::traits::units::DegreesRatio>;

/// @brief Alias option, specifying field value units are "radians".
/// @headerfile comms/options.h
using UnitsRadians =
    Units<comms::traits::units::Angle, comms::traits::units::RadiansRatio>;

/// @brief Alias option, specifying field value units are "nanoamps".
/// @headerfile comms/options.h
using UnitsNanoamps =
    Units<comms::traits::units::Current, comms::traits::units::NanoampsRatio>;

/// @brief Alias option, specifying field value units are "microamps".
/// @headerfile comms/options.h
using UnitsMicroamps =
    Units<comms::traits::units::Current, comms::traits::units::MicroampsRatio>;

/// @brief Alias option, specifying field value units are "milliamps".
/// @headerfile comms/options.h
using UnitsMilliamps =
    Units<comms::traits::units::Current, comms::traits::units::MilliampsRatio>;

/// @brief Alias option, specifying field value units are "amps".
/// @headerfile comms/options.h
using UnitsAmps =
    Units<comms::traits::units::Current, comms::traits::units::AmpsRatio>;

/// @brief Alias option, specifying field value units are "kiloamps".
/// @headerfile comms/options.h
using UnitsKiloamps =
    Units<comms::traits::units::Current, comms::traits::units::KiloampsRatio>;

/// @brief Alias option, specifying field value units are "nanovolts".
/// @headerfile comms/options.h
using UnitsNanovolts =
    Units<comms::traits::units::Voltage, comms::traits::units::NanovoltsRatio>;

/// @brief Alias option, specifying field value units are "microvolts".
/// @headerfile comms/options.h
using UnitsMicrovolts =
    Units<comms::traits::units::Voltage, comms::traits::units::MicrovoltsRatio>;

/// @brief Alias option, specifying field value units are "millivolts".
/// @headerfile comms/options.h
using UnitsMillivolts =
    Units<comms::traits::units::Voltage, comms::traits::units::MillivoltsRatio>;

/// @brief Alias option, specifying field value units are "volts".
/// @headerfile comms/options.h
using UnitsVolts =
    Units<comms::traits::units::Voltage, comms::traits::units::VoltsRatio>;

/// @brief Alias option, specifying field value units are "kilovolts".
/// @headerfile comms/options.h
using UnitsKilovolts =
    Units<comms::traits::units::Voltage, comms::traits::units::KilovoltsRatio>;

/// @brief Alias option, specifying field value units are "bytes".
/// @headerfile comms/options.h
using UnitsBytes =
    Units<comms::traits::units::Memory, comms::traits::units::BytesRatio>;    

/// @brief Alias option, specifying field value units are "kilobytes".
/// @headerfile comms/options.h
using UnitsKilobytes =
    Units<comms::traits::units::Memory, comms::traits::units::KilobytesRatio>;        

/// @brief Alias option, specifying field value units are "megabytes".
/// @headerfile comms/options.h
using UnitsMegabytes =
    Units<comms::traits::units::Memory, comms::traits::units::MegabytesRatio>;     

/// @brief Alias option, specifying field value units are "gigabytes".
/// @headerfile comms/options.h
using UnitsGigabytes =
    Units<comms::traits::units::Memory, comms::traits::units::GigabytesRatio>;         

/// @brief Alias option, specifying field value units are "terabytes".
/// @headerfile comms/options.h
using UnitsTerabytes =
    Units<comms::traits::units::Memory, comms::traits::units::TerabytesRatio>;         

/// @brief Alias to DefaultValueInitialiser, it defines initialiser class that
///     assigns numeric value provided as the template argument to this option.
/// @details If the required numeric value is too big (doesn't fit into @b
///     std::intmax_t type), please use @ref DefaultBigUnsignedNumValue option
///     class instead.
/// @tparam TVal Numeric value is to be assigned to the field in default constructor.
/// @see @ref DefaultBigUnsignedNumValue
/// @headerfile comms/options.h
template<std::intmax_t TVal>
using DefaultNumValue =
    DefaultValueInitialiser<
        details::DefaultNumValueInitialiser<std::intmax_t, TVal>
    >;

/// @brief Alias to DefaultValueInitialiser, it defines initialiser class that
///     assigns big unsigned numeric value provided as the template argument to this option.
/// @details If the required numeric value is small enough to fit into @b
///     std::intmax_t type, it is recommended to use @ref DefaultNumValue option
///     class instead.
/// @tparam TVal Numeric value is to be assigned to the field in default constructor.
/// @see @ref DefaultBigUnsignedNumValue
/// @headerfile comms/options.h
template<std::uintmax_t TVal>
using DefaultBigUnsignedNumValue =
    DefaultValueInitialiser<
        details::DefaultNumValueInitialiser<std::uintmax_t, TVal>
    >;

/// @brief Provide range of valid numeric values.
/// @details Quite often numeric fields such as comms::field::IntValue or
///     comms::option::EnumValue have limited number of valid values ranges.
///     This option can be used multiple times to provide several valid ranges.@n
///     If values are too big to fit into @b std::intmax_t type, please use
///     @ref ValidBigUnsignedNumValueRange option instead.
/// @tparam TMinValue Minimal valid numeric value
/// @tparam TMaxValue Maximal valid numeric value
/// @note The intersection of the provided multiple ranges is @b NOT checked.
/// @warning Some older compilers (@b gcc-4.7) fail to compile valid C++11 code
///     that allows usage of multiple @ref ValidNumValueRange options. If this is
///     the case, please don't pass more than one @ref ValidNumValueRange option.
/// @see @ref ValidNumValue
/// @see @ref ValidBigUnsignedNumValueRange
/// @headerfile comms/options.h
template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
struct ValidNumValueRange
{
    static_assert(TMinValue <= TMaxValue, "Invalid range");
};

/// @brief Clear accumulated ranges of valid values.
struct ValidRangesClear {};

/// @brief Similar to @ref ValidNumValueRange, but overrides (nullifies)
///     all previously set valid values ranges.
/// @see @ref ValidNumValueOverride
/// @see @ref ValidBigUnsignedNumValueRangeOverride
/// @deprecated Use @ref ValidRangesClear instead.
template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
using ValidNumValueRangeOverride =
    std::tuple<
        ValidNumValueRange<TMinValue, TMaxValue>,
        ValidRangesClear
    >;

/// @brief Alias to @ref ValidNumValueRange.
/// @details Equivalent to @b ValidNumValueRange<TValue, TValue>
template<std::intmax_t TValue>
using ValidNumValue = ValidNumValueRange<TValue, TValue>;

/// @brief Alias to @ref ValidNumValueRangeOverride.
/// @details Equivalent to @b ValidNumValueRangeOverride<TValue, TValue>
/// @deprecated Use @ref ValidRangesClear instead.
template<std::intmax_t TValue>
using ValidNumValueOverride = ValidNumValueRangeOverride<TValue, TValue>;

/// @brief Provide range of valid unsigned numeric values.
/// @details Similar to @ref ValidNumValueRange, but dedicated to
///     big unsigned numbers, which don't fit into @b std::intmax_t type.
/// @tparam TMinValue Minimal valid numeric value
/// @tparam TMaxValue Maximal valid numeric value
/// @note The intersection of the provided multiple ranges is @b NOT checked.
/// @warning Some older compilers (@b gcc-4.7) fail to compile valid C++11 code
///     that allows usage of multiple @ref ValidNumValueRange options. If this is
///     the case, please don't pass more than one
///     @ref ValidNumValueRange or @ref ValidBigUnsignedNumValueRange option.
/// @see @ref ValidNumValueRange
/// @see @ref ValidBigUnsignedNumValue
/// @headerfile comms/options.h
template<std::uintmax_t TMinValue, std::uintmax_t TMaxValue>
struct ValidBigUnsignedNumValueRange
{
    static_assert(TMinValue <= TMaxValue, "Invalid range");
};

/// @brief Similar to @ref ValidBigUnsignedNumValueRange, but overrides (nullifies)
///     all previously set valid values ranges.
/// @see @ref ValidNumValueOverride
/// @see @ref ValidBigUnsignedNumValueOverride
/// @deprecated Use @ref ValidRangesClear instead.
template<std::uintmax_t TMinValue, std::uintmax_t TMaxValue>
using ValidBigUnsignedNumValueRangeOverride =
    std::tuple<
        ValidBigUnsignedNumValueRange<TMinValue, TMaxValue>,
        ValidRangesClear
    >;

/// @brief Alias to @ref ValidBigUnsignedNumValueRange.
/// @details Equivalent to @b ValidBigUnsignedNumValueRange<TValue, TValue>
template<std::uintmax_t TValue>
using ValidBigUnsignedNumValue = ValidBigUnsignedNumValueRange<TValue, TValue>;

/// @brief Alias to @ref ValidBigUnsignedNumValueRangeOverride.
/// @details Equivalent to @b ValidBigUnsignedNumValueRangeOverride<TValue, TValue>
/// @deprecated Use @ref ValidRangesClear instead.
template<std::uintmax_t TValue>
using ValidBigUnsignedNumValueOverride = ValidBigUnsignedNumValueRangeOverride<TValue, TValue>;

/// @brief Alias to ContentsValidator, it defines validator class that checks
///     that reserved bits of the field have expected values.
/// @details It is usually used with comms::field::BitmaskValue field to
///     specify values of the unused/reserved bits.
///     The custom validator will return true if
///     @code
///     (field.value() & TMask) == TValue
///     @endcode
/// @tparam TMask Mask that specifies reserved bits.
/// @tparam TValue Expected value of the reserved bits. Defaults to 0.
/// @headerfile comms/options.h
template<std::uintmax_t TMask, std::uintmax_t TValue = 0U>
using BitmaskReservedBits = ContentsValidator<details::BitmaskReservedBitsValidator<TMask, TValue> >;

/// @brief Alias to DefaultValueInitialiser, it sets default mode
///     to field::Optional field.
/// @tparam TVal Optional mode value is to be assigned to the field in default constructor.
/// @see @ref MissingByDefault
/// @see @ref ExistsByDefault
/// @headerfile comms/options.h
template<comms::field::OptionalMode TVal>
using DefaultOptionalMode = DefaultValueInitialiser<details::DefaultOptModeInitialiser<TVal> >;

/// @brief Alias to @ref DefaultOptionalMode.
/// @details Equivalent to
///     @code
///     DefaultOptionalMode<comms::field::OptionalMode::Missing>
///     @endcode
using MissingByDefault = DefaultOptionalMode<comms::field::OptionalMode::Missing>;

/// @brief Alias to @ref DefaultOptionalMode.
/// @details Equivalent to
///     @code
///     DefaultOptionalMode<comms::field::OptionalMode::Exists>
///     @endcode
using ExistsByDefault = DefaultOptionalMode<comms::field::OptionalMode::Exists>;

/// @brief Alias to DefaultOptionalMode<comms::field::OptinalMode::Missing>
using OptionalMissingByDefault = MissingByDefault;

/// @brief Alias to DefaultOptionalMode<comms::field::OptinalMode::Exists>
using OptionalExistsByDefault = ExistsByDefault;

/// @brief Alias to DefaultValueInitialiser, it initalises comms::field::Variant field
///     to contain valid default value of the specified member.
/// @tparam TIdx Index of the default member.
/// @headerfile comms/options.h
template <std::size_t TIdx>
using DefaultVariantIndex = DefaultValueInitialiser<details::DefaultVariantIndexInitialiser<TIdx> >;

/// @brief Force comms::protocol::ChecksumLayer and
///     comms::protocol::ChecksumPrefixLayer, to verify checksum prior to
///     forwarding read to the wrapped layer(s).
/// @headerfile comms/options.h
struct ChecksumLayerVerifyBeforeRead {};

/// @brief Force field not to be serialized during read/write operations
/// @details Some protocols may define some constant values that are predefined
///     and are not present on I/O link when serialized. Sometimes it is convenient
///     to have such values abstracted away as fields, which are not actually
///     serialised. Using this option will have such effect: read/write operaitons
///     will not change the value of iterators and will report immediate success.
///     The serialisation length is always reported as 0.
/// @headerfile comms/options.h
struct EmptySerialization {};

/// @brief Same as @ref EmptySerialization.
/// @details Just British English spelling.
/// @headerfile comms/options.h
using EmptySerialisation = EmptySerialization;

/// @brief Option to force @ref comms::protocol::ProtocolLayerBase class to
///     split read operation "until" and "from" data (payload) layer.
/// @details Can be used by some layers which require its read operation to be
///     fully complete before read is forwared to data layer, i.e. until message
///     contents being read.
/// @headerfile comms/options.h
struct ProtocolLayerForceReadUntilDataSplit {};

/// @brief Disallow usage of @ref ProtocolLayerForceReadUntilDataSplit option in
///     earlier (outer wrapping) layers.
/// @details Some layers, such as @ref comms::protocol::ChecksumLayer cannot
///     split their "read" operation to "until" and "from" data layer. They can
///     use this option to prevent outer layers from using
///     @ref ProtocolLayerForceReadUntilDataSplit one.
/// @headerfile comms/options.h
struct ProtocolLayerDisallowReadUntilDataSplit {};

/// @brief Mark field class to have custom
///     implementation of @b read functionality.
/// @headerfile comms/options.h
struct HasCustomRead {};

/// @brief Mark message / field class to have custom
///     implementation of @b refresh functionality.
/// @headerfile comms/options.h
struct HasCustomRefresh {};

/// @brief Mark field class to have custom
///     implementation of @b read functionality.
/// @headerfile comms/options.h
struct HasCustomWrite {};

/// @brief Mark message class as providing its name information
/// @headerfile comms/options.h
struct HasName {};

/// @brief Option that notifies comms::MessageBase about existence of
///     custom refresh functionality in derived class.
/// @details Alias to @ref HasCustomRefresh for backward compatibility.
/// @deprecated Use @ref HasCustomRefresh instead.
/// @headerfile comms/options.h
using HasDoRefresh = HasCustomRefresh;

/// @brief Option for @ref comms::protocol::TransportValueLayer to
///     mark that the handled field is a "pseudo" one, i.e. is not serialised.
struct PseudoValue {};

/// @brief Provide type to be used for versioning
/// @tparam T Type of the version value. Expected to be unsigned integral one.
template <typename T>
struct VersionType
{
    static_assert(std::is_integral<T>::value, "Only unsigned integral types are supported for versions");
    static_assert(std::is_unsigned<T>::value, "Only unsigned integral types are supported for versions");
};

/// @brief Mark message / field class to have custom
///     implementation of version update functionality.
/// @headerfile comms/options.h
struct HasCustomVersionUpdate {};

/// @brief Mark an @ref comms::field::Optional field as existing
///     between specified versions.
/// @tparam TFrom First version when field has been added
/// @tparam TUntil Last version when field still hasn't been removed.
/// @pre @b TFrom <= @b TUntil
template <std::uintmax_t TFrom, std::uintmax_t TUntil>
struct ExistsBetweenVersions
{
    static_assert(TFrom <= TUntil, "Invalid version parameters");
};

/// @brief Mark an @ref comms::field::Optional field as existing
///     starting from specified version.
/// @details Alias to @ref ExistsBetweenVersions
/// @tparam TVer First version when field has been added
template <std::uintmax_t TVer>
using ExistsSinceVersion = ExistsBetweenVersions<TVer, std::numeric_limits<std::uintmax_t>::max()>;

/// @brief Mark an @ref comms::field::Optional field as existing
///     only until specified version.
/// @details Alias to @ref ExistsBetweenVersions
/// @tparam TVer Last version when field still hasn't been removed.
template <std::uintmax_t TVer>
using ExistsUntilVersion = ExistsBetweenVersions<0, TVer>;

/// @brief Make the field's contents to be invalid by default.
struct InvalidByDefault {};

/// @brief Add storage of version information inside private data members.
/// @details The version information can be accessed using @b getVersion() member function.
struct VersionStorage {};

/// @brief Option to specify real extending class.
/// @details Used for some layer classes in @ref comms::protocol namespace.
template <typename T>
struct ExtendingClass {};

/// @brief Option to specify index of member field containing remaining length in bytes
/// @details Applicable only to @ref comms::field::Bundle fields.
template <std::size_t TIdx>
struct RemLengthMemberField {};

} // namespace def

namespace app
{

/// @brief No-op option, doesn't have any effect.
/// @headerfile comms/options.h
struct EmptyOption {};

/// @brief Option used to specify type of iterator used for reading.
/// @tparam TIter Type of the iterator.
/// @headerfile comms/options.h
template <typename TIter>
struct ReadIterator {};

/// @brief Option used to specify type of iterator used for writing.
/// @tparam TIter Type of the iterator.
/// @headerfile comms/options.h
template <typename TIter>
struct WriteIterator {};

/// @brief Option used to add @b getId() function into Message interface.
/// @headerfile comms/options.h
struct IdInfoInterface {};

/// @brief Option used to add @b valid() function into Message interface.
/// @headerfile comms/options.h
struct ValidCheckInterface {};

/// @brief Option used to add @b length() function into Message interface.
/// @headerfile comms/options.h
struct LengthInfoInterface {};

/// @brief Option used to add @b refresh() function into Message interface.
/// @headerfile comms/options.h
struct RefreshInterface {};

/// @brief Option used to add @b name() function into Message interface.
/// @headerfile comms/options.h
struct NameInterface {};

/// @brief Option used to specify type of the message handler.
/// @tparam T Type of the handler.
/// @headerfile comms/options.h
template <typename T>
struct Handler {};

/// @brief Option used to inhibit default implementation of @b dispatchImpl()
///     in comms::MessageBase.
/// @headerfile comms/options.h
struct NoDispatchImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::readImpl()
///     regardless of other availability conditions.
/// @headerfile comms/options.h
struct NoReadImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::writeImpl()
///     regardless of other availability conditions.
/// @headerfile comms/options.h
struct NoWriteImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::validImpl()
///     regardless of other availability conditions.
/// @headerfile comms/options.h
struct NoValidImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::lengthImpl()
///     regardless of other availability conditions.
/// @headerfile comms/options.h
struct NoLengthImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::refreshImpl()
///     regardless of other availability conditions.
/// @headerfile comms/options.h
struct NoRefreshImpl {};

/// @brief Option that forces "in place" allocation with placement "new" for
///     initialisation, instead of usage of dynamic memory allocation.
/// @headerfile comms/options.h
struct InPlaceAllocation {};

/// @brief Option used to allow @ref comms::GenericMessage generation inside
///  @ref comms::MsgFactory and/or @ref comms::protocol::MsgIdLayer classes.
/// @tparam TGenericMessage Type of message, expected to be a variant of
///     @ref comms::GenericMessage.
template <typename TGenericMessage>
struct SupportGenericMessage {};

/// @brief Option that forces usage of embedded uninitialised data area instead
///     of dynamic memory allocation.
/// @details Applicable to fields that represent collection of raw data or other
///     fields, such as comms::field::ArrayList or comms::field::String. By
///     default, these fields will use
///     <a href="http://en.cppreference.com/w/cpp/container/vector">std::vector</a> or
///     <a href="http://en.cppreference.com/w/cpp/string/basic_string">std::string</a>
///     for their internal data storage. If this option is used, it will force
///     such fields to use @ref comms::util::StaticVector or @ref comms::util::StaticString
///     with the capacity provided by this option.
/// @tparam TSize Size of the storage area in number of elements, for strings it does @b NOT include
///     the '\0' terminating character.
/// @headerfile comms/options.h
template <std::size_t TSize>
struct FixedSizeStorage {};

/// @brief Set custom storage type for fields like comms::field::String or
///     comms::field::ArrayList.
/// @details By default comms::field::String uses
///     <a href="http://en.cppreference.com/w/cpp/string/basic_string">std::string</a>
///     and comms::field::ArrayList uses
///     <a href="http://en.cppreference.com/w/cpp/container/vector">std::vector</a> as
///     their internal storage types. The @ref FixedSizeStorage option forces
///     them to use comms::util::StaticString and comms::util::StaticVector
///     instead. This option can be used to provide any other third party type.
///     Such type must define the same public interface as @b std::string (when used
///     with comms::field::String) or @b std::vector (when used with
///     comms::field::ArrayList).
/// @tparam TType Custom storage type
/// @headerfile comms/options.h
template <typename TType>
struct CustomStorageType {};

/// @brief Option that forces usage of fixed size storage for sequences with fixed
///     size.
/// @details Equivalent to @ref FixedSizeStorage option, but applicable only
///     to sequence types @ref comms::field::ArrayList or @ref comms::field::String, that
///     alrady use @ref SequenceFixedSize option. Usage of this option do not
///     require knowledge of the storage area size.
/// @headerfile comms/options.h
struct SequenceFixedSizeUseFixedSizeStorage {};

/// @brief Force the destructor of comms::Message class to be @b non-virtual,
///     even if there are other virtual functions defined.
/// @headerfile comms/options.h
struct NoVirtualDestructor {};

/// @brief Use "view" on original raw data instead of copying it.
/// @details Can be used with @ref comms::field::String and raw data @ref comms::field::ArrayList.@n
///     For @ref comms::field::String it will force usage of 
///     <a href="https://en.cppreference.com/w/cpp/string/basic_string_view">std::string_view</a> (if available) as 
///     inner storage type (instead of @b std::string). In case @b std::string_view is unavalable 
///     (C++17 support is disabled or standard library of insufficient version) @ref comms::util::StringView
///     will be used instead.@n
///     For raw data @ref comms::field::ArrayList it will force usage of 
///     <a href="https://en.cppreference.com/w/cpp/container/span">std::span&lt;std::uint8_t&gt;</a>(if available) as 
///     inner storage type (instead of @b std::vector<std::uint8_t>). In case @b std::span is unavalable 
///     (C++20 support is disabled or standard library of insufficient version) @ref comms::util::ArrayView
///     will be used instead.@n
/// @note The original data must be preserved until destruction of the field
///     that uses the "view".
/// @note Incompatible with other options that contol data storage type,
///     such as @ref comms::option::CustomStorageType or @ref comms::option::FixedSizeStorage
/// @note To force usage of provided @ref comms::util::StringView or @ref comms::util::ArrayView
///     instead of standard @b std::string_view or @b std::span, use @ref comms::option::CustomStorageType
///     option.
/// @headerfile comms/options.h
struct OrigDataView {};

/// @brief Force a particular way to dispatch message object and/or type.
/// @tparam T Expected to be one of the tags from @ref comms::traits::dispatch namespace.
template <typename T>
struct ForceDispatch {};

/// @brief Force generation of v-tables and polymorphic dispatch logic for
///     message object and/or message object type
using ForceDispatchPolymorphic = ForceDispatch<comms::traits::dispatch::Polymorphic>;

/// @brief Force generation of static binary search dispatch logic for
///     message object and/or message object type
using ForceDispatchStaticBinSearch = ForceDispatch<comms::traits::dispatch::StaticBinSearch>;

/// @brief Force generation of linear switch statmenets for dispatch logic of
///     message object and/or message object type
using ForceDispatchLinearSwitch = ForceDispatch<comms::traits::dispatch::LinearSwitch>;

} // namespace app

// Definition options

/// @brief Same as @ref comms::option::def::Endian.
template <typename TEndian>
using Endian = comms::option::def::Endian<TEndian>;

/// @brief Same as @ref comms::option::def::BigEndian
using BigEndian = comms::option::def::BigEndian;

/// @brief Same as @ref comms::option::def::LittleEndian
using LittleEndian = comms::option::def::LittleEndian;

/// @brief Same as @ref comms::option::def::MsgIdType
template <typename T>
using MsgIdType = comms::option::def::MsgIdType<T>;

/// @brief Same as @ref comms::option::def::StaticNumIdImpl
template <std::intmax_t TId>
using StaticNumIdImpl = comms::option::def::StaticNumIdImpl<TId>;

/// @brief Same as @ref comms::option::def::NoIdImpl
using NoIdImpl = comms::option::def::NoIdImpl;

/// @brief Same as @ref comms::option::def::MsgType
template <typename TMsg>
using MsgType = comms::option::def::MsgType<TMsg>;

/// @brief Same as @ref comms::option::def::ExtraTransportFields
template <typename TFields>
using ExtraTransportFields = comms::option::def::ExtraTransportFields<TFields>;

/// @brief Same as @ref comms::option::def::VersionInExtraTransportFields
template <std::size_t TIdx>
using VersionInExtraTransportFields = comms::option::def::VersionInExtraTransportFields<TIdx>;

/// @brief Same as @ref comms::option::def::FieldsImpl
template <typename TFields>
using FieldsImpl = comms::option::def::FieldsImpl<TFields>;

/// @brief Same as @ref comms::option::def::ZeroFieldsImpl
using ZeroFieldsImpl = comms::option::def::ZeroFieldsImpl;

/// @brief Same as @ref comms::option::def::HasDoGetId
using HasDoGetId = comms::option::def::HasDoGetId;

/// @brief Same as @ref comms::option::def::AssumeFieldsExistence
using AssumeFieldsExistence = comms::option::def::AssumeFieldsExistence;

/// @brief Same as @ref comms::option::def::FixedLength
template<std::size_t TLen, bool TSignExtend = true>
using FixedLength = comms::option::def::FixedLength<TLen, TSignExtend>;

/// @brief Same as @ref comms::option::def::FixedBitLength
template<std::size_t TLen>
using FixedBitLength = comms::option::def::FixedBitLength<TLen>;

/// @brief Same as @ref comms::option::def::VarLength
template<std::size_t TMin, std::size_t TMax>
using VarLength = comms::option::def::VarLength<TMin, TMax>;

/// @brief Same as @ref comms::option::def::NumValueSerOffset
template<std::intmax_t TOffset>
using NumValueSerOffset = comms::option::def::NumValueSerOffset<TOffset>;

/// @brief Same as @ref comms::option::def::ScalingRatio
template <std::intmax_t TNum, std::intmax_t TDenom>
using ScalingRatio = comms::option::def::ScalingRatio<TNum, TDenom>;

/// @brief Same as @ref comms::option::def::SequenceSizeFieldPrefix
template <typename TField>
using SequenceSizeFieldPrefix = comms::option::def::SequenceSizeFieldPrefix<TField>;

/// @brief Same as @ref comms::option::def::SequenceSerLengthFieldPrefix
template <typename TField, comms::ErrorStatus TReadErrorStatus = comms::ErrorStatus::InvalidMsgData>
using SequenceSerLengthFieldPrefix =
    comms::option::def::SequenceSerLengthFieldPrefix<TField, TReadErrorStatus>;

/// @brief Same as @ref comms::option::def::SequenceElemSerLengthFieldPrefix
template <typename TField, comms::ErrorStatus TReadErrorStatus = comms::ErrorStatus::InvalidMsgData>
using SequenceElemSerLengthFieldPrefix =
    comms::option::def::SequenceElemSerLengthFieldPrefix<TField, TReadErrorStatus>;

/// @brief Same as @ref comms::option::def::SequenceElemFixedSerLengthFieldPrefix
template <typename TField, comms::ErrorStatus TReadErrorStatus = comms::ErrorStatus::InvalidMsgData>
using SequenceElemFixedSerLengthFieldPrefix =
    comms::option::def::SequenceElemFixedSerLengthFieldPrefix<TField, TReadErrorStatus>;

/// @brief Same as @ref comms::option::def::SequenceTerminationFieldSuffix
template <typename TField>
using SequenceTerminationFieldSuffix = comms::option::def::SequenceTerminationFieldSuffix<TField>;

/// @brief Same as @ref comms::option::def::SequenceTrailingFieldSuffix
template <typename TField>
using SequenceTrailingFieldSuffix = comms::option::def::SequenceTrailingFieldSuffix<TField>;

/// @brief Same as @ref comms::option::def::SequenceSizeForcingEnabled
using SequenceSizeForcingEnabled = comms::option::def::SequenceSizeForcingEnabled;

/// @brief Same as @ref comms::option::def::SequenceLengthForcingEnabled
using SequenceLengthForcingEnabled = comms::option::def::SequenceLengthForcingEnabled;

/// @brief Same as @ref comms::option::def::SequenceElemLengthForcingEnabled
using SequenceElemLengthForcingEnabled = comms::option::def::SequenceElemLengthForcingEnabled;

/// @brief Same as @ref comms::option::def::SequenceFixedSize
template <std::size_t TSize>
using SequenceFixedSize = comms::option::def::SequenceFixedSize<TSize>;

/// @brief Same as @ref comms::option::def::DefaultValueInitialiser
template <typename T>
using DefaultValueInitialiser = comms::option::def::DefaultValueInitialiser<T>;

/// @brief Same as @ref comms::option::def::ContentsValidator
template <typename T>
using ContentsValidator = comms::option::def::ContentsValidator<T>;

/// @brief Same as @ref comms::option::def::ContentsRefresher
template <typename T>
using ContentsRefresher = comms::option::def::ContentsRefresher<T>;

/// @brief Same as @ref comms::option::def::CustomValueReader
template <typename T>
using CustomValueReader = comms::option::def::CustomValueReader<T>;

/// @brief Same as @ref comms::option::def::FailOnInvalid
template <comms::ErrorStatus TStatus = comms::ErrorStatus::InvalidMsgData>
using FailOnInvalid = comms::option::def::FailOnInvalid<TStatus>;

/// @brief Same as @ref comms::option::def::IgnoreInvalid
using IgnoreInvalid = comms::option::def::IgnoreInvalid;

/// @brief Same as @ref comms::option::def::Units
template <typename TType, typename TRatio>
using Units = comms::option::def::Units<TType, TRatio>;

/// @brief Same as @ref comms::option::def::UnitsNanoseconds
using UnitsNanoseconds = comms::option::def::UnitsNanoseconds;

/// @brief Same as @ref comms::option::def::UnitsMicroseconds
using UnitsMicroseconds = comms::option::def::UnitsMicroseconds;

/// @brief Same as @ref comms::option::def::UnitsMilliseconds
using UnitsMilliseconds = comms::option::def::UnitsMilliseconds;

/// @brief Same as @ref comms::option::def::UnitsSeconds
using UnitsSeconds = comms::option::def::UnitsSeconds;

/// @brief Same as @ref comms::option::def::UnitsMinutes
using UnitsMinutes = comms::option::def::UnitsMinutes;

/// @brief Same as @ref comms::option::def::UnitsHours
using UnitsHours = comms::option::def::UnitsHours;

/// @brief Same as @ref comms::option::def::UnitsDays
using UnitsDays = comms::option::def::UnitsDays;

/// @brief Same as @ref comms::option::def::UnitsWeeks
using UnitsWeeks = comms::option::def::UnitsWeeks;

/// @brief Same as @ref comms::option::def::UnitsNanometers
using UnitsNanometers = comms::option::def::UnitsNanometers;

/// @brief Same as @ref comms::option::def::UnitsMicrometers
using UnitsMicrometers = comms::option::def::UnitsMicrometers;

/// @brief Same as @ref comms::option::def::UnitsMillimeters
using UnitsMillimeters = comms::option::def::UnitsMillimeters;

/// @brief Same as @ref comms::option::def::UnitsCentimeters
using UnitsCentimeters = comms::option::def::UnitsCentimeters;

/// @brief Same as @ref comms::option::def::UnitsMeters
using UnitsMeters = comms::option::def::UnitsMeters;

/// @brief Same as @ref comms::option::def::UnitsKilometers
using UnitsKilometers = comms::option::def::UnitsKilometers;

/// @brief Same as @ref comms::option::def::UnitsNanometersPerSecond
using UnitsNanometersPerSecond = comms::option::def::UnitsNanometersPerSecond;

/// @brief Same as @ref comms::option::def::UnitsMicrometersPerSecond
using UnitsMicrometersPerSecond = comms::option::def::UnitsMicrometersPerSecond;

/// @brief Same as @ref comms::option::def::UnitsMillimetersPerSecond
using UnitsMillimetersPerSecond = comms::option::def::UnitsMillimetersPerSecond;

/// @brief Same as @ref comms::option::def::UnitsCentimetersPerSecond
using UnitsCentimetersPerSecond = comms::option::def::UnitsCentimetersPerSecond;

/// @brief Same as @ref comms::option::def::UnitsMetersPerSecond
using UnitsMetersPerSecond = comms::option::def::UnitsMetersPerSecond;

/// @brief Same as @ref comms::option::def::UnitsKilometersPerSecond
using UnitsKilometersPerSecond = comms::option::def::UnitsKilometersPerSecond;

/// @brief Same as @ref comms::option::def::UnitsKilometersPerHour
using UnitsKilometersPerHour = comms::option::def::UnitsKilometersPerHour;

/// @brief Same as @ref comms::option::def::UnitsHertz
using UnitsHertz = comms::option::def::UnitsHertz;

/// @brief Same as @ref comms::option::def::UnitsKilohertz
using UnitsKilohertz = comms::option::def::UnitsKilohertz;

/// @brief Same as @ref comms::option::def::UnitsMegahertz
using UnitsMegahertz = comms::option::def::UnitsMegahertz;

/// @brief Same as @ref comms::option::def::UnitsGigahertz
using UnitsGigahertz = comms::option::def::UnitsGigahertz;

/// @brief Same as @ref comms::option::def::UnitsDegrees
using UnitsDegrees = comms::option::def::UnitsDegrees;

/// @brief Same as @ref comms::option::def::UnitsRadians
using UnitsRadians = comms::option::def::UnitsRadians;

/// @brief Same as @ref comms::option::def::UnitsNanoamps
using UnitsNanoamps = comms::option::def::UnitsNanoamps;

/// @brief Same as @ref comms::option::def::UnitsMicroamps
using UnitsMicroamps = comms::option::def::UnitsMicroamps;

/// @brief Same as @ref comms::option::def::UnitsMilliamps
using UnitsMilliamps = comms::option::def::UnitsMilliamps;

/// @brief Same as @ref comms::option::def::UnitsAmps
using UnitsAmps = comms::option::def::UnitsAmps;

/// @brief Same as @ref comms::option::def::UnitsKiloamps
using UnitsKiloamps = comms::option::def::UnitsKiloamps;

/// @brief Same as @ref comms::option::def::UnitsNanovolts
using UnitsNanovolts = comms::option::def::UnitsNanovolts;

/// @brief Same as @ref comms::option::def::UnitsMicrovolts
using UnitsMicrovolts = comms::option::def::UnitsMicrovolts;

/// @brief Same as @ref comms::option::def::UnitsMillivolts
using UnitsMillivolts = comms::option::def::UnitsMillivolts;

/// @brief Same as @ref comms::option::def::UnitsVolts
using UnitsVolts = comms::option::def::UnitsVolts;

/// @brief Same as @ref comms::option::def::UnitsKilovolts
using UnitsKilovolts = comms::option::def::UnitsKilovolts;

/// @brief Same as @ref comms::option::def::UnitsBytes
using UnitsBytes = comms::option::def::UnitsBytes;

/// @brief Same as @ref comms::option::def::UnitsKilobytes
using UnitsKilobytes = comms::option::def::UnitsKilobytes;

/// @brief Same as @ref comms::option::def::UnitsMegabytes
using UnitsMegabytes = comms::option::def::UnitsMegabytes;

/// @brief Same as @ref comms::option::def::UnitsGigabytes
using UnitsGigabytes = comms::option::def::UnitsGigabytes;

/// @brief Same as @ref comms::option::def::UnitsTerabytes
using UnitsTerabytes = comms::option::def::UnitsTerabytes;

/// @brief Same as @ref comms::option::def::DefaultNumValue
template<std::intmax_t TVal>
using DefaultNumValue = comms::option::def::DefaultNumValue<TVal>;

/// @brief Same as @ref comms::option::def::DefaultBigUnsignedNumValue
template<std::uintmax_t TVal>
using DefaultBigUnsignedNumValue = comms::option::def::DefaultBigUnsignedNumValue<TVal>;

/// @brief Same as @ref comms::option::def::ValidNumValueRange
template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
using ValidNumValueRange = comms::option::def::ValidNumValueRange<TMinValue, TMaxValue>;

/// @brief Same as @ref comms::option::def::ValidRangesClear
using ValidRangesClear = comms::option::def::ValidRangesClear;

/// @brief Same as @ref comms::option::def::ValidNumValueRangeOverride
/// @deprecated Use @ref ValidRangesClear instead.
template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
using ValidNumValueRangeOverride = comms::option::def::ValidNumValueRangeOverride<TMinValue, TMaxValue>;

/// @brief Same as @ref comms::option::def::ValidNumValue
template<std::intmax_t TValue>
using ValidNumValue = comms::option::def::ValidNumValue<TValue>;

/// @brief Same as @ref comms::option::def::ValidNumValueOverride
/// @deprecated Use @ref ValidRangesClear instead.
template<std::intmax_t TValue>
using ValidNumValueOverride = comms::option::def::ValidNumValueOverride<TValue>;

/// @brief Same as @ref comms::option::def::ValidBigUnsignedNumValueRange
template<std::uintmax_t TMinValue, std::uintmax_t TMaxValue>
using ValidBigUnsignedNumValueRange =
    comms::option::def::ValidBigUnsignedNumValueRange<TMinValue, TMaxValue>;

/// @brief Same as @ref comms::option::def::ValidBigUnsignedNumValueRangeOverride
/// @deprecated Use @ref ValidRangesClear instead.
template<std::uintmax_t TMinValue, std::uintmax_t TMaxValue>
using ValidBigUnsignedNumValueRangeOverride =
    comms::option::def::ValidBigUnsignedNumValueRangeOverride<TMinValue, TMaxValue>;

/// @brief Same as @ref comms::option::def::ValidBigUnsignedNumValue
template<std::uintmax_t TValue>
using ValidBigUnsignedNumValue =
    comms::option::def::ValidBigUnsignedNumValue<TValue>;

/// @brief Same as @ref comms::option::def::ValidBigUnsignedNumValueOverride
/// @deprecated Use @ref ValidRangesClear instead.
template<std::uintmax_t TValue>
using ValidBigUnsignedNumValueOverride =
    comms::option::def::ValidBigUnsignedNumValueOverride<TValue>;

/// @brief Same as @ref comms::option::def::BitmaskReservedBits
template<std::uintmax_t TMask, std::uintmax_t TValue = 0U>
using BitmaskReservedBits = comms::option::def::BitmaskReservedBits<TMask, TValue>;

/// @brief Same as @ref comms::option::def::DefaultOptionalMode
template<comms::field::OptionalMode TVal>
using DefaultOptionalMode = comms::option::def::DefaultOptionalMode<TVal>;

/// @brief Same as @ref comms::option::def::MissingByDefault
using MissingByDefault = comms::option::def::MissingByDefault;

/// @brief Same as @ref comms::option::def::ExistsByDefault
using ExistsByDefault = comms::option::def::ExistsByDefault;

/// @brief Same as @ref comms::option::def::OptionalMissingByDefault
using OptionalMissingByDefault = comms::option::def::OptionalMissingByDefault;

/// @brief Same as @ref comms::option::def::OptionalMissingByDefault
using OptionalExistsByDefault = comms::option::def::OptionalMissingByDefault;

/// @brief Same as @ref comms::option::def::DefaultVariantIndex
template <std::size_t TIdx>
using DefaultVariantIndex = comms::option::def::DefaultVariantIndex<TIdx>;

/// @brief Same as @ref comms::option::def::ChecksumLayerVerifyBeforeRead
using ChecksumLayerVerifyBeforeRead = comms::option::def::ChecksumLayerVerifyBeforeRead;

/// @brief Same as @ref comms::option::def::EmptySerialization
using EmptySerialization = comms::option::def::EmptySerialization;

/// @brief Same as @ref comms::option::def::EmptySerialisation
using EmptySerialisation = comms::option::def::EmptySerialisation;

/// @brief Same as @ref comms::option::def::ProtocolLayerForceReadUntilDataSplit
using ProtocolLayerForceReadUntilDataSplit =
    comms::option::def::ProtocolLayerForceReadUntilDataSplit;

/// @brief Same as @ref comms::option::def::ProtocolLayerDisallowReadUntilDataSplit
using ProtocolLayerDisallowReadUntilDataSplit =
    comms::option::def::ProtocolLayerDisallowReadUntilDataSplit;

/// @brief Same as @ref comms::option::def::HasCustomRead
using HasCustomRead = comms::option::def::HasCustomRead;

/// @brief Same as @ref comms::option::def::HasCustomRefresh
using HasCustomRefresh = comms::option::def::HasCustomRefresh;

/// @brief Same as @ref comms::option::def::HasName
using HasName = comms::option::def::HasName;

/// @brief Same as @ref comms::option::def::HasDoRefresh
/// @deprecated Use @ref HasCustomRefresh instead.
using HasDoRefresh = comms::option::def::HasDoRefresh;

/// @brief Same as @ref comms::option::def::PseudoValue
using PseudoValue = comms::option::def::PseudoValue;

/// @brief Same as @ref comms::option::def::VersionType
template <typename T>
using VersionType = comms::option::def::VersionType<T>;

/// @brief Same as @ref comms::option::def::HasCustomVersionUpdate
using HasCustomVersionUpdate = comms::option::def::HasCustomVersionUpdate;

/// @brief Same as @ref comms::option::def::ExistsBetweenVersions
template <std::uintmax_t TFrom, std::uintmax_t TUntil>
using ExistsBetweenVersions = comms::option::def::ExistsBetweenVersions<TFrom, TUntil>;

/// @brief Same as @ref comms::option::def::ExistsSinceVersion
template <std::uintmax_t TVer>
using ExistsSinceVersion = comms::option::def::ExistsSinceVersion<TVer>;

/// @brief Same as @ref comms::option::def::ExistsUntilVersion
template <std::uintmax_t TVer>
using ExistsUntilVersion = comms::option::def::ExistsUntilVersion<TVer>;

/// @brief Same as @ref comms::option::def::InvalidByDefault
using InvalidByDefault = comms::option::def::InvalidByDefault;

/// @brief Same as @ref comms::option::def::VersionStorage
using VersionStorage = comms::option::def::VersionStorage;

/// @brief Same as @ref comms::option::def::ExtendingClass
template <typename T>
using ExtendingClass = comms::option::def::ExtendingClass<T>;

/// @brief Same as @ref comms::option::def::RemLengthMemberField
template <std::size_t TIdx>
using RemLengthMemberField = comms::option::def::RemLengthMemberField<TIdx>;

// Application customization options

/// @brief Same as @ref comms::option::app::EmptyOption
using EmptyOption = comms::option::app::EmptyOption;

/// @brief Same as @ref comms::option::app::ReadIterator
template <typename TIter>
using ReadIterator = comms::option::app::ReadIterator<TIter>;

/// @brief Same as @ref comms::option::app::WriteIterator
template <typename TIter>
using WriteIterator = comms::option::app::WriteIterator<TIter>;

/// @brief Same as @ref comms::option::app::IdInfoInterface
using IdInfoInterface = comms::option::app::IdInfoInterface;

/// @brief Same as @ref comms::option::app::ValidCheckInterface
using ValidCheckInterface = comms::option::app::ValidCheckInterface;

/// @brief Same as @ref comms::option::app::LengthInfoInterface
using LengthInfoInterface = comms::option::app::LengthInfoInterface;

/// @brief Same as @ref comms::option::app::RefreshInterface
using RefreshInterface = comms::option::app::RefreshInterface;

/// @brief Same as @ref comms::option::app::NameInterface
using NameInterface = comms::option::app::NameInterface;

/// @brief Same as @ref comms::option::app::Handler
template <typename T>
using Handler = comms::option::app::Handler<T>;

/// @brief Same as @ref comms::option::app::NoDispatchImpl
using NoDispatchImpl = comms::option::app::NoDispatchImpl;

/// @brief Same as @ref comms::option::app::NoReadImpl
using NoReadImpl = comms::option::app::NoReadImpl;

/// @brief Same as @ref comms::option::app::NoWriteImpl
using NoWriteImpl = comms::option::app::NoWriteImpl;

/// @brief Same as @ref comms::option::app::NoValidImpl
using NoValidImpl = comms::option::app::NoValidImpl;

/// @brief Same as @ref comms::option::app::NoLengthImpl
using NoLengthImpl = comms::option::app::NoLengthImpl;

/// @brief Same as @ref comms::option::app::NoRefreshImpl
using NoRefreshImpl = comms::option::app::NoRefreshImpl;

/// @brief Same as @ref comms::option::app::InPlaceAllocation
using InPlaceAllocation = comms::option::app::InPlaceAllocation;

/// @brief Same as @ref comms::option::app::SupportGenericMessage
template <typename TGenericMessage>
using SupportGenericMessage = comms::option::app::SupportGenericMessage<TGenericMessage>;

/// @brief Same as @ref comms::option::app::FixedSizeStorage
template <std::size_t TSize>
using FixedSizeStorage = comms::option::app::FixedSizeStorage<TSize>;

/// @brief Same as @ref comms::option::app::CustomStorageType
template <typename TType>
using CustomStorageType = comms::option::app::CustomStorageType<TType>;

/// @brief Same as @ref comms::option::app::SequenceFixedSizeUseFixedSizeStorage
using SequenceFixedSizeUseFixedSizeStorage = comms::option::app::SequenceFixedSizeUseFixedSizeStorage;

/// @brief Same as @ref comms::option::app::NoVirtualDestructor
using NoVirtualDestructor = comms::option::app::NoVirtualDestructor;

/// @brief Same as @ref comms::option::app::OrigDataView
using OrigDataView = comms::option::app::OrigDataView;

/// @brief Same as @ref comms::option::app::ForceDispatch
template <typename T>
using ForceDispatch = comms::option::app::ForceDispatch<T>;

/// @brief Same as @ref comms::option::app::ForceDispatchPolymorphic
using ForceDispatchPolymorphic = comms::option::app::ForceDispatchPolymorphic;

/// @brief Same as @ref comms::option::app::ForceDispatchStaticBinSearch
using ForceDispatchStaticBinSearch = comms::option::app::ForceDispatchStaticBinSearch;

/// @brief Same as @ref comms::option::app::ForceDispatchLinearSwitch
using ForceDispatchLinearSwitch = comms::option::app::ForceDispatchLinearSwitch;

}  // namespace option

}  // namespace comms


