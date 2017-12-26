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

/// @file
/// Contains definition of all the options used by the @b COMMS library

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

/// @brief No-op option, doesn't have any effect.
/// @headerfile comms/options.h
struct EmptyOption {};

/// @brief Option used to specify type of the ID.
/// @tparam T Type of the message ID.
/// @headerfile comms/options.h
template <typename T>
struct MsgIdType {};

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

/// @brief Option used to specify type of the message handler.
/// @tparam T Type of the handler.
/// @headerfile comms/options.h
template <typename T>
struct Handler {};

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

/// @brief Option used to inhibit default implementation of @b dispatchImpl()
///     in comms::MessageBase.
/// @headerfile comms/options.h
struct NoDispatchImpl {};

/// @brief Option used to specify some extra fields from transport framing.
/// @details Some fields from transport framing may influence the way on how
///     message fields get read or written. It may also have an influence on
///     how message is handled. This option is intended to provide a list
///     of such fields, bundled in @b std::tuple, to @ref comms::Message interface
///     class.
/// @tparam TFields The fields of the message bundled in std::tuple.
/// @headerfile comms/options.h
template <typename TFields>
struct ExtraTransportFields;

/// @cond SKIP_DOC
template <typename... TFields>
struct ExtraTransportFields<std::tuple<TFields...> >
{
};
/// @endcond


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

/// @brief Option that notifies comms::MessageBase about existence of
///     custom refresh functionality in derived class.
/// @headerfile comms/options.h
struct HasDoRefresh {};

/// @brief Option that notifies comms::MessageBase about existence of
///     @b doGetId() member function in derived class.
/// @headerfile comms/options.h
struct HasDoGetId {};

/// @brief Option that notifies comms::MessageBase about existence of
///     access to fields.
/// @details Can be useful when there is a chain of inheritances from
///     comms::MessageBase.
/// @headerfile comms/options.h
struct AssumeFieldsExistence {};

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
/// @headerfile comms/options.h
template<std::size_t TLen>
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
/// @tparam TSize Size of the storage area, for strings it does @b NOT include
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
/// @tparam TReadErrorStatus Error status to return when read operation fails when should not
/// @headerfile comms/options.h
template <typename TField, comms::ErrorStatus TReadErrorStatus = comms::ErrorStatus::InvalidMsgData>
struct SequenceSerLengthFieldPrefix {};

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
/// @headerfile comms/options.h
struct SequenceSizeForcingEnabled {};

/// @brief Option to enable external forcing of the collection element
///     serialisation length.
/// @details Some protocols may prefix the variable length lists with serialisation
///     length of a <b>single element</b> in addition to the number of elements
///     in the list. Usage of this option
///     enables @b forceElemLength() and @b clearElemLengthForcing() functions in
///     the comms::field::ArrayList
///     which can be used to specify the element serialisation length after it was read
///     independently. @n
///     When writing such comms::field::ArrayList field (defined with this option),
///     the call to @b forceElemLength() may be used to add padding bytes at
///     the end of each element. It may be used to force alignment of the next element.
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

/// @brief Option that forces usage of fixed size storage for sequences with fixed
///     size.
/// @details Equivalent to @ref FixedSizeStorage option, but applicable only
///     to sequence types @ref comms::field::ArrayList or @ref comms::field::String, that
///     alrady use @ref SequenceFixedSize option. Usage of this option do not
///     require knowledge of the storage area size.
/// @headerfile comms/options.h
struct SequenceFixedSizeUseFixedSizeStorage {};

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

/// @brief Force the destructor of comms::Message class to be @b non-virtual,
///     even if there are other virtual functions defined.
/// @headerfile comms/options.h
struct NoVirtualDestructor {};

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

namespace details
{

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

/// @brief Alias to @ref ValidNumValueRange.
/// @details Equivalent to @b ValidNumValueRange<TValue, TValue>
template<std::intmax_t TValue>
using ValidNumValue = ValidNumValueRange<TValue, TValue>;

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

/// @brief Alias to @ref ValidBigUnsignedNumValueRange.
/// @details Equivalent to @b ValidBigUnsignedNumValueRange<TValue, TValue>
template<std::uintmax_t TValue>
using ValidBigUnsignedNumValue = ValidBigUnsignedNumValueRange<TValue, TValue>;

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
/// @headerfile comms/options.h
template<comms::field::OptionalMode TVal>
using DefaultOptionalMode = DefaultValueInitialiser<details::DefaultOptModeInitialiser<TVal> >;

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

/// @brief Use "view" on original raw data instead of copying it.
/// @details Can be used with @ref comms::field::String and raw data @ref comms::field::ArrayList,
///     will force usage of @ref comms::util::StringView and comms::util::ArrayView
///     respectively as data storage type.
/// @note The original data must be preserved until destruction of the field
///     that uses the "view".
/// @note Incompatible with other options that contol data storage type,
///     such as @ref comms::option::CustomStorageType or @ref comms::option::FixedSizeStorage
struct OrigDataView {};

/// @brief Force field not to be serialized during read/write operations
/// @details Some protocols may define some constant values that are predefined
///     and are not present on I/O link when serialized. Sometimes it is convenient
///     to have such values abstracted away as fields, which are not actually
///     serialised. Using this option will have such effect: read/write operaitons
///     will not change the value of iterators and will report immediate success.
///     The serialisation length is always reported as 0.
struct EmptySerialization {};

/// @brief Same as @ref EmptySerialization.
/// @details Just British English spelling.
using EmptySerialisation = EmptySerialization;

}  // namespace option

}  // namespace comms


