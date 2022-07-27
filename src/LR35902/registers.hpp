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

    constexpr static std::underlying_type<Flags>::type to_value(Flags f) noexcept
    {
        return static_cast<std::underlying_type<Flags>::type>(f);
    }

    template<std::size_t B /*bit-depth*/, std::size_t O/*offset*/>
    struct Register 
    {
        using size_t = bytes_to_uint_t<bits_to_bytes_v<B>>;
    };

    template<typename Z, typename N, typename H, typename C>
    constexpr void flags(const Z z, const N n, const H h, const C c);

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

template<typename T> constexpr
std::underlying_type<Registers::Flags>::type 
set_mask(const Registers::Flags f, const T v);

template<> constexpr
std::underlying_type<Registers::Flags>::type
set_mask<int>(const Registers::Flags f, const int v) 
{ 
    return v == 1 ? Registers::to_value(f) : 0;
}

template<> constexpr
std::underlying_type<Registers::Flags>::type
set_mask<bool>(const Registers::Flags f, const bool v)
{
    return v ? Registers::to_value(f) : 0;
}

template<> constexpr
std::underlying_type<Registers::Flags>::type
set_mask<char>(const Registers::Flags, const char)
{
    return 0;
}

template<typename T> constexpr
std::underlying_type<Registers::Flags>::type
leave_mask(const Registers::Flags f, const T v);

template<> constexpr
std::underlying_type<Registers::Flags>::type
leave_mask<int>(const Registers::Flags, const int) 
{ 
    return 0;
}

template<> constexpr
std::underlying_type<Registers::Flags>::type
leave_mask<bool>(const Registers::Flags, const bool)
{
    return 0;
}

template<> constexpr
std::underlying_type<Registers::Flags>::type
leave_mask<char>(const Registers::Flags f, const char)
{
    return Registers::to_value(f);
}

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

constexpr unsigned int get_bit(uint8_t n, int b)
{
    uint8_t t1 = n << (7-b);
    uint8_t t2 = t1 >> 7;
    return t2;
}

template<typename Z, typename N, typename H, typename C>
constexpr void Registers::flags(const Z z, const N n, const H h, const C c)
{
    using namespace Register;
    using flags_t = std::underlying_type<Flags>::type;

    const flags_t set = set_mask(Flags::Z, z) | set_mask(Flags::N, n) | set_mask(Flags::H, h) | set_mask(Flags::C, c);
    const flags_t leave = leave_mask(Flags::Z, z) | leave_mask(Flags::N, n) | leave_mask(Flags::H, h) | leave_mask(Flags::C, c);
    write(F{}, (read(F{}) & leave) | set);
}


}