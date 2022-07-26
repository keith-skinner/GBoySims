#pragma once
#include <cstdint>
#include <type_traits>
#include <cstddef>

#include "utility/bits.hpp"

namespace LR35902
{

struct Registers
{
    enum class Flags : uint8_t
    {
        Z = 0b1000'0000,
        N = 0b0100'0000,
        H = 0b0010'0000,
        C = 0b0001'0000,
    };

    constexpr std::underlying_type<Flags>::type to_value(Flags f) noexcept
    {
        return static_cast<std::underlying_type<Flags>::type>(f);
    }

    template<std::size_t B /*bit-depth*/, std::size_t O/*offset*/>
    struct Register 
    {
        using size_t = bytes_to_uint_t<bits_to_bytes_v<B>>;
    };

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
    uint16_t PC = 0;
    uint16_t SP = 0;
};

namespace Register
{

struct A : Registers::Register<8, offsetof(Registers, AF) + 0ULL> {};
struct F : Registers::Register<8, offsetof(Registers, AF) + 1ULL> {};
struct B : Registers::Register<8, offsetof(Registers, BC) + 0ULL> {};
struct C : Registers::Register<8, offsetof(Registers, BC) + 1ULL> {};
struct D : Registers::Register<8, offsetof(Registers, DE) + 0ULL> {};
struct E : Registers::Register<8, offsetof(Registers, DE) + 1ULL> {};
struct H : Registers::Register<8, offsetof(Registers, HL) + 0ULL> {};
struct L : Registers::Register<8, offsetof(Registers, HL) + 1ULL> {};

struct AF : Registers::Register<16, offsetof(Registers, AF)> {};
struct BC : Registers::Register<16, offsetof(Registers, BC)> {};
struct DE : Registers::Register<16, offsetof(Registers, DE)> {};
struct HL : Registers::Register<16, offsetof(Registers, HL)> {};
struct PC : Registers::Register<16, offsetof(Registers, PC)> {};
struct SP : Registers::Register<16, offsetof(Registers, SP)> {};

template<typename R> static constexpr bool is_register8_v = 
       std::is_same_v<R, A>
    || std::is_same_v<R, F>
    || std::is_same_v<R, B>
    || std::is_same_v<R, C>
    || std::is_same_v<R, D>
    || std::is_same_v<R, E>
    || std::is_same_v<R, H>
    || std::is_same_v<R, L>;

template<typename R> static constexpr bool is_register16_v = 
       std::is_same_v<R, AF>
    || std::is_same_v<R, BC>
    || std::is_same_v<R, DE>
    || std::is_same_v<R, HL>
    || std::is_same_v<R, PC>
    || std::is_same_v<R, SP>;

template<typename R> static constexpr bool is_register_v = 
       is_register8_v<R>
    || is_register16_v<R>;

}




}