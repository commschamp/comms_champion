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


#pragma once

#include <tuple>
#include <type_traits>
#include <limits>
#include <ratio>

#include "comms/traits.h"
#include "comms/ErrorStatus.h"

namespace comms
{

namespace option
{

// Message/Field common options

/// @brief Options to specify endian.
/// @tparam TEndian Endian type. Must be either comms::traits::endian::Big or
///     comms::traits::endian::Little.
template <typename TEndian>
struct Endian
{
    typedef TEndian Type;
};

/// @brief Alias option to Endian specifying Big endian.
using BigEndian = Endian<comms::traits::endian::Big>;

/// @brief Alias option to Endian specifying Little endian.
using LittleEndian = Endian<comms::traits::endian::Little>;

/// @brief No-op option, doesn't have any effect.
struct EmptyOption {};

/// @brief Option used to specify type of the ID.
/// @tparam T Type of the message ID.
template <typename T>
struct MsgIdType
{
    typedef T Type;
};

/// @brief Option used to specify type of iterator used for reading.
/// @tparam TIter Type of the iterator.
template <typename TIter>
struct ReadIterator
{
    typedef TIter Type;
};

/// @brief Option used to specify type of iterator used for writing.
/// @tparam TIter Type of the iterator.
template <typename TIter>
struct WriteIterator
{
    typedef TIter Type;
};

/// @brief Option used to add valid() function into Message interface.
struct ValidCheckInterface {};

/// @brief Option used to add length() function into Message interface.
struct LengthInfoInterface {};

/// @brief Option used to specify type of the message handler.
/// @tparam T Type of the handler.
template <typename T>
struct Handler
{
    typedef T Type;
};

/// @brief Option used to specify numeric ID of the message.
/// @tparam TId Numeric ID value.
template <std::intmax_t TId>
struct StaticNumIdImpl
{
    static const auto Value = TId;
};

/// @brief Option used to specify that message doesn't have valid ID.
struct NoIdImpl {};

/// @brief Option used to force implementation of dispatch functionality.
/// @details This option can be provided to comms::MessageBase to force the
///     implementation of comms::MessageBase::dispatchImpl() member function.
/// @tparam TActual Actual message type - derived from comms::MessageBase.
template <typename TActual>
struct DispatchImpl
{
    typedef TActual MsgType;
};

/// @brief Option used to specify fields of the message and force implementation
///     of default read, write, validity check, and length retrieval information
///     of the message.
/// @tparam TFields The fields of the message bundled in std::tuple.
template <typename TFields>
struct FieldsImpl;

/// @cond SKIP_DOC
template <typename... TFields>
struct FieldsImpl<std::tuple<TFields...> >
{
    typedef std::tuple<TFields...> Fields;
};
/// @endcond

/// @brief Alias to FieldsImpl<std::tuple<> >
using NoFieldsImpl = FieldsImpl<std::tuple<> >;

/// @brief Option that forces "in place" allocation with placement "new" for
///     initialisation, instead of usage of dynamic memory allocation.
struct InPlaceAllocation {};

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
template<std::size_t TLen>
struct FixedLength
{
    static const std::size_t Value = TLen;
};

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
template<std::size_t TLen>
struct FixedBitLength
{
    static const std::size_t Value = TLen;
};

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
template<std::size_t TMin, std::size_t TMax>
struct VarLength
{
    static_assert(TMin <= TMax, "TMin must not be greater that TMax.");
    static const std::size_t MinValue = TMin;
    static const std::size_t MaxValue = TMax;
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
///     comms::option:: NumValueSerOffset can be used. For example:
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
///     Note that in the example above the field value (accessible by value() member
///     function of the field) will have type std::uint16_t and will be equal to
///     say 2015, while when serialised it consumes only 1 byte (thanks to
///     comms::option::FixedLength option) and reduced value of 15 is written.
/// @tparam TOffset Offset value to be added when serialising field.
template<std::intmax_t TOffset>
struct NumValueSerOffset
{
    static const auto Value = TOffset;
};

/// @brief Option that forces usage of embedded uninitialised data are instead of
///     dynamic memory allocation.
/// @details Applicable to fields that represent collection of raw data or other
///     fields, such as comms::field::ArrayList or comms::field::String. By
///     default, these fields will use
///     <a href="http://en.cppreference.com/w/cpp/container/vector">std::vector</a> or
///     <a href="http://en.cppreference.com/w/cpp/string/basic_string">std::string</a>
///     for their internad data storage. If this option is used, it will force
///     such fields to use comms::util::StaticVector or comms::util::StaticString
///     with the capacity provided by this option.
template <std::size_t TSize>
struct FixedSizeStorage
{
    static const std::size_t Value = TSize;
};

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
///     Then, to accessed the scaled value of the field use scaleAs() or
///     setScaled() methods of comms::field::IntValue field:
///     @code
///     void processField(const MyField& field)
///     {
///         auto distInMillimetres = field.value();
///         auto distInMeters = field.scaleAs<double>();
///     }
///     @endcode
/// @tparam TNum Numerator of the scaling ratio.
/// @tparam TDenom Denominator of the scaling ratio.
template <std::intmax_t TNum, std::intmax_t TDenom>
struct ScalingRatio
{
    typedef std::ratio<TNum, TDenom> Type;
};

/// @brief Option that modify the default behaviour of collection fields to
///     prepend the serialised data with number of elements information.
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
template <typename TField>
struct SequenceSizeFieldPrefix
{
    typedef TField Type;
};

/// @brief Option that forces collection fields to append provides suffix every
///     type it is serialised.
/// @details Sometimes protocols use zero-termination for strings instead of
///     prefixing them with their size. Below is an example of how to achieve
///     such termination using SequenceTrailingFieldSuffix option.
///     @code
///     using MyFieldBase = comms::Field<comms::option::BigEndian>;
///     using MyField =
///         comms::field::String<
///             MyFieldBase,
///             comms::option::SequenceTrailingFieldSuffix<
///                 comms::field::IntValue<MyFieldBase, char, comms::option::DefaultNumValue<0> >
///             >
///         >;
///     @endcode
/// @tparam TField Type of the field that represents suffix
template <typename TField>
struct SequenceTrailingFieldSuffix
{
    typedef TField Type;
};

/// @brief Option to enable external forcing of the collection's field size.
/// @details Sometimes the size information is detached from the data sequence
///     itself, i.e. there may be one or more independent fields between the
///     size field and the first byte of the collection. Usage of this function
///     enables forceReadElemCount() and clearReadElemCount() functions in
///     the collection fields, such as comms::field::ArrayList or comms::field::String.
struct SequenceSizeForcingEnabled
{
};

/// @brief Option used to define exact number of elements in the collection field.
/// @details Protocol specification may define that there is exact number of
///     elements in the sequence. Use SequenceFixedSize option to convey
///     this information to the field definition, which will force read() and
///     write() member functions of the collection field to behave as expected.
template <std::size_t TSize>
struct SequenceFixedSize
{
    static const std::size_t Value = TSize;
};

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
template <typename T>
struct DefaultValueInitialiser
{
    typedef T Type;
};

/// @brief Option that specifies custom validation class.
/// @details By default, value of every field is considered to be valid
///     (valid() member function of the field returns true). If there is a need
///     to validate the value of the function. Use this option to define
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
/// @tparam T Type of the initialiser class.
template <typename T>
struct ContentsValidator
{
    typedef T Type;
};

/// @brief Option that forces field's read operation to fail if invalid value
///     is received.
/// @details Sometimes protocol is very strict about what field's values are
///     allowed and forces to abandon a message if invalid value is received.
///     If comms::option::FailOnInvalid is provided as an option to a field,
///     the validity is going to checked automatically after the read. If invalid
///     value is identified, error will be returned from the read() operation.
struct FailOnInvalid {};

/// @brief Option that forces field's read operation to ignore read data if invalid value
///     is received.
/// @details If this option is provided to the field, the read operation will
///     check the validity of the read value. If it is identified as invalid,
///     the read value is not assigned to the field, i.e. the field's value
///     remains unchanged, although no error is reported.
struct IgnoreInvalid {};

namespace details
{

template<std::intmax_t TVal>
struct DefaultNumValueInitialiser
{
    template <typename TField>
    void operator()(TField&& field)
    {
        typedef typename std::decay<TField>::type FieldType;
        typedef typename FieldType::ValueType ValueType;
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
        typedef typename std::conditional<
            (std::numeric_limits<decltype(MinValue)>::min() < MinValue),
            CompareTag,
            ReturnTrueTag
        >::type MinTag;

        typedef typename std::conditional<
            (MaxValue < std::numeric_limits<decltype(MaxValue)>::max()),
            CompareTag,
            ReturnTrueTag
        >::type MaxTag;

        return aboveMin(field.value(), MinTag()) && belowMax(field.value(), MaxTag());
    }

private:
    struct ReturnTrueTag {};
    struct CompareTag {};

    template <typename TValue>
    static constexpr bool aboveMin(const TValue& value, CompareTag)
    {
        typedef typename std::decay<decltype(value)>::type ValueType;
        return (static_cast<ValueType>(MinValue) <= value);
    }

    template <typename TValue>
    static constexpr bool aboveMin(const TValue&, ReturnTrueTag)
    {
        return true;
    }

    template <typename TValue>
    static constexpr bool belowMax(const TValue& value, CompareTag)
    {
        typedef typename std::decay<decltype(value)>::type ValueType;
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
        typedef typename std::decay<TField>::type FieldType;
        typedef typename FieldType::ValueType ValueType;

        return (field.value() & static_cast<ValueType>(TMask)) == static_cast<ValueType>(TValue);
    }
};

}  // namespace details

/// @brief Alias to DefaultValueInitialiser, it defines initialiser class that
///     assigns numeric value provided as the template argument to this option.
/// @tparam TVal Numeric value is to be assigned to the field in default constructor.
template<std::intmax_t TVal>
using DefaultNumValue = DefaultValueInitialiser<details::DefaultNumValueInitialiser<TVal> >;

/// @brief Alias to ContentsValidator, it defines validator class that checks
///     that the field's value is between two numeric values provided template
///     parameters to this option.
/// @details Quite often numeric fields such as comms::field::IntValue or
///     comms::option::EnumValue have a single valid values range. This alias
///     to comms::option::ContentsValidator provides an easy way to specify
///     such range.
/// @tparam TMinValue Minimal valid numeric value
/// @tparam TMaxValue Maximal valid numeric value
template<std::intmax_t TMinValue, std::intmax_t TMaxValue>
using ValidNumValueRange = ContentsValidator<details::NumValueRangeValidator<TMinValue, TMaxValue> >;

/// @brief Alias to ContentsValidator, it defines validator class that checks
///     that reserved bits of the field have expected values.
/// @details It is usually used with comms::field::BitmaskValue field to
///     specify values of the unused/reserved bits.
///     The custom validator will return true if
///     @code
///     (field.value() & TMask) == TValue
///     @endcode
/// @tparam TMask Mask that specifies reserved bits.
/// @tparam TValue Expected value of the reserved bits
template<std::uintmax_t TMask, std::uintmax_t TValue>
using BitmaskReservedBits = ContentsValidator<details::BitmaskReservedBitsValidator<TMask, TValue> >;


}  // namespace option

}  // namespace comms


