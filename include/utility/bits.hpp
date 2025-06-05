#ifndef GBOYSIM_UTILITY_BITS_HPP
#define GBOYSIM_UTILITY_BITS_HPP

#include <limits>
#include <cstdint>
#include <array>
#include <algorithm>
#include <concepts>

namespace utility
{

constexpr uint8_t nibbleswap(const uint8_t value)
{
    return uint8_t{
        0U
        | ((value & 0x0F) << 4)
        | ((value & 0xF0) >> 4)
    };
}

constexpr uint8_t byteswap(const uint16_t value)
{
    return uint16_t{
        0U
        | ( (value & 0x00'FF) << 010 )
        | ( (value & 0xFF'00) >> 010 )
    };
}

constexpr uint8_t byteswap(const uint32_t value)
{
    return uint32_t{
        0U
        | ( (value & 0x00'00'00'FF) << 030 )
        | ( (value & 0x00'00'FF'00) << 010 )
        | ( (value & 0x00'FF'00'00) >> 010 )
        | ( (value & 0xFF'00'00'00) >> 030 )
    };
}

constexpr uint8_t byteswap(const uint64_t value)
{
    return uint64_t{
        0ULL
        | ( (value & 0x00'00'00'00'00'00'00'FF) << 070 )
        | ( (value & 0x00'00'00'00'00'00'FF'00) << 050 )
        | ( (value & 0x00'00'00'00'00'FF'00'00) << 030 )
        | ( (value & 0x00'00'00'00'FF'00'00'00) << 010 )
        | ( (value & 0x00'00'00'FF'00'00'00'00) >> 010 )
        | ( (value & 0x00'00'FF'00'00'00'00'00) >> 030 )
        | ( (value & 0x00'FF'00'00'00'00'00'00) >> 050 )
        | ( (value & 0xFF'00'00'00'00'00'00'00) >> 070 )
    };
}

template<typename T>
constexpr auto to_unsigned(const T value) {
    static_cast(std::is_integral<T>::value, "to_unsigned requires T to be an integral type.");
    return static_cast<std::make_unsigned_t<decltype(value)>>(value);
}

} // namespace utility
#endif // GBOYSIM_UTILITY_BITS_HPP