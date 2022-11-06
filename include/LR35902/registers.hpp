#pragma once
#include <string_view>
#include <common/registers.hpp>

namespace LR35902::Definitions::Names
{

struct LR35902 { static constexpr std::string_view name = "LR35902"; };

namespace Flags
{
struct Z { static constexpr std::string_view name = "Z"; };
struct N { static constexpr std::string_view name = "N"; };
struct H { static constexpr std::string_view name = "H"; };
struct C { static constexpr std::string_view name = "C"; };
}

struct A { static constexpr std::string_view name = "A"; };
struct F { static constexpr std::string_view name = "F"; };
struct B { static constexpr std::string_view name = "B"; };
struct C { static constexpr std::string_view name = "C"; };
struct D { static constexpr std::string_view name = "D"; };
struct E { static constexpr std::string_view name = "E"; };
struct H { static constexpr std::string_view name = "H"; };
struct L { static constexpr std::string_view name = "L"; };

struct AF { static constexpr std::string_view name = "AF"; };
struct BC { static constexpr std::string_view name = "BC"; };
struct DE { static constexpr std::string_view name = "DE"; };
struct HL { static constexpr std::string_view name = "HL"; };
struct PC { static constexpr std::string_view name = "PC"; };
struct SP { static constexpr std::string_view name = "SP"; };


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
using DE = RegisterDef<Names::BC, uint16_t, 16, ::LR35902::BC, ::LR35902::B, ::LR35902::C>;
using HL = RegisterDef<Names::BC, uint16_t, 16, ::LR35902::BC, ::LR35902::B, ::LR35902::C>;

using PC = RegisterDef<Names::BC, uint16_t, 16, ::LR35902::BC>;
using SP = RegisterDef<Names::BC, uint16_t, 16, ::LR35902::BC>;

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
