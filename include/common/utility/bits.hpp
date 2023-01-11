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


//Example: 
//  constexpr auto carry = 
//      [](const byte a, const byte b, const byte carry) { 
//          return bit_carry<byte, 8>(a, b, carry); 
//      };
//  constexpr auto half_carry = 
//      [](const byte a, const byte b, const byte carry) { 
//          return bit_carry<byte, 4>(a, b, carry); 
//      }; 
template<std::integral I, std::integral auto BIT>
constexpr bool bit_carry(const I a, const I b, const I c = 0) //c is carry
{
    constexpr std::uint64_t bit = (1 << BIT);
    constexpr std::uint64_t mask = bit - 1;
    return ((a & mask) - (b & mask) - (c & mask)) & bit;
}