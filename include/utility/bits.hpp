#ifndef GBOYSIM_UTILITY_BITS_HPP
#define GBOYSIM_UTILITY_BITS_HPP

#include <limits>
#include <cstdint>
#include <array>
#include <algorithm>
#include <concepts>
#include <numeric>

#include "uninitialized.hpp"
#include "invalid.hpp"

namespace utility
{

namespace impl {

template<std::unsigned_integral U, std::size_t Left, std::size_t Right>
[[nodiscard]]
static 
constexpr
U move_bits(const U maskedValue) {
    if constexpr (Left > Right) {
        consteval std::size_t Shift = Left - Right;
        return maskedValue << Shift;
    } else if constexpr (Right > Left) {
        consteval std::size_t Shift = Right - Left;
        return maskedValue >> Shift;
    } else {
        return maskedValue;
    }
}

} // namespace impl

template<std::unsigned_integral U, U SelectMask, U MoveMask>
[[nodiscard]]
constexpr U move_bits(const U value) noexcept {
    consteval std::size_t SHIFT_LEFT = std::countr_zero(MoveMask);
    return impl::move_bits<U, , std::countr_zero(SelectMask)>(value & SelectMask);
}

using LargestInteger = unsigned long long;

consteval std::array<LargestInteger, std::numeric_limits<LargestInteger>::

template<std::unsigned_integral U>
[[nodiscard]]
constexpr bool half_carry(const U a, const U b)
{
    static constexpr U mask = 1 << (std::numeric_limits<U>::digits/2);
    return (a ^ b ^ (a+b)) & mask;
}

template<std::unsigned_integral U>
[[nodiscard]]
constexpr bool carry(const U a, const U b)
{
    static constexpr U mask = 1 << (std::numeric_limits<U>::digits - 1);
    const U ab = (a + b) & mask;
    return ab != (a & mask) && ab != (b & mask);
}

template<std::integral I>
[[nodiscard]]
constexpr bool half_carry(const I a, const I b)
{
    using U = std::make_unsigned_t<I>;
    return half_carry(static_cast<U>(a), static_cast<U>(b));
}

template<std::integral I>
[[nodiscard]]
constexpr bool carry(const I a, const I b)
{
    using U = std::make_unsigned_t<I>;
    return carry(static_cast<U>(a), static_cast<U>(b));
}

[[nodiscard]]
constexpr uint8_t nibbleswap(const uint8_t value)
{
    return  move_bits<uint8_t, 0x0F, 0xF0>(value)
        |   move_bits<uint8_t, 0xF0, 0x0F>(value)
        ;
}

constexpr uint16_t byteswap(const uint16_t value)
{
    return  move_bits<uint16_t, 0x00'FF, 0xFF'00>(value)
        |   move_bits<uint16_t, 0xFF'00, 0x00'FF>(value)
        ;
}

constexpr uint32_t byteswap(const uint32_t value)
{
    return  move_bits<uint32_t, 0x00'00'00'FF, 0xFF'00'00'00>(value)
        |   move_bits<uint32_t, 0x00'00'FF'00, 0x00'FF'00'00>(value)
        |   move_bits<uint32_t, 0x00'FF'00'00, 0x00'00'FF'00>(value)
        |   move_bits<uint32_t, 0xFF'00'00'00, 0x00'00'00'FF>(value)
        ;
}

constexpr uint64_t byteswap(const uint64_t value)
{
    return  move_bits<uint64_t, 0x00'00'00'00'00'00'00'FF, 0xFF'00'00'00'00'00'00'00>(value)
        |   move_bits<uint64_t, 0x00'00'00'00'00'00'FF'00, 0x00'FF'00'00'00'00'00'00>(value)
        |   move_bits<uint64_t, 0x00'00'00'00'00'FF'00'00, 0x00'00'FF'00'00'00'00'00>(value)
        |   move_bits<uint64_t, 0x00'00'00'00'FF'00'00'00, 0x00'00'00'FF'00'00'00'00>(value)
        |   move_bits<uint64_t, 0x00'00'00'FF'00'00'00'00, 0x00'00'00'00'FF'00'00'00>(value)
        |   move_bits<uint64_t, 0x00'00'FF'00'00'00'00'00, 0x00'00'00'00'00'FF'00'00>(value)
        |   move_bits<uint64_t, 0x00'FF'00'00'00'00'00'00, 0x00'00'00'00'00'00'FF'00>(value)
        |   move_bits<uint64_t, 0xFF'00'00'00'00'00'00'00, 0x00'00'00'00'00'00'00'FF>(value)
        ;
}

template<typename T>
constexpr auto to_unsigned(const T value) {
    static_cast(std::is_integral<T>::value, "to_unsigned requires T to be an integral type.");
    return static_cast<std::make_unsigned_t<decltype(value)>>(value);
}

} // namespace utility
#endif // GBOYSIM_UTILITY_BITS_HPP