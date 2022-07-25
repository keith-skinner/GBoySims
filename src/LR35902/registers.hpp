#pragma once
#include <cstdint>
#include <type_traits>
#include <cstddef>

#include "utility/bits.hpp"

namespace LR35902
{

enum class Flags : uint8_t
{
    Z = 0b1000'0000,
    N = 0b0100'0000,
    H = 0b0010'0000,
    C = 0b0001'0000,
};

constexpr std::underlying_type<Flags>::type to_value(Flags f) noexcept {
    return static_cast<std::underlying_type<Flags>::type>(f);
}

template<std::size_t B /*bit-depth*/, std::size_t O/*offset*/>
struct Register 
{
    using size_t = bytes_to_uint_t<bits_to_bytes_v<B>>;
};

struct Registers
{

    template<size_t Bits, size_t Offset>
    constexpr typename Register<Bits, Offset>::size_t read(const Register<Bits, Offset>) const
    {
        const uintptr_t pos = reinterpret_cast<uintptr_t>(this) + Offset;
        return *reinterpret_cast<typename Register<Bits, Offset>::size_t *>(pos);
    }

    template<size_t Bits, size_t Offset>
    constexpr void write(const Register<Bits, Offset>, const typename Register<Bits, Offset>::size_t value)
    {
        const uintptr_t pos = reinterpret_cast<uintptr_t>(this) + Offset;
        *(reinterpret_cast<typename Register<Bits, Offset>::size_t *>(pos)) = value;
    }

    uint16_t AF = 0;
    uint16_t BC = 0;
    uint16_t DE = 0;
    uint16_t HL = 0;
};

struct A : Register<8, offsetof(Registers, AF) + 0ULL>{};
struct F : Register<8, offsetof(Registers, AF) + 1ULL>{};
struct B : Register<8, offsetof(Registers, BC) + 0ULL>{};
struct C : Register<8, offsetof(Registers, BC) + 1ULL>{};
struct D : Register<8, offsetof(Registers, DE) + 0ULL>{};
struct E : Register<8, offsetof(Registers, DE) + 1ULL>{};
struct H : Register<8, offsetof(Registers, HL) + 0ULL>{};
struct L : Register<8, offsetof(Registers, HL) + 1ULL>{};

struct AF : Register<16, offsetof(Registers, AF)>{};
struct BC : Register<16, offsetof(Registers, BC)>{};
struct DE : Register<16, offsetof(Registers, DE)>{};
struct HL : Register<16, offsetof(Registers, HL)>{};

}