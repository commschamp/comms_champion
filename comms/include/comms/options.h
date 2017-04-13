//
// Copyright 2014 - 2017 (C). Alex Robenko. All rights reserved.
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

} // namespace details

// Message/Field common options

/// @brief Options to specify endian.
/// @tparam TEndian Endian type. Must be either comms::traits::endian::Big or
///     comms::traits::endian::Little.
template <typename TEndian>
struct Endian
{
    using Type = TEndian;
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
    using Type = T;
};

/// @brief Option used to specify type of iterator used for reading.
/// @tparam TIter Type of the iterator.
template <typename TIter>
struct ReadIterator
{
    using Type = TIter;
};

/// @brief Option used to specify type of iterator used for writing.
/// @tparam TIter Type of the iterator.
template <typename TIter>
struct WriteIterator
{
    using Type = TIter;
};

/// @brief Option used to add @b getId() function into Message interface.
struct IdInfoInterface {};

/// @brief Option used to add @b valid() function into Message interface.
struct ValidCheckInterface {};

/// @brief Option used to add @b length() function into Message interface.
struct LengthInfoInterface {};

/// @brief Option used to add @b refresh() function into Message interface.
struct RefreshInterface {};

/// @brief Option used to specify type of the message handler.
/// @tparam T Type of the handler.
template <typename T>
struct Handler
{
    using Type = T;
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

/// @brief Option used to specify actual type of the message.
template <typename TMsg>
struct MsgType
{
    using Type = TMsg;
};

/// @brief Option used to inhibit default implementation of @b dispatchImpl()
///     in comms::MessageBase.
/// @details
struct NoDispatchImpl {};

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
    using Fields = std::tuple<TFields...>;
};
/// @endcond

/// @brief Alias to FieldsImpl<std::tuple<> >
using ZeroFieldsImpl = FieldsImpl<std::tuple<> >;

/// @brief Option that inhibits implementation of comms::MessageBase::readImpl()
///     regardless of other availability conditions.
struct NoReadImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::writeImpl()
///     regardless of other availability conditions.
struct NoWriteImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::validImpl()
///     regardless of other availability conditions.
struct NoValidImpl {};

/// @brief Option that inhibits implementation of comms::MessageBase::lengthImpl()
///     regardless of other availability conditions.
struct NoLengthImpl {};

/// @brief Option that notifies comms::MessageBase about existence of
///     custom refresh functionality in derived class.
struct HasDoRefresh {};

/// @brief Option that notifies comms::MessageBase about existence of
///     access to fields.
/// @details Can be useful when there is a chain of inheritances from
///     comms::MessageBase.
struct AssumeFieldsExistence {};

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
template<std::intmax_t TOffset>
struct NumValueSerOffset
{
    static const auto Value = TOffset;
};

/// @brief Option that forces usage of embedded uninitialised data area instead
///     of dynamic memory allocation.
/// @details Applicable to fields that represent collection of raw data or other
///     fields, such as comms::field::ArrayList or comms::field::String. By
///     default, these fields will use
///     <a href="http://en.cppreference.com/w/cpp/container/vector">std::vector</a> or
///     <a href="http://en.cppreference.com/w/cpp/string/basic_string">std::string</a>
///     for their internal data storage. If this option is used, it will force
///     such fields to use comms::util::StaticVector or comms::util::StaticString
///     with the capacity provided by this option.
/// @tparam TSize Size of the storage area, for strings it does @b NOT include
///     the '\0' terminating character.
template <std::size_t TSize>
struct FixedSizeStorage
{
    static const std::size_t Value = TSize;
};

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
template <typename TType>
struct CustomStorageType
{
    using Type = TType;
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
template <std::intmax_t TNum, std::intmax_t TDenom>
struct ScalingRatio
{
    using Type = std::ratio<TNum, TDenom>;
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
    using Type = TField;
};

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
template <typename TField>
struct SequenceTerminationFieldSuffix
{
    using Type = TField;
};

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
template <typename TField>
struct SequenceTrailingFieldSuffix
{
    using Type = TField;
};

/// @brief Option to enable external forcing of the collection's field size.
/// @details Sometimes the size information is detached from the data sequence
///     itself, i.e. there may be one or more independent fields between the
///     size field and the first byte of the collection. In such case it becomes
///     impossible to use SequenceSizeFieldPrefix option. Instead, the size
///     information must be provided by external calls. Usage of this option
///     enables @b forceReadElemCount() and @b clearReadElemCount() functions in
///     the collection fields, such as comms::field::ArrayList or comms::field::String
///     which can be used to specify the size information after it was read
///     independently.
struct SequenceSizeForcingEnabled
{
};

/// @brief Option used to define exact number of elements in the collection field.
/// @details Protocol specification may define that there is exact number of
///     elements in the sequence. Use SequenceFixedSize option to convey
///     this information to the field definition, which will force @b read() and
///     @b write() member functions of the collection field to behave as expected.
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
    using Type = T;
};

/// @brief Option that specifies custom validation class.
/// @details By default, value of every field is considered to be valid
///     (@b valid() member function of the field returns true). If there is a need
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
    using Type = T;
};

/// @brief Option that specifies custom value reader class.
/// @details It may be useful to override default reading functionality for complex
///     fields, such as comms::field::Bundle, where how members are read are
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
template <typename T>
struct CustomValueReader
{
    using Type = T;
};

/// @brief Option that forces field's read operation to fail if invalid value
///     is received.
/// @details Sometimes protocol is very strict about what field's values are
///     allowed and forces to abandon a message if invalid value is received.
///     If comms::option::FailOnInvalid is provided as an option to a field,
///     the validity is going to checked automatically after the read. If invalid
///     value is identified, error will be returned from the @b read() operation.
template <comms::ErrorStatus = comms::ErrorStatus::InvalidMsgData>
struct FailOnInvalid {};

/// @brief Option that forces field's read operation to ignore read data if invalid value
///     is received.
/// @details If this option is provided to the field, the read operation will
///     check the validity of the read value. If it is identified as invalid,
///     the read value is not assigned to the field, i.e. the field's value
///     remains unchanged, although no error is reported.
struct IgnoreInvalid {};

/// @brief Force the destructor of comms::Message class to be @b non-virtual,
///     even if there are other virtual functions defined.
struct NoVirtualDestructor {};

/// @brief Options to specify units of the field.
/// @tparam TType Type of the unints, can be any type from comms::traits::units
///     namespace.
/// @tparam TRatio Ratio within the units type, must be a variant of
///     @b std::ratio type.
template <typename TType, typename TRatio>
struct Units
{
    static_assert(details::isRatio<TRatio>(),
        "TRatio parameter must be a variant of std::ratio");
};

/// @brief Alias option, specifying field value units are "nanoseconds".
using UnitsNanoseconds =
    Units<comms::traits::units::Time, comms::traits::units::NanosecondsRatio>;

/// @brief Alias option, specifying field value units are "microseconds".
using UnitsMicroseconds =
    Units<comms::traits::units::Time, comms::traits::units::MicrosecondsRatio>;

/// @brief Alias option, specifying field value units are "milliseconds".
using UnitsMilliseconds =
    Units<comms::traits::units::Time, comms::traits::units::MillisecondsRatio>;

/// @brief Alias option, specifying field value units are "seconds".
using UnitsSeconds =
    Units<comms::traits::units::Time, comms::traits::units::SecondsRatio>;

/// @brief Alias option, specifying field value units are "minutes".
using UnitsMinutes =
    Units<comms::traits::units::Time, comms::traits::units::MinutesRatio>;

/// @brief Alias option, specifying field value units are "hours".
using UnitsHours =
    Units<comms::traits::units::Time, comms::traits::units::HoursRatio>;

/// @brief Alias option, specifying field value units are "days".
using UnitsDays =
    Units<comms::traits::units::Time, comms::traits::units::DaysRatio>;

/// @brief Alias option, specifying field value units are "weeks".
using UnitsWeeks =
    Units<comms::traits::units::Time, comms::traits::units::WeeksRatio>;

/// @brief Alias option, specifying field value units are "nanometers".
using UnitsNanometers =
    Units<comms::traits::units::Distance, comms::traits::units::NanometersRatio>;

/// @brief Alias option, specifying field value units are "micrometers".
using UnitsMicrometers =
    Units<comms::traits::units::Distance, comms::traits::units::MicrometersRatio>;

/// @brief Alias option, specifying field value units are "millimeters".
using UnitsMillimeters =
    Units<comms::traits::units::Distance, comms::traits::units::MillimetersRatio>;

/// @brief Alias option, specifying field value units are "centimeters".
using UnitsCentimeters =
    Units<comms::traits::units::Distance, comms::traits::units::CentimetersRatio>;

/// @brief Alias option, specifying field value units are "meters".
using UnitsMeters =
    Units<comms::traits::units::Distance, comms::traits::units::MetersRatio>;

/// @brief Alias option, specifying field value units are "kilometers".
using UnitsKilometers =
    Units<comms::traits::units::Distance, comms::traits::units::KilometersRatio>;

/// @brief Alias option, specifying field value units are "nanometers per second".
using UnitsNanometersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::NanometersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "micrometers per second".
using UnitsMicrometersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::MicrometersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "millimeters per second".
using UnitsMillimetersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::MillimetersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "centimeters per second".
using UnitsCentimetersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::CentimetersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "meters per second".
using UnitsMetersPerSecond =
    Units<comms::traits::units::Speed, comms::traits::units::MetersPerSecondRatio>;

/// @brief Alias option, specifying field value units are "kilometers per hour".
using UnitsKilometersPerHour =
    Units<comms::traits::units::Speed, comms::traits::units::KilometersPerHourRatio>;

/// @brief Alias option, specifying field value units are "hertz".
using UnitsHertz =
    Units<comms::traits::units::Frequency, comms::traits::units::HzRatio>;

/// @brief Alias option, specifying field value units are "kilohertz".
using UnitsKilohertz =
    Units<comms::traits::units::Frequency, comms::traits::units::KiloHzRatio>;

/// @brief Alias option, specifying field value units are "megahertz".
using UnitsMegahertz =
    Units<comms::traits::units::Frequency, comms::traits::units::MegaHzRatio>;

/// @brief Alias option, specifying field value units are "gigahertz".
using UnitsGigahertz =
    Units<comms::traits::units::Frequency, comms::traits::units::GigaHzRatio>;

namespace details
{

template<std::intmax_t TVal>
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
/// @tparam TValue Expected value of the reserved bits. Defaults to 0.
template<std::uintmax_t TMask, std::uintmax_t TValue = 0U>
using BitmaskReservedBits = ContentsValidator<details::BitmaskReservedBitsValidator<TMask, TValue> >;

/// @brief Alias to DefaultValueInitialiser, it sets default mode
///     to field::Optional field.
/// @tparam TVal Optional mode value is to be assigned to the field in default constructor.
template<comms::field::OptionalMode TVal>
using DefaultOptionalMode = DefaultValueInitialiser<details::DefaultOptModeInitialiser<TVal> >;


}  // namespace option

}  // namespace comms


