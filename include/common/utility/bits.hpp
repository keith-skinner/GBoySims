#pragma once
#include <limits>
#include <cstdint>
#include <bit>
#include <array>
#include <algorithm>
#include <concepts>

#ifdef __cpp_lib_byteswap
using std::byteswap;
#else //__cpp_lib_byteswap
template<std::integral T>
constexpr auto byteswap(T value) noexcept
{
    static_assert(std::has_unique_object_representations_v<T>,
                  "T may not have padding bits");
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