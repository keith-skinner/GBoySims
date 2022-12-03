#pragma once
#include <common/arguments.hpp>
#include "names.hpp"

/***
 * The intention of this file is to create a system for the multiple types of arguments that can appear for opcodes
 *   - An immediate that is not a reference to an address
 *   - An immediate that is a reference to an absoulte address
 *   - An immediate that is a reference to a zero-page address
 *   - A register that is a not a reference to an address
 *   - A register that is a reference to an absolute address
 *   - A register that is a reference to a zero-page address
*/

//Access is either Immediate or via Reference
namespace LR35902::Access
{

using common::Arguments::Access::a_immediate;
using common::Arguments::Access::a_reference;
template<typename T>
concept a_arg_access = a_immediate<T> || a_reference<T>;

using Immediate = common::Arguments::Access::immediate_t<uint16_t>;
using Reference = common::Arguments::Access::reference_t<uint16_t, uint16_t, 0x0000>;
using zReference = common::Arguments::Access::reference_t<uint16_t, uint16_t, 0xFF00>;


} // namespace LR35902::Access
namespace LR35902::Type
{


template<typename NameT, typename T>
using Immediate = common::Arguments::Type::immediate_t<NameT, T>;
using common::Arguments::Type::a_immediate;

template<typename NameT, std::unsigned_integral TypeT>
struct Register
:   common::Arguments::Type::register_t<NameT, TypeT>
{
    using Name = NameT;
    using Type = TypeT;
    static constexpr Name name{};
};

// Immediate Data Types
using S8  = Immediate<Name::Literal::S8, std::int8_t>;
using D8  = Immediate<Name::Literal::D8, std::uint8_t>;
using A8  = Immediate<Name::Literal::A8, std::uint8_t>;
using A16 = Immediate<Name::Literal::A16, std::uint16_t>;
using D16 = Immediate<Name::Literal::D16, std::uint16_t>;

// 8-Bit registers
using A = Register<Name::Register::A, std::uint8_t>;
using F = Register<Name::Register::F, std::uint8_t>;
using B = Register<Name::Register::B, std::uint8_t>;
using C = Register<Name::Register::C, std::uint8_t>;
using D = Register<Name::Register::D, std::uint8_t>;
using E = Register<Name::Register::E, std::uint8_t>;
using H = Register<Name::Register::H, std::uint8_t>;
using L = Register<Name::Register::L, std::uint8_t>;

// 16-Bit registers
using AF = Register<Name::Register::AF, std::uint16_t>;
using BC = Register<Name::Register::BC, std::uint16_t>;
using DE = Register<Name::Register::DE, std::uint16_t>;
using HL = Register<Name::Register::HL, std::uint16_t>;
using PC = Register<Name::Register::PC, std::uint16_t>;
using SP = Register<Name::Register::SP, std::uint16_t>;

template<typename T, typename Name = typename T::Name>
static constexpr bool is_register8_v = std::is_same_v<T, Register<Name, std::uint8_t>>;
template<typename T>
concept a_register8 = is_register8_v<T>;

template<typename T, typename Name = typename T::Name>
static constexpr bool is_register16_v = std::is_same_v<T, Register<Name, std::uint16_t>>;
template<typename T>
concept a_register16 = is_register16_v<T>;

template<typename T>
concept a_register = a_register8<T> || a_register16<T>;

template <typename T>
concept a_arg_type = a_immediate<T> || a_register<T>;


} // namespace LR35902::Type
//Type is a merging of Access and Type
namespace LR35902::Args
{

constexpr bool INC = true;
constexpr bool DEC = false;

template<LR35902::Type::a_arg_type T, LR35902::Access::a_arg_access A>
using Argument = common::Arguments::argument_t<T, A>;

//LR35902 Specific
// 8-bit register arguments
using A = Argument<Type::A, Access::Immediate>;
using B = Argument<Type::B, Access::Immediate>;
using C = Argument<Type::C, Access::Immediate>;
using D = Argument<Type::D, Access::Immediate>;
using E = Argument<Type::E, Access::Immediate>;
using H = Argument<Type::H, Access::Immediate>;
using L = Argument<Type::L, Access::Immediate>;

// 16-bit register arguments
using AF = Argument<Type::AF, Access::Immediate>;
using BC = Argument<Type::BC, Access::Immediate>;
using DE = Argument<Type::DE, Access::Immediate>;
using HL = Argument<Type::HL, Access::Immediate>;
using SP = Argument<Type::SP, Access::Immediate>;
using PC = Argument<Type::PC, Access::Immediate>;

// 8-bit register references
using zC = Argument<Type::C, Access::zReference>;

// 16-bit register references
using rBC = Argument<Type::BC, Access::Reference>;
using rDE = Argument<Type::DE, Access::Reference>;
using rHL = Argument<Type::HL, Access::Reference>;
using rPC = Argument<Type::PC, Access::Reference>;
using rSP = Argument<Type::SP, Access::Reference>;

// immediate arguments
using S8  = Argument<Type::S8 , Access::Immediate>;
using D8  = Argument<Type::D8 , Access::Immediate>;
using A16 = Argument<Type::A16, Access::Immediate>;
using D16 = Argument<Type::D16, Access::Immediate>;

// immediate reference arguments
using zA8  = Argument<Type::A8 , Access::zReference>;
using rA16 = Argument<Type::A16, Access::Reference> ;


} // namespce LR35902::Args
