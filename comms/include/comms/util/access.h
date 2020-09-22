//
// Copyright 2014 - 2020 (C). Alex Robenko. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <limits>
#include <iterator>

#include "comms/util/type_traits.h"
#include "comms/details/tag.h"

namespace comms
{

namespace util
{

namespace traits
{

namespace endian
{

/// @brief Empty class used in traits to indicate Big Endian.
struct Big {};

/// @brief Empty class used in traits to indicate Little Endian.
struct Little {};

}  // namespace endian

}  // namespace traits

namespace details
{

template <typename T>
using AccessSelectTypeItself = typename std::decay<T>::type;

template <typename T>
using AccessSelectIntType = 
    typename comms::util::Conditional<
        std::is_signed<T>::value
    >::template Type<int, unsigned>;

template <typename T>
using AccessOptimisedValueType = 
    typename comms::util::LazyShallowConditional<
        sizeof(T) >= sizeof(int)
    >::template Type<
        AccessSelectTypeItself,
        AccessSelectIntType,
        T
    >;

template <typename TUnsignedByteType, typename T>
typename std::decay<T>::type signExtCommon(T value, std::size_t size)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_unsigned<ValueType>::value, "Type T must be unsigned");
    static_assert(std::is_unsigned<TUnsignedByteType>::value,
                                "Type TUnsignedByteType must be unsigned");

    static const std::size_t BinDigits =
        std::numeric_limits<TUnsignedByteType>::digits;
    static_assert(BinDigits % 8 == 0, "Byte size assumption is not valid");

    ValueType mask =
        (static_cast<ValueType>(1) << ((size * BinDigits) - 1));
    if (value & mask) {
        return value | (~((mask << 1) - 1));
    }
    return value;
}

template <typename...>
class SignExt
{
    template <typename... TParams>
    using FullSize = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using PartialSize = comms::details::tag::Tag2<>;    

public:
    template <std::size_t TSize, typename TByteType, typename T>
    static typename std::decay<T>::type value(T val)
    {
        using ValueType = typename std::decay<T>::type;
        using Tag = 
            typename comms::util::LazyShallowConditional<
                sizeof(ValueType) == TSize
            >::template Type<
                FullSize,
                PartialSize
            >;

        return valueInternal<TSize, TByteType>(val, Tag());
    }

private:

    template <std::size_t TSize, typename TByteType, typename T, typename... TParams>
    static typename std::decay<T>::type valueInternal(T val, FullSize<TParams...>)
    {
        return val;
    }

    template <std::size_t TSize, typename TByteType, typename T, typename... TParams>
    static typename std::decay<T>::type valueInternal(T val, PartialSize<TParams...>)
    {
        using ValueType = typename std::decay<T>::type;
        using UnsignedValueType = typename std::make_unsigned<ValueType>::type;
        static_assert(std::is_integral<ValueType>::value, "T must be integer type");
        using UnsignedByteType = typename std::make_unsigned<TByteType>::type;

        auto castedValue = static_cast<UnsignedValueType>(val);
        return static_cast<ValueType>(
            signExtCommon<UnsignedByteType>(castedValue, TSize));
    }
};

template <typename TIter>
using AccessIteratorByteType = typename std::iterator_traits<typename std::decay<TIter>::type>::value_type;

template <typename TIter>
struct AccessContainerByteTypeDetector
{
    using Type = AccessIteratorByteType<TIter>;
};

template <typename TContainer>
struct AccessContainerByteTypeDetector<std::back_insert_iterator<TContainer> >
{
    using Type = typename TContainer::value_type;
};

template <typename TContainer>
struct AccessContainerByteTypeDetector<std::insert_iterator<TContainer> >
{
    using Type = typename TContainer::value_type;
};

template <typename TContainer>
struct AccessContainerByteTypeDetector<std::front_insert_iterator<TContainer> >
{
    using Type = typename TContainer::value_type;
};

template <typename TIter>
using AccessContainerByteType = 
    typename AccessContainerByteTypeDetector<typename std::decay<TIter>::type>::Type;

template <typename TIter>
using AccessByteType = 
    typename comms::util::LazyShallowConditional<
        std::is_void<AccessIteratorByteType<TIter> >::value
    >::template Type<
        AccessContainerByteType,
        AccessIteratorByteType,
        TIter
    >;

template <typename T, typename TIter>
void writeBigUnsigned(T value, std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_unsigned<ValueType>::value, "T type must be unsigned");
    static_assert(std::is_integral<ValueType>::value, "T must be integral type");

    using ByteType = AccessByteType<TIter>;
    static_assert(!std::is_void<ByteType>::value, "Invalid byte type");
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;
    static const std::size_t BinDigits =
        std::numeric_limits<UnsignedByteType>::digits;
    static_assert(BinDigits % 8 == 0, "Byte size assumption is not valid");

    std::size_t remainingSize = size;
    while (remainingSize > 0) {
        std::size_t remaingShift = ((remainingSize - 1) * BinDigits);
        auto byte = static_cast<ByteType>(value >> remaingShift);
        *iter = byte;
        ++iter;
        --remainingSize;
    }
}

template <typename T, typename TIter>
void writeLittleUnsigned(T value, std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_integral<ValueType>::value, "T must be integral type");
    static_assert(std::is_unsigned<ValueType>::value, "T type must be unsigned");

    using ByteType = AccessByteType<TIter>;
    static_assert(!std::is_void<ByteType>::value, "Invalid byte type");
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;
    static const std::size_t BinDigits =
        std::numeric_limits<UnsignedByteType>::digits;
    static_assert(BinDigits % 8 == 0, "Byte size assumption is not valid");

    std::size_t remainingSize = size;
    while (remainingSize > 0) {
        std::size_t remaingShift = ((size - remainingSize) * BinDigits);

        auto byte = static_cast<ByteType>(value >> remaingShift);
        *iter = byte;
        ++iter;
        --remainingSize;
    }
}

template <typename TEndian>
struct WriteUnsignedFuncWrapper;

template <>
struct WriteUnsignedFuncWrapper<traits::endian::Big>
{
    template <typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        writeBigUnsigned(value, size, iter);
    }
};

template <>
struct WriteUnsignedFuncWrapper<traits::endian::Little>
{
    template <typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        writeLittleUnsigned(value, size, iter);
    }
};

template <typename TEndian, typename T, typename TIter>
void write(T value, std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_integral<ValueType>::value, "T must be integral type");
    using UnsignedType = typename std::make_unsigned<ValueType>::type;
    UnsignedType unsignedValue = static_cast<UnsignedType>(value);
    WriteUnsignedFuncWrapper<TEndian>::write(unsignedValue, size, iter);
}

template <typename TEndian, typename T, typename TIter>
void writeRandomAccess(T value, std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_integral<ValueType>::value, "T must be integral type");
    static_assert(
        std::is_same<
            typename std::iterator_traits<TIter>::iterator_category,
            std::random_access_iterator_tag
        >::value,
        "TIter must be random access iterator");

    using ByteType = AccessByteType<TIter>;
    static_assert(!std::is_void<ByteType>::value, "Invalid byte type");
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;
    static_assert(!std::is_const<UnsignedByteType>::value, "Value must be updatable");

    auto startPtr = reinterpret_cast<UnsignedByteType*>(&(*iter));
    auto endPtr = startPtr;
    write<TEndian>(value, size, endPtr);
    iter += (endPtr - startPtr);
}

template <typename...>
class WriteHelper
{
    template <typename... TParams>
    using RandomAccessTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using RegularTag = comms::details::tag::Tag2<>;

    template <typename TIter>
    using RandomAccessOrPointerTag = 
        typename comms::util::LazyShallowConditional<
            std::is_pointer<TIter>::value &&
            std::is_unsigned<AccessByteType<TIter> >::value        
        >::template Type<
            RegularTag,
            RandomAccessTag
        >;       

    template <typename TIter>
    using Tag = 
        typename comms::util::LazyShallowConditional<
            std::is_same<
                typename std::iterator_traits<TIter>::iterator_category,
                std::random_access_iterator_tag
            >::value
        >::template Type<
            RandomAccessOrPointerTag,
            RegularTag,
            TIter
        >;

    template <typename TEndian, typename T, typename TIter, typename... TParams>
    static void writeInternal(T value, std::size_t size, TIter& iter, RandomAccessTag<TParams...>)
    {
        writeRandomAccess<TEndian>(value, size, iter);
    }

    template <typename TEndian, typename T, typename TIter, typename... TParams>
    static void writeInternal(T value, std::size_t size, TIter& iter, RegularTag<TParams...>)
    {
        details::write<TEndian>(value, size, iter);
    }        

public:
    template <typename TEndian, typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        using ValueType = typename std::decay<T>::type;
        using AccessOptimisedValueType = details::AccessOptimisedValueType<ValueType>;

        return writeInternal<TEndian>(static_cast<AccessOptimisedValueType>(value), size, iter, Tag<TIter>());
    }    
};

template <typename T, typename TIter>
T readBigUnsigned(std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_integral<ValueType>::value, "T must be integral type");
    static_assert(std::is_unsigned<ValueType>::value, "T type must be unsigned");

    using ByteType = AccessByteType<TIter>;
    static_assert(!std::is_void<ByteType>::value, "Invalid byte type");
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;
    static const std::size_t BinDigits =
        std::numeric_limits<UnsignedByteType>::digits;
    static_assert(BinDigits % 8 == 0, "Byte size assumption is not valid");

    ValueType value = 0;
    std::size_t remainingSize = size;
    while (remainingSize > 0) {
        auto byte = *iter;
        value <<= BinDigits;
        value |= static_cast<decltype(value)>(static_cast<UnsignedByteType>(byte));
        ++iter;
        --remainingSize;
    }
    return value;
}

template <typename T, typename TIter>
T readLittleUnsigned(std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_integral<ValueType>::value, "T must be integral type");
    static_assert(std::is_unsigned<ValueType>::value, "T type must be unsigned");

    using ByteType = AccessByteType<TIter>;
    static_assert(!std::is_void<ByteType>::value, "Invalid byte type");
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;
    static const std::size_t BinDigits =
        std::numeric_limits<UnsignedByteType>::digits;
    static_assert(BinDigits % 8 == 0, "Byte size assumption is not valid");

    ValueType value = 0;
    std::size_t remainingSize = size;
    while (remainingSize > 0) {
        auto byte = *iter;
        value |= static_cast<ValueType>(static_cast<UnsignedByteType>(byte)) <<
            ((size - remainingSize) * BinDigits);
        ++iter;
        --remainingSize;
    }

    return static_cast<T>(value);
}

template <typename TEndian>
struct ReadUnsignedFuncWrapper;

template <>
struct ReadUnsignedFuncWrapper<traits::endian::Big>
{
    template <typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        return readBigUnsigned<T>(size, iter);
    }
};

template <>
struct ReadUnsignedFuncWrapper<traits::endian::Little>
{
    template <typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        return readLittleUnsigned<T>(size, iter);
    }
};

template <typename TEndian, typename T, typename TIter>
T read(std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_integral<ValueType>::value, "T must be integral type");

    using UnsignedType = typename std::make_unsigned<ValueType>::type;
    auto value = ReadUnsignedFuncWrapper<TEndian>::template read<UnsignedType>(size, iter);
    return static_cast<T>(static_cast<ValueType>(value));
}

template <typename TEndian, typename T, typename TIter>
T readFromPointerToSigned(std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;

    static_assert(std::is_integral<ValueType>::value, "T must be integral type");
    static_assert(
        std::is_same<
            typename std::iterator_traits<TIter>::iterator_category,
            std::random_access_iterator_tag
        >::value,
        "TIter must be random access iterator");

    using ByteType = AccessByteType<TIter>;
    static_assert(!std::is_void<ByteType>::value, "Invalid byte type");
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;

    auto startPtr = reinterpret_cast<const UnsignedByteType*>(&(*iter));
    auto endPtr = startPtr;
    auto value = details::read<TEndian, ValueType>(size, endPtr);
    iter += (endPtr - startPtr);
    return static_cast<T>(static_cast<ValueType>(value));
}

template <typename...>
class ReadHelper
{
    template <typename... TParams>
    using PointerToSignedTag = comms::details::tag::Tag1<>;

    template <typename... TParams>
    using OtherTag = comms::details::tag::Tag2<>;

    template <typename TIter>
    using PointerCheckTag = 
        typename comms::util::LazyShallowConditional<
            std::is_const<AccessByteType<TIter> >::value &&
            std::is_unsigned<AccessByteType<TIter> >::value
        >::template Type<
            OtherTag,
            PointerToSignedTag
        >;

    template <typename TIter>
    using Tag = 
        typename comms::util::LazyShallowConditional<
            std::is_pointer<TIter>::value
        >::template Type<
            PointerCheckTag,
            OtherTag,
            TIter
        >;

    template <typename TEndian, typename T, typename TIter, typename... TParams>
    static T readInternal(std::size_t size, TIter& iter, PointerToSignedTag<TParams...>)
    {
        return readFromPointerToSigned<TEndian, T>(size, iter);
    }

    template <typename TEndian, typename T, typename TIter, typename... TParams>
    static T readInternal(std::size_t size, TIter& iter, OtherTag<TParams...>)
    {
        return details::read<TEndian, T>(size, iter);
    }

public:
    template <typename TEndian, typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        using ValueType = typename std::decay<T>::type;
        using AccessOptimisedValueType = details::AccessOptimisedValueType<ValueType>;
        return
            static_cast<ValueType>(
                readInternal<TEndian, AccessOptimisedValueType>(size, iter, Tag<TIter>()));
    }

    template <typename TEndian, typename T, std::size_t TSize, typename TIter>
    static T read(TIter& iter)
    {
        using ValueType = typename std::decay<T>::type;
        using ByteType = details::AccessByteType<TIter>;
        static_assert(!std::is_void<ByteType>::value, "Invalid byte type");
        static_assert(TSize <= sizeof(ValueType), "Precondition failure");
        auto retval = read<TEndian, ValueType>(TSize, iter);
        if (std::is_signed<ValueType>::value) {
            retval = details::SignExt<>::template value<TSize, ByteType>(retval);
        }
        return static_cast<T>(retval);
    }    
};

}  // namespace details

/// @brief Write part of integral value into the output area using big
///     endian notation.
/// @tparam TSize Number of bytes to write.
/// @param[in] value Integral type value to be written.
/// @param[in, out] iter Output iterator.
/// @pre TSize <= sizeof(T).
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least TSize times.
/// @post The iterator is advanced.
template <std::size_t TSize, typename T, typename TIter>
void writeBig(T value, TIter& iter)
{
    details::WriteHelper<>::template write<traits::endian::Big>(value, TSize, iter);
}

/// @brief Write part of integral value into the output area using big
///     endian notation.
/// @param[in] value Integral type value to be written.
/// @param[in] size Number of bytes to write.
/// @param[in, out] iter Output iterator.
/// @pre TSize <= sizeof(T).
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least TSize times.
/// @post The iterator is advanced.
template <typename T, typename TIter>
void writeBig(T value, std::size_t size, TIter& iter)
{
    details::WriteHelper<>::template write<traits::endian::Big>(value, size, iter);
}

/// @brief Write integral value into the output area using big
///     endian notation.
/// @param[in] value Integral type value to be written.
/// @param[in, out] iter Output iterator.
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least sizeof(T) times.
/// @post The iterator is advanced.
template <typename T, typename TIter>
void writeBig(T value, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    writeBig<sizeof(ValueType)>(static_cast<ValueType>(value), iter);
}

/// @brief Read part of integral value from the input area using big
///     endian notation.
/// @tparam T Type to read.
/// @tparam TSize Number of bytes to read.
/// @param[in, out] iter Input iterator.
/// @return Read value
/// @pre TSize <= sizeof(T).
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least TSize times.
/// @post The iterator is advanced.
template <typename T, std::size_t TSize, typename TIter>
T readBig(TIter& iter)
{
    return details::ReadHelper<>::template read<traits::endian::Big, T, TSize>(iter);
}

/// @brief Read integral value from the input area using big
///     endian notation.
/// @tparam T Type to read.
/// @param[in, out] iter Input iterator.
/// @return Read value
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least sizeof(T) times.
/// @post The iterator is advanced.
template <typename T, typename TIter>
T readBig(TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    return static_cast<T>(readBig<ValueType, sizeof(ValueType)>(iter));
}

/// @brief Write part of integral value into the output area using little
///     endian notation.
/// @tparam TSize Number of bytes to write.
/// @param[in] value Integral type value to be written.
/// @param[in, out] iter Output iterator.
/// @pre TSize <= sizeof(T).
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least TSize times.
/// @post The iterator is advanced.
template <std::size_t TSize, typename T, typename TIter>
void writeLittle(T value, TIter& iter)
{
    details::WriteHelper<>::template write<traits::endian::Little>(value, TSize, iter);
}

/// @brief Write part of integral value into the output area using little
///     endian notation.
/// @param[in] value Integral type value to be written.
/// @param[in] size Number of bytes to write.
/// @param[in, out] iter Output iterator.
/// @pre TSize <= sizeof(T).
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least TSize times.
/// @post The iterator is advanced.
template <typename T, typename TIter>
void writeLittle(T value, std::size_t size, TIter& iter)
{
    details::WriteHelper<>::template write<traits::endian::Little>(value, size, iter);
}

/// @brief Write integral value into the output area using big
///     endian notation.
/// @param[in] value Integral type value to be written.
/// @param[in, out] iter Output iterator.
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least sizeof(T) times.
/// @post The iterator is advanced.
template <typename T, typename TIter>
void writeLittle(T value, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    writeLittle<sizeof(ValueType)>(static_cast<ValueType>(value), iter);
}

/// @brief Read part of integral value from the input area using little
///     endian notation.
/// @tparam T Type to read.
/// @tparam TSize Number of bytes to read.
/// @param[in, out] iter Input iterator.
/// @return Read value
/// @pre TSize <= sizeof(T).
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least TSize times.
/// @post The iterator is advanced.
template <typename T, std::size_t TSize, typename TIter>
T readLittle(TIter& iter)
{
    return details::ReadHelper<>::template read<traits::endian::Little, T, TSize>(iter);
}

/// @brief Read integral value from the input area using little
///     endian notation.
/// @tparam T Type to read.
/// @param[in, out] iter Input iterator.
/// @return Read value
/// @pre The iterator must be valid and can be successfully dereferenced
///      and incremented at least sizeof(T) times.
/// @post The iterator is advanced.
template <typename T, typename TIter>
T readLittle(TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    return static_cast<T>(readLittle<ValueType, sizeof(ValueType)>(iter));
}

/// @brief Same as writeBig<T, TIter>()
template <typename T, typename TIter>
void writeData(
    T value,
    TIter& iter,
    const traits::endian::Big& endian)
{
    static_cast<void>(endian);
    writeBig(value, iter);
}

/// @brief Same as writeBig<TSize, T, TIter>()
template <std::size_t TSize, typename T, typename TIter>
void writeData(
    T value,
    TIter& iter,
    const traits::endian::Big& endian)
{
    static_cast<void>(endian);
    writeBig<TSize>(value, iter);
}

/// @brief Same as writeBig<T, TIter>()
template <typename T, typename TIter>
void writeData(
    T value,
    std::size_t size,
    TIter& iter,
    const traits::endian::Big& endian)
{
    static_cast<void>(endian);
    writeBig(value, size, iter);
}

/// @brief Same as writeLittle<T, TIter>()
template <typename T, typename TIter>
void writeData(
    T value,
    TIter& iter,
    const traits::endian::Little& endian)
{
    static_cast<void>(endian);
    writeLittle(value, iter);
}

/// @brief Same as writeLittle<TSize, T, TIter>()
template <std::size_t TSize, typename T, typename TIter>
void writeData(
    T value,
    TIter& iter,
    const traits::endian::Little& endian)
{
    static_cast<void>(endian);
    return writeLittle<TSize>(value, iter);
}

/// @brief Same as writeLittle<T, TIter>()
template <typename T, typename TIter>
void writeData(
    T value,
    std::size_t size, 
    TIter& iter,
    const traits::endian::Little& endian)
{
    static_cast<void>(endian);
    writeLittle(value, size, iter);
}

/// @brief Same as readBig<T, TIter>()
template <typename T, typename TIter>
T readData(TIter& iter, const traits::endian::Big& endian)
{
    static_cast<void>(endian);
    return readBig<T>(iter);
}

/// @brief Same as readBig<T, TSize, TIter>()
template <typename T, std::size_t TSize, typename TIter>
T readData(TIter& iter, const traits::endian::Big& endian)
{
    static_cast<void>(endian);
    return readBig<T, TSize>(iter);
}

/// @brief Same as readLittle<T, TIter>()
template <typename T, typename TIter>
T readData(TIter& iter, const traits::endian::Little& endian)
{
    static_cast<void>(endian);
    return readLittle<T>(iter);
}

/// @brief Same as readData<T, TSize, TIter>()
template <typename T, std::size_t TSize, typename TIter>
T readData(TIter& iter, const traits::endian::Little& endian)
{
    static_cast<void>(endian);
    return readLittle<T, TSize>(iter);
}


}  // namespace util

}  // namespace comms
