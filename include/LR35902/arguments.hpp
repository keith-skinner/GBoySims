#pragma once
#include <concepts>
#include <common/arguments.hpp>
#include <variant>
#include "registers.hpp"
#include "names.hpp"

/***
 * The intention of this file is to create a system for the multiple types of arguments that can appear for opcodes.
 * The groups of opcode arguments include:
 *   - An immediate value
 *   - An immediate value that is a reference to an absolute address
 *   - An immediate value that is a reference to a zero-page address
 * 
 *   - A register
 *   - A register that holds a reference to an absolute address
 *   - A register that holds a reference to a zero-page address
*/

//Access is either Immediate or via Reference
namespace LR35902::Instructions
{

namespace Access {
template<std::integral DataT>
using Immediate  = CPU::Arguments::Access::Immediate<DataT>;
using Reference  = CPU::Arguments::Access::Reference<uint16_t, 0x0000>;
using zReference = CPU::Arguments::Access::Reference<uint16_t, 0xFF00>;

template<typename A>
concept a_reference =
    std::isSame<A, Access::Reference>
||  std::isSame<A, Access::zReference>;

} // namespace Access


namespace Source {
using CPU::Arguments::Source::HardCoded;
using CPU::Arguments::Source::Register;
} // namespace Source


namespace Arguments
{
// 8-bit register arguments
using A = Argument<Name::Register::A, Source::Register, Access::Immediate<uint8_t>>;
using F = Argument<Name::Register::F, Source::Register, Access::Immediate<uint8_t>>;
using B = Argument<Name::Register::B, Source::Register, Access::Immediate<uint8_t>>;
using C = Argument<Name::Register::C, Source::Register, Access::Immediate<uint8_t>>;
using D = Argument<Name::Register::D, Source::Register, Access::Immediate<uint8_t>>;
using E = Argument<Name::Register::E, Source::Register, Access::Immediate<uint8_t>>;
using H = Argument<Name::Register::H, Source::Register, Access::Immediate<uint8_t>>;
using L = Argument<Name::Register::L, Source::Register, Access::Immediate<uint8_t>>;

// 16-bit register arguments
using AF = Argument<Name::Register::AF, Source::Register, Access::Immediate<uint16_t>>;
using BC = Argument<Name::Register::BC, Source::Register, Access::Immediate<uint16_t>>;
using DE = Argument<Name::Register::DE, Source::Register, Access::Immediate<uint16_t>>;
using HL = Argument<Name::Register::HL, Source::Register, Access::Immediate<uint16_t>>;
using SP = Argument<Name::Register::SP, Source::Register, Access::Immediate<uint16_t>>;
using PC = Argument<Name::Register::PC, Source::Register, Access::Immediate<uint16_t>>;

// 8-bit register references
using zC = Argument<Name::Register::C, Source::Register, Access::zReference>;

// 8-bit immediate references
using zA8  = Argument<Name::Literal::A8,  Source::HardCoded, Access::zReference>;
using rA16 = Argument<Name::Literal::A16, Source::HardCoded, Access::Reference>;

// 16-bit register references
using rBC = Argument<Name::Register::BC, Source::Register, Access::Reference>;
using rDE = Argument<Name::Register::DE, Source::Register, Access::Reference>;
using rHL = Argument<Name::Register::HL, Source::Register, Access::Reference>;
using rPC = Argument<Name::Register::PC, Source::Register, Access::Reference>;
using rSP = Argument<Name::Register::SP, Source::Register, Access::Reference>;

// immediate arguments
using S8  = Argument<Name::Literal::S8,  Source::HardCoded, Access::Immediate<int8_t>;
using D8  = Argument<Name::Literal::D8,  Source::HardCoded, Access::Immediate<uint8_t>;
using D16 = Argument<Name::Literal::D16, Source::HardCoded, Access::Immediate<uint16_t>;

template<template<typename Name, typename Source, typename Access> class Arg>
concept a_argument = std::is_same_v<Arg, Argument<Name, Source, Access>>;

template<template<typename Name, typename S, typename A> class Arg>
concept a_destination = a_argument<Arg> && !(std::is_same_v<S, Source::HardCoded> && std::is_same_v<A, Access::Immediate>);

template<template<typename N, typename S, typename A> class Arg>
concept a_reference = a_argument<Arg> && Access::a_reference<A>;

template<typename Arg>
concept a_r16 = 
    std::is_same_v<Arg, BC>
||  std::is_same_v<Arg, DE>
||  std::is_same_v<Arg, HL>
||  std::is_same_v<Arg, SP>;

template<typename Arg>
concept a_r8 =
    std::is_same_v<Arg, B>
||  std::is_same_v<Arg, C>
||  std::is_same_v<Arg, D>
||  std::is_same_v<Arg, E>
||  std::is_same_v<Arg, H>
||  std::is_same_v<Arg, L>
||  std::is_same_v<Arg, A>;

template<typename Arg>
concept a_r8_or_rHL =
    a_r8<Arg>
||  std::is_same_v<Arg, rHL>;


} // namespace Arguments

} // namespace LR35902::Instructions
