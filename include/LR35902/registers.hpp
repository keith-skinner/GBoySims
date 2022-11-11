#pragma once
#include <string_view>
#include <common/registers.hpp>
#include <sc/string_constant.hpp>

namespace LR35902::Definitions::Names
{

using LR35902 = decltype("LR35902"_sc);

namespace Flags
{
using Z = decltype("Flags::Z"_sc);
using N = decltype("Flags::N"_sc);
using H = decltype("Flags::H"_sc);
using C = decltype("Flags::C"_sc);
}

using A = decltype("A"_sc);
using F = decltype("F"_sc);
using B = decltype("B"_sc);
using C = decltype("C"_sc);
using D = decltype("D"_sc);
using E = decltype("E"_sc);
using H = decltype("H"_sc);
using L = decltype("L"_sc);

using AF = decltype("AF"_sc);
using BC = decltype("BC"_sc);
using DE = decltype("DE"_sc);
using HL = decltype("HL"_sc);
using PC = decltype("PC"_sc);
using SP = decltype("SP"_sc);


} // namespace LR35902::Definitions::Names
namespace LR35902
{


template <typename Name, typename Repr, std::size_t Msb, std::size_t Lsb>
using Register = common::register_t<Name, Repr, Msb, Lsb>;

// Single Bit flags
namespace Flags 
{
using Z = Register<Definitions::Names::Flags::Z, bool, 7, 7>;
using N = Register<Definitions::Names::Flags::N, bool, 6, 6>;
using H = Register<Definitions::Names::Flags::H, bool, 5, 5>;
using C = Register<Definitions::Names::Flags::C, bool, 4, 4>;
}

// 8 Bit Registers
using A = Register<Definitions::Names::A, std::uint8_t, 15, 8>;
using F = Register<Definitions::Names::F, std::uint8_t,  7, 0>;
using B = Register<Definitions::Names::B, std::uint8_t, 15, 8>;
using C = Register<Definitions::Names::C, std::uint8_t,  7, 0>;
using D = Register<Definitions::Names::D, std::uint8_t, 15, 8>;
using E = Register<Definitions::Names::E, std::uint8_t,  7, 0>;
using H = Register<Definitions::Names::H, std::uint8_t, 15, 8>;
using L = Register<Definitions::Names::L, std::uint8_t,  7, 0>;

// 16 bit Registers
using AF = Register<Definitions::Names::AF, std::uint16_t, 15, 0>;
using BC = Register<Definitions::Names::BC, std::uint16_t, 15, 0>;
using DE = Register<Definitions::Names::DE, std::uint16_t, 15, 0>;
using HL = Register<Definitions::Names::HL, std::uint16_t, 15, 0>;
using PC = Register<Definitions::Names::PC, std::uint16_t, 15, 0>;
using SP = Register<Definitions::Names::SP, std::uint16_t, 15, 0>;


} // namespace LR35902
namespace LR35902::Definitions
{

template <typename Name, typename Repr, std::size_t Width, typename ... SubRegisters>
using RegisterDef = common::registerdef_t<Name, Repr, Width, SubRegisters...>;
using AF = RegisterDef<Names::AF, uint16_t, 16, ::LR35902::AF, ::LR35902::A, ::LR35902::F, 
    ::LR35902::Flags::Z, ::LR35902::Flags::N, ::LR35902::Flags::H, ::LR35902::Flags::C>;
using BC = RegisterDef<Names::BC, uint16_t, 16, ::LR35902::BC, ::LR35902::B, ::LR35902::C>;
using DE = RegisterDef<Names::DE, uint16_t, 16, ::LR35902::DE, ::LR35902::D, ::LR35902::E>;
using HL = RegisterDef<Names::HL, uint16_t, 16, ::LR35902::HL, ::LR35902::H, ::LR35902::L>;

using PC = RegisterDef<Names::PC, uint16_t, 16, ::LR35902::PC>;
using SP = RegisterDef<Names::SP, uint16_t, 16, ::LR35902::SP>;

} // namespace LR35902::Defintions
namespace LR35902
{


using RegisterFile = common::registerfile_t<
    Definitions::Names::LR35902, 
    Definitions::AF, 
    Definitions::BC, 
    Definitions::DE, 
    Definitions::HL, 
    Definitions::PC, 
    Definitions::SP>;


} // namespace LR35902
