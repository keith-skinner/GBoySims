#pragma once
#include <string_view>
#include <CPU/registers.hpp>
#include <sc/string_constant.hpp>
#include <names.hpp>

namespace LR35902
{

namespace Register
{
template <typename Name, std::size_t Msb, std::size_t Lsb>
using CPU::Register<Name, Msb, Lsb>; // Range Lsb to Msb is inclusive

// Single Bit flags
namespace Flags {
using Z = Register<Name::Flag::Z, 7, 7>;
using N = Register<Name::Flag::N, 6, 6>;
using H = Register<Name::Flag::H, 5, 5>;
using C = Register<Name::Flag::C, 4, 4>;
} // namespace Flags


// 8 Bit Registers
using A = Register<Name::Register::A, 15, 8>;
using F = Register<Name::Register::F,  7, 0>;
using B = Register<Name::Register::B, 15, 8>;
using C = Register<Name::Register::C,  7, 0>;
using D = Register<Name::Register::D, 15, 8>;
using E = Register<Name::Register::E,  7, 0>;
using H = Register<Name::Register::H, 15, 8>;
using L = Register<Name::Register::L,  7, 0>;


// 16 bit Registers
using AF = Register<Name::Register::AF, 15, 0>;
using BC = Register<Name::Register::BC, 15, 0>;
using DE = Register<Name::Register::DE, 15, 0>;
using HL = Register<Name::Register::HL, 15, 0>;
using PC = Register<Name::Register::PC, 15, 0>;
using SP = Register<Name::Register::SP, 15, 0>;
} // namespace Register


using RegisterFile = CPU::RegisterFile<Name::LR35902,
    CPU::RegisterDef<uint16_t, 16, LR35902::Register::AF, LR35902::Register::A, LR35902::Register::F,
        LR35902::Register::Flags::Z, LR35902::Register::Flags::N, LR35902::Register::Flags::H, LR35902::Register::Flags::C>,
    CPU::RegisterDef<uint16_t, 16, LR35902::Register::BC, LR35902::Register::B, LR35902::Register::C>,
    CPU::RegisterDef<uint16_t, 16, LR35902::Register::DE, LR35902::Register::D, LR35902::Register::E>,
    CPU::RegisterDef<uint16_t, 16, LR35902::Register::HL, LR35902::Register::H, LR35902::Register::L>,
    CPU::RegisterDef<uint16_t, 16, LR35902::Register::PC>,
    CPU::RegisterDef<uint16_t, 16, LR35902::Register::SP>
>;
} // namespace LR35902
