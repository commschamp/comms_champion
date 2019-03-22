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

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <limits>
#include <iterator>

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

template <typename T, bool TMakeIntSize>
struct TypeOptimiser;

template <typename T>
struct TypeOptimiser<T, false>
{
    using Type = typename std::decay<T>::type;
};

template <typename T>
struct TypeOptimiser<T, true>
{
    using Type = typename std::conditional<std::is_signed<T>::value, int, unsigned>::type;
};

template <typename T>
using OptimisedValueType = typename TypeOptimiser<T, sizeof(T) < sizeof(int)>::Type;

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

template <typename T, std::size_t TSize, typename TByteType>
class SignExt
{
    struct FullSize {};
    struct PartialSize {};
public:
    using ValueType = typename std::decay<T>::type;

    static ValueType value(T val)
    {
        using Tag = typename std::conditional<
            sizeof(ValueType) == TSize,
            FullSize,
            PartialSize
        >::type;

        return valueInternal(val, Tag());
    }

private:

    static ValueType valueInternal(T val, FullSize)
    {
        return val;
    }

    static ValueType valueInternal(T val, PartialSize)
    {
        using UnsignedValueType = typename std::make_unsigned<ValueType>::type;
        static_assert(std::is_integral<ValueType>::value, "T must be integer type");
        using UnsignedByteType = typename std::make_unsigned<TByteType>::type;

        auto castedValue = static_cast<UnsignedValueType>(val);
        return static_cast<ValueType>(
            signExtCommon<UnsignedByteType>(castedValue, TSize));
    }
};

template <typename TIter, bool TIsPointer>
struct ByteTypeRetriever;

template <typename TIter>
struct ByteTypeRetriever<TIter, true>
{
    using Type = typename std::decay<decltype(*(TIter()))>::type;
};

template <typename TIter>
struct ByteTypeRetriever<TIter, false>
{
    using DecayedIter = typename std::decay<TIter>::type;
    using Type = typename DecayedIter::value_type;
};

template <typename TContainer>
struct ByteTypeRetriever<std::back_insert_iterator<TContainer>, false>
{
    using DecayedContainer = typename std::decay<TContainer>::type;
    using Type = typename DecayedContainer::value_type;
};

template <typename TContainer>
struct ByteTypeRetriever<std::front_insert_iterator<TContainer>, false>
{
    using DecayedContainer = typename std::decay<TContainer>::type;
    using Type = typename DecayedContainer::value_type;
};

template <typename TIter>
using ByteType = typename ByteTypeRetriever<TIter, std::is_pointer<TIter>::value>::Type;

template <typename T, typename TIter>
void writeBigUnsigned(T value, std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_unsigned<ValueType>::value, "T type must be unsigned");
    static_assert(std::is_integral<ValueType>::value, "T must be integral type");

    using ByteType = ByteType<TIter>;
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

    using ByteType = ByteType<TIter>;
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

    using ByteType = ByteType<TIter>;
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;
    static_assert(!std::is_const<UnsignedByteType>::value, "Value must be updatable");

    auto startPtr = reinterpret_cast<UnsignedByteType*>(&(*iter));
    auto endPtr = startPtr;
    write<TEndian>(value, size, endPtr);
    iter += (endPtr - startPtr);
}

template <typename TEndian, bool TIsPointerToUnsigned>
struct WriteRandomAccessHelper;

template <typename TEndian>
struct WriteRandomAccessHelper<TEndian, false>
{
    template <typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        writeRandomAccess<TEndian>(value, size, iter);
    }
};

template <typename TEndian>
struct WriteRandomAccessHelper<TEndian, true>
{
    template <typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        details::write<TEndian>(value, size, iter);
    }
};

template <typename TEndian, bool TIsRandomAccess>
struct WriteHelper;

template <typename TEndian>
struct WriteHelper<TEndian, false>
{
    template <typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        details::write<TEndian>(value, size, iter);
    }
};

template <typename TEndian>
struct WriteHelper<TEndian, true>
{
    template <typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        using ByteType = ByteType<TIter>;
        static const bool IsPointerToUnsigned =
            std::is_pointer<TIter>::value &&
            std::is_unsigned<ByteType>::value;
        return WriteRandomAccessHelper<TEndian, IsPointerToUnsigned>::write(value, size, iter);
    }
};

template <typename T, typename TIter>
T readBigUnsigned(std::size_t size, TIter& iter)
{
    using ValueType = typename std::decay<T>::type;
    static_assert(std::is_integral<ValueType>::value, "T must be integral type");
    static_assert(std::is_unsigned<ValueType>::value, "T type must be unsigned");

    using ByteType = ByteType<TIter>;
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

    using ByteType = ByteType<TIter>;
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

    using ByteType = ByteType<TIter>;
    using UnsignedByteType = typename std::make_unsigned<ByteType>::type;

    auto startPtr = reinterpret_cast<const UnsignedByteType*>(&(*iter));
    auto endPtr = startPtr;
    auto value = details::read<TEndian, ValueType>(size, endPtr);
    iter += (endPtr - startPtr);
    return static_cast<T>(static_cast<ValueType>(value));
}

template <typename TEndian, bool TIsPointer, bool TIsUnsignedConst>
struct ReadRandomAccessHelper;

template <typename TEndian>
struct ReadRandomAccessHelper<TEndian, true, false>
{
    template <typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        return readFromPointerToSigned<TEndian, T>(size, iter);
    }
};

template <typename TEndian>
struct ReadRandomAccessHelper<TEndian, true, true>
{
    template <typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        return details::read<TEndian, T>(size, iter);
    }
};

template <typename TEndian, bool TIsUnsignedConst>
struct ReadRandomAccessHelper<TEndian, false, TIsUnsignedConst>
{
    template <typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        return details::read<TEndian, T>(size, iter);
    }
};

template <typename TEndian, bool TIsRandomAccess>
struct ReadHelper;

template <typename TEndian>
struct ReadHelper<TEndian, false>
{
    template <typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        return details::read<TEndian, T>(size, iter);
    }
};

template <typename TEndian>
struct ReadHelper<TEndian, true>
{
    template <typename T, typename TIter>
    static T read(std::size_t size, TIter& iter)
    {
        using ByteType = ByteType<TIter>;
        static const bool IsPointer =
            std::is_pointer<TIter>::value;

        static const bool IsUnsignedConstData =
            std::is_const<ByteType>::value &&
            std::is_unsigned<ByteType>::value;
        return ReadRandomAccessHelper<TEndian, IsPointer, IsUnsignedConstData>::template read<T>(size, iter);
    }
};

template <template <typename, bool> class THelper>
struct Writer
{
    template <typename TEndian, std::size_t TSize, typename T, typename TIter>
    static void write(T value, TIter& iter)
    {
        using ValueType = typename std::decay<T>::type;
        using OptimisedValueType = details::OptimisedValueType<ValueType>;

        static_assert(TSize <= sizeof(ValueType), "Precondition failure");
        static const bool IsRandomAccess =
            std::is_same<
                typename std::iterator_traits<TIter>::iterator_category,
                std::random_access_iterator_tag
            >::value;
        THelper<TEndian, IsRandomAccess>::write(
                            static_cast<OptimisedValueType>(value), TSize, iter);
    }


    template <typename TEndian, typename T, typename TIter>
    static void write(T value, std::size_t size, TIter& iter)
    {
        using ValueType = typename std::decay<T>::type;
        using OptimisedValueType = details::OptimisedValueType<ValueType>;

        //GASSERT(size <= sizeof(ValueType));
        static const bool IsRandomAccess =
            std::is_same<
                typename std::iterator_traits<TIter>::iterator_category,
                std::random_access_iterator_tag
            >::value;
        THelper<TEndian, IsRandomAccess>::write(
                            static_cast<OptimisedValueType>(value), size, iter);
    }
};

template <template <typename, bool> class THelper>
struct Reader
{
    template <typename TEndian, typename T, std::size_t TSize, typename TIter>
    static T read(TIter& iter)
    {
        using ValueType = typename std::decay<T>::type;
        using OptimisedValueType = details::OptimisedValueType<ValueType>;
        using ByteType = details::ByteType<TIter>;

        static_assert(TSize <= sizeof(ValueType), "Precondition failure");
        static const bool IsRandomAccess =
            std::is_same<
                typename std::iterator_traits<TIter>::iterator_category,
                std::random_access_iterator_tag
            >::value;
        auto retval =
            static_cast<ValueType>(
                THelper<TEndian, IsRandomAccess>::template read<OptimisedValueType>(TSize, iter));

        if (std::is_signed<ValueType>::value) {
            retval = details::SignExt<decltype(retval), TSize, ByteType>::value(retval);
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
    details::Writer<details::WriteHelper>::template write<traits::endian::Big, TSize>(value, iter);
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
    details::Writer<details::WriteHelper>::template write<traits::endian::Big>(value, size, iter);
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
    return details::Reader<details::ReadHelper>::template read<traits::endian::Big, T, TSize>(iter);
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
    details::Writer<details::WriteHelper>::template write<traits::endian::Little, TSize>(value, iter);
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
    details::Writer<details::WriteHelper>::template write<traits::endian::Little>(value, size, iter);
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
    return details::Reader<details::ReadHelper>::template read<traits::endian::Little, T, TSize>(iter);
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
