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

constexpr auto byteswap(std::unsigned_integral auto v) noexcept ->decltype(v)
{
    uint8_t * result = reinterpret_cast<uint8_t *>(&v);
    for (size_t i = 0; i<sizeof(result)/2; ++i)
        std::swap(result[i], result[sizeof(result)-1-i]);
    return v;
}

bool is_native_endian(std::endian v)
{
    return std::endian::native == v;
}

constexpr auto to_little_endian(std::unsigned_integral auto v) -> decltype(v)
{
    if (!is_native_endian(std::endian::little))
        return byteswap(v);
    return v;
}