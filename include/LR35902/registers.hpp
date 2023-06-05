#pragma once
#include <string_view>
#include <common/registers.hpp>
#include <sc/string_constant.hpp>
#include <names.hpp>

namespace LR35902
{

template <typename Name, typename Repr, std::size_t Msb, std::size_t Lsb>
using Register = common::register_t<Name, Repr, Msb, Lsb>; // Range Lsb to Msb is inclusive

// Single Bit flags
namespace Flags
{
using Z = Register<Name::Flag::Z, bool, 7, 7>;
using N = Register<Name::Flag::N, bool, 6, 6>;
using H = Register<Name::Flag::H, bool, 5, 5>;
using C = Register<Name::Flag::C, bool, 4, 4>;
}

// 8 Bit Registers
using A = Register<Name::Register::A, std::uint8_t, 15, 8>;
using F = Register<Name::Register::F, std::uint8_t,  7, 0>;
using B = Register<Name::Register::B, std::uint8_t, 15, 8>;
using C = Register<Name::Register::C, std::uint8_t,  7, 0>;
using D = Register<Name::Register::D, std::uint8_t, 15, 8>;
using E = Register<Name::Register::E, std::uint8_t,  7, 0>;
using H = Register<Name::Register::H, std::uint8_t, 15, 8>;
using L = Register<Name::Register::L, std::uint8_t,  7, 0>;

// 16 bit Registers
using AF = Register<Name::Register::AF, std::uint16_t, 15, 0>;
using BC = Register<Name::Register::BC, std::uint16_t, 15, 0>;
using DE = Register<Name::Register::DE, std::uint16_t, 15, 0>;
using HL = Register<Name::Register::HL, std::uint16_t, 15, 0>;
using PC = Register<Name::Register::PC, std::uint16_t, 15, 0>;
using SP = Register<Name::Register::SP, std::uint16_t, 15, 0>;


} // namespace LR35902
namespace LR35902::Definitions
{

template <typename Name, typename Repr, std::size_t Width, typename ... SubRegisters>
using RegisterDef = common::registerdef_t<Name, Repr, Width, SubRegisters...>;

using AF = RegisterDef<Name::Register::AF, uint16_t, 16, ::LR35902::AF, ::LR35902::A, ::LR35902::F, ::LR35902::Flags::Z, ::LR35902::Flags::N, ::LR35902::Flags::H, ::LR35902::Flags::C>;
using BC = RegisterDef<Name::Register::BC, uint16_t, 16, ::LR35902::BC, ::LR35902::B, ::LR35902::C>;
using DE = RegisterDef<Name::Register::DE, uint16_t, 16, ::LR35902::DE, ::LR35902::D, ::LR35902::E>;
using HL = RegisterDef<Name::Register::HL, uint16_t, 16, ::LR35902::HL, ::LR35902::H, ::LR35902::L>;
using PC = RegisterDef<Name::Register::PC, uint16_t, 16, ::LR35902::PC>;
using SP = RegisterDef<Name::Register::SP, uint16_t, 16, ::LR35902::SP>;

} // namespace LR35902::Defintions
namespace LR35902
{

using RegisterFile = common::registerfile_t<
    Name::LR35902,
    Definitions::AF,
    Definitions::BC,
    Definitions::DE,
    Definitions::HL,
    Definitions::PC,
    Definitions::SP>;


} // namespace LR35902

