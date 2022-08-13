#pragma once
#include <limits>
#include <cstdint>
#include <bit>
#include <array>
#include <algorithm>
#include <concepts>

template<size_t B> struct bits_to_bytes { static constexpr size_t value = B / 8; };
template<size_t B> static constexpr size_t bits_to_bytes_v = bits_to_bytes<B>::value;

template<size_t V> struct bytes_to_int;
template<> struct bytes_to_int<1U> { using value = int8_t; };
template<> struct bytes_to_int<2U> { using value = int16_t; };
template<> struct bytes_to_int<3U> { using value = int32_t; };
template<> struct bytes_to_int<4U> { using value = int32_t; };
template<> struct bytes_to_int<5U> { using value = int64_t; };
template<> struct bytes_to_int<6U> { using value = int64_t; };
template<> struct bytes_to_int<7U> { using value = int64_t; };
template<> struct bytes_to_int<8U> { using value = int64_t; };
template<size_t V> using bytes_to_int_t = typename bytes_to_int<V>::value;

template<size_t V> struct bytes_to_uint;
template<> struct bytes_to_uint<1U> { using value = uint8_t; };
template<> struct bytes_to_uint<2U> { using value = uint16_t; };
template<> struct bytes_to_uint<3U> { using value = uint32_t; };
template<> struct bytes_to_uint<4U> { using value = uint32_t; };
template<> struct bytes_to_uint<5U> { using value = uint64_t; };
template<> struct bytes_to_uint<6U> { using value = uint64_t; };
template<> struct bytes_to_uint<7U> { using value = uint64_t; };
template<> struct bytes_to_uint<8U> { using value = uint64_t; };
template<size_t V> using bytes_to_uint_t = typename bytes_to_uint<V>::value;

#ifdef __cpp_lib_byteswap
using std::byteswap;
#else //__cpp_lib_byteswap
template<std::integral T>
constexpr T byteswap(T value) noexcept
{
    static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::ranges::reverse(value_representation);
    return std::bit_cast<T>(value_representation);
}
#endif // __cpp_lib_byteswap

constexpr uint8_t nibbleswap(const uint8_t value)
{
    return ((value & 0x0F) << 4)
         | ((value & 0xF0) >> 4); 
}

constexpr bool is_native_endian(std::endian v)
{
    return std::endian::native == v;
}

constexpr auto to_little_endian(const std::unsigned_integral auto v) -> decltype(v)
{
    if (!is_native_endian(std::endian::little))
        return byteswap(v);
    return v;
}

constexpr auto to_native_endian(const std::unsigned_integral auto v) -> decltype(v)
{
    return to_little_endian(v);
}

constexpr auto to_little_endian(const uint8_t b0, const uint8_t b1) -> uint16_t
{
    return static_cast<uint16_t>(b0) | (static_cast<uint16_t>(b1) << 8);
}

constexpr auto to_native_endian(const uint8_t b0, const uint8_t b1) -> uint16_t
{
    return to_native_endian(to_little_endian(b0, b1));
}

constexpr bool carry(std::unsigned_integral auto a, std::unsigned_integral auto b)
{
    static_assert(std::is_same_v<decltype(a), decltype(b)>, "a and b need to be the same type.");
    using T = decltype(a);
    constexpr T check_bit = (std::numeric_limits<T>::max() ^ (std::numeric_limits<T>::max() >> 1));
    const bool alb = a & check_bit;
    const bool blb = b & check_bit;
    const bool ablb = (a + b) & check_bit;
    return (alb && blb && !ablb) || (!alb && !blb && ablb);
}

constexpr bool half_carry(const std::unsigned_integral auto a, const std::unsigned_integral auto b)
{
    static_assert(std::is_same_v<decltype(a), decltype(b)>, "a and b need to be the same type.");
    using T = decltype(a);
    constexpr T check_bit = (std::numeric_limits<T>::max() ^ (std::numeric_limits<T>::max() >> 1)) >> 4;
    const bool alb = a & check_bit;
    const bool blb = b & check_bit;
    const bool ablb = (a + b) & check_bit;
    return (alb && blb && !ablb) || (!alb && !blb && ablb);
}

constexpr bool half_carry(const std::uint16_t a, const std::uint16_t b)
{
    return (((a & 0x0FFF) + (b & 0x0FFF)) & 0x1000) == 0x1000;
}